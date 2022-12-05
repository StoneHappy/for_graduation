#include "RCScheduler.h"
#include <Renderer/Mesh.h>
namespace GU
{
	RCScheduler::RCScheduler()
	{
		m_ctx = new BuildContext();
	}
	void RCScheduler::handelConfig(rcConfig rcconfig, Mesh* mesh)
	{
		createRCMesh(mesh, rcmesh);
		rc_cfg = rcconfig;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(rcconfig.bmin, mesh->bmin);
		rcVcopy(rcconfig.bmax, mesh->bmax);
		rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);
	}
	bool RCScheduler::handelBuild()
	{
		rcCalcBounds(rcmesh.getVerts(), rcmesh.getVertCount(), m_meshBMin, m_meshBMax);

		const float* bmin = m_meshBMin;
		const float* bmax = m_meshBMax;
		const float* verts = rcmesh.getVerts();
		const int nverts = rcmesh.getVertCount();
		const int* tris = rcmesh.getTris();
		const int ntris = rcmesh.getTriCount();
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