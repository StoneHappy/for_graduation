#include "RCScheduler.h"
#include <Renderer/Mesh.h>
namespace GU
{
	RCScheduler::RCScheduler()
	{
		m_ctx = new BuildContext();
	}
	bool RCScheduler::handelBuild(const RCParams& rcparams, Mesh* mesh)
	{
		createRCMesh(mesh, rcmesh);
		float bmin[3];
		float bmax[3];
		const float* verts = rcmesh.getVerts();
		const int nverts = rcmesh.getVertCount();
		const int* tris = rcmesh.getTris();
		const int ntris = rcmesh.getTriCount();
		rcCalcBounds(rcmesh.getVerts(), rcmesh.getVertCount(), bmin, bmax);
		rcVcopy(m_meshBMin, bmin);
		rcVcopy(m_meshBMax, bmax);
		//
		// Step 1. Initialize build config.
		//

		// Init build configuration from GUI
		memset(&m_cfg, 0, sizeof(m_cfg));
		m_cfg.cs = rcparams.m_cellSize;
		m_cfg.ch = rcparams.m_cellHeight;
		m_cfg.walkableSlopeAngle = rcparams.m_agentMaxSlope;
		m_cfg.walkableHeight = (int)ceilf(rcparams.m_agentHeight / m_cfg.ch);
		m_cfg.walkableClimb = (int)floorf(rcparams.m_agentMaxClimb / m_cfg.ch);
		m_cfg.walkableRadius = (int)ceilf(rcparams.m_agentRadius / m_cfg.cs);
		m_cfg.maxEdgeLen = (int)(rcparams.m_edgeMaxLen / rcparams.m_cellSize);
		m_cfg.maxSimplificationError = rcparams.m_edgeMaxError;
		m_cfg.minRegionArea = (int)rcSqr(rcparams.m_regionMinSize);		// Note: area = size*size
		m_cfg.mergeRegionArea = (int)rcSqr(rcparams.m_regionMergeSize);	// Note: area = size*size
		m_cfg.maxVertsPerPoly = (int)rcparams.m_vertsPerPoly;
		m_cfg.detailSampleDist = rcparams.m_detailSampleDist < 0.9f ? 0 : rcparams.m_cellSize * rcparams.m_detailSampleDist;
		m_cfg.detailSampleMaxError = rcparams.m_cellHeight * rcparams.m_detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(m_cfg.bmin, bmin);
		rcVcopy(m_cfg.bmax, bmax);
		rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

		// Reset build times gathering.
		m_ctx->resetTimers();

		// Start the build process.	
		m_ctx->startTimer(RC_TIMER_TOTAL);

		m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
		m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
		m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);


		// Step 2. Rasterize input polygon soup.
		//
		// Allocate voxel heightfield where we rasterize our input data to.
		m_solid = rcAllocHeightfield();
		if (!m_solid)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
			return false;
		}
		if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
			return false;
		}

		// Allocate array that can hold triangle area types.
		// If you have multiple meshes you need to process, allocate
		// and array which can hold the max number of triangles you need to process.
		m_triareas = new unsigned char[ntris];
		if (!m_triareas)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
			return false;
		}

		// Find triangles which are walkable based on their slope and rasterize them.
		// If your input data is multiple meshes, you can transform them here, calculate
		// the are type for each of the meshes and rasterize them.
		memset(m_triareas, 0, ntris * sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
		if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
			return false;
		}


		if (!rcparams.m_keepInterResults)
		{
			delete[] m_triareas;
			m_triareas = 0;
		}

		//
		// Step 3. Filter walkables surfaces.
		//

		// Once all geoemtry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (rcparams.m_filterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
		if (rcparams.m_filterLedgeSpans)
			rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
		if (rcparams.m_filterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);


		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		m_chf = rcAllocCompactHeightfield();
		if (!m_chf)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
			return false;
		}
		if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
			return false;
		}

		if (!rcparams.m_keepInterResults)
		{
			rcFreeHeightField(m_solid);
			m_solid = 0;
		}

		// Erode the walkable area by agent radius.
		if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
			return false;
		}
		
		return true;
	}
	void RCScheduler::createRCMesh(Mesh* mesh, rcMeshLoaderObj& rcMesh)
	{
		int vcap = 0;
		int fcap = 0;
		for (size_t i = 0; i < mesh->m_vertices.size(); i++)
		{
			rcMesh.addVertex(mesh->m_vertices[i].pos.x, mesh->m_vertices[i].pos.y, mesh->m_vertices[i].pos.z, vcap);
		}

		for (size_t i = 0; i < mesh->m_indices.size(); i += 3)
		{
			rcMesh.addTriangle(mesh->m_indices[i], mesh->m_indices[i + 1], mesh->m_indices[i + 2], fcap);
		}
	}
}