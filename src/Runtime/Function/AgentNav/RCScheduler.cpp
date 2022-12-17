#include "RCScheduler.h"
#include <Renderer/Mesh.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>
#include <Recast.h>
#include <Function/AgentNav/RCData.h>
#include <MainWindow.h>
#include <Function/AgentNav/ChunkyTriMesh.h>
namespace GU
{
	static bool isectSegAABB(const float* sp, const float* sq,
		const float* amin, const float* amax,
		float& tmin, float& tmax)
	{
		static const float EPS = 1e-6f;

		float d[3];
		d[0] = sq[0] - sp[0];
		d[1] = sq[1] - sp[1];
		d[2] = sq[2] - sp[2];
		tmin = 0.0;
		tmax = 1.0f;

		for (int i = 0; i < 3; i++)
		{
			if (fabsf(d[i]) < EPS)
			{
				if (sp[i] < amin[i] || sp[i] > amax[i])
					return false;
			}
			else
			{
				const float ood = 1.0f / d[i];
				float t1 = (amin[i] - sp[i]) * ood;
				float t2 = (amax[i] - sp[i]) * ood;
				if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
				if (t1 > tmin) tmin = t1;
				if (t2 < tmax) tmax = t2;
				if (tmin > tmax) return false;
			}
		}

		return true;
	}

	static bool intersectSegmentTriangle(const float* sp, const float* sq,
		const float* a, const float* b, const float* c,
		float& t)
	{
		float v, w;
		float ab[3], ac[3], qp[3], ap[3], norm[3], e[3];
		rcVsub(ab, b, a);
		rcVsub(ac, c, a);
		rcVsub(qp, sp, sq);

		// Compute triangle normal. Can be precalculated or cached if
		// intersecting multiple segments against the same triangle
		rcVcross(norm, ab, ac);

		// Compute denominator d. If d <= 0, segment is parallel to or points
		// away from triangle, so exit early
		float d = rcVdot(qp, norm);
		if (d <= 0.0f) return false;

		// Compute intersection t value of pq with plane of triangle. A ray
		// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
		// dividing by d until intersection has been found to pierce triangle
		rcVsub(ap, sp, a);
		t = rcVdot(ap, norm);
		if (t < 0.0f) return false;
		if (t > d) return false; // For segment; exclude this code line for a ray test

		// Compute barycentric coordinate components and test if within bounds
		rcVcross(e, qp, ap);
		v = rcVdot(ac, e);
		if (v < 0.0f || v > d) return false;
		w = -rcVdot(ab, e);
		if (w < 0.0f || v + w > d) return false;

		// Segment/ray intersects triangle. Perform delayed division
		t /= d;

		return true;
	}


	RCScheduler::RCScheduler()
	{
		m_ctx = new BuildContext();
		m_navQuery = dtAllocNavMeshQuery();
		m_crowd = dtAllocCrowd();
	}
	bool RCScheduler::handelBuild(const RCParams& rcparams, Mesh* mesh)
	{
		GLOBAL_MAINWINDOW->progressBegin(7);
		GLOBAL_MAINWINDOW->setStatus(QString::fromLocal8Bit("开始处理导航网格"));
		createRCMesh(mesh, m_mesh);

		m_chunkyMesh = new rcChunkyTriMesh();
		if (!m_chunkyMesh)
		{
			return false;
		}
		if (!rcCreateChunkyTriMesh(m_mesh.getVerts(), m_mesh.getTris(), m_mesh.getTriCount(), 256, m_chunkyMesh))
		{
			return false;
		}

		float bmin[3];
		float bmax[3];
		const float* verts = m_mesh.getVerts();
		const int nverts = m_mesh.getVertCount();
		const int* tris = m_mesh.getTris();
		const int ntris = m_mesh.getTriCount();
		rcCalcBounds(m_mesh.getVerts(), m_mesh.getVertCount(), bmin, bmax);
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

		GLOBAL_MAINWINDOW->progressTick();

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
		GLOBAL_MAINWINDOW->progressTick();
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

		GLOBAL_MAINWINDOW->progressTick();
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

		m_heightFieldSolid = new RCHeightfieldSolid(*m_solid);

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
		

		// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
		// There are 3 martitioning methods, each with some pros and cons:
		// 1) Watershed partitioning
		//   - the classic Recast partitioning
		//   - creates the nicest tessellation
		//   - usually slowest
		//   - partitions the heightfield into nice regions without holes or overlaps
		//   - the are some corner cases where this method creates produces holes and overlaps
		//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
		//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
		//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
		// 2) Monotone partioning
		//   - fastest
		//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
		//   - creates long thin polygons, which sometimes causes paths with detours
		//   * use this if you want fast navmesh generation
		// 3) Layer partitoining
		//   - quite fast
		//   - partitions the heighfield into non-overlapping regions
		//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
		//   - produces better triangles than monotone partitioning
		//   - does not have the corner cases of watershed partitioning
		//   - can be slow and create a bit ugly tessellation (still better than monotone)
		//     if you have large open areas with small obstacles (not a problem if you use tiles)
		//   * good choice to use for tiled navmesh with medium and small sized tiles

		if (rcparams.m_partitionType == SAMPLE_PARTITION_WATERSHED)
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(m_ctx, *m_chf))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
				return false;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
				return false;
			}
		}
		else if (rcparams.m_partitionType == SAMPLE_PARTITION_MONOTONE)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
				return false;
			}
		}
		else // SAMPLE_PARTITION_LAYERS
		{
			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea))
			{
				m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
				return false;
			}
		}

		GLOBAL_MAINWINDOW->progressTick();
		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		m_cset = rcAllocContourSet();
		if (!m_cset)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
			return false;
		}
		if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
			return false;
		}
		GLOBAL_MAINWINDOW->progressTick();
		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		m_pmesh = rcAllocPolyMesh();
		if (!m_pmesh)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
			return false;
		}
		if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
			return false;
		}

		GLOBAL_MAINWINDOW->progressTick();
		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		m_dmesh = rcAllocPolyMeshDetail();
		if (!m_dmesh)
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
			return false;
		}

		if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
			return false;
		}
		m_polymesh = new RCMesh(*m_dmesh);
		m_polyContourMesh = new RCContour(*m_dmesh);

		if (!rcparams.m_keepInterResults)
		{
			rcFreeCompactHeightfield(m_chf);
			m_chf = 0;
			rcFreeContourSet(m_cset);
			m_cset = 0;
		}

		// At this point the navigation mesh data is ready, you can access it from m_pmesh.
		// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

		//
		// (Optional) Step 8. Create Detour data from Recast poly mesh.
		//

		// The GUI may allow more max points per polygon than Detour can handle.
		// Only build the detour navmesh if we do not exceed the limit.
		if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			unsigned char* navData = 0;
			int navDataSize = 0;

			// Update poly flags from areas.
			for (int i = 0; i < m_pmesh->npolys; ++i)
			{
				if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
					m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

				if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
					m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
				}
				else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
				{
					m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
				}
			}


			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = m_pmesh->verts;
			params.vertCount = m_pmesh->nverts;
			params.polys = m_pmesh->polys;
			params.polyAreas = m_pmesh->areas;
			params.polyFlags = m_pmesh->flags;
			params.polyCount = m_pmesh->npolys;
			params.nvp = m_pmesh->nvp;
			params.detailMeshes = m_dmesh->meshes;
			params.detailVerts = m_dmesh->verts;
			params.detailVertsCount = m_dmesh->nverts;
			params.detailTris = m_dmesh->tris;
			params.detailTriCount = m_dmesh->ntris;
#if 0
			params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
			params.offMeshConRad = m_geom->getOffMeshConnectionRads();
			params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
			params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
			params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
			params.offMeshConUserID = m_geom->getOffMeshConnectionId();
			params.offMeshConCount = m_geom->getOffMeshConnectionCount();
#endif
			params.walkableHeight = rcparams.m_agentHeight;
			params.walkableRadius = rcparams.m_agentRadius;
			params.walkableClimb = rcparams.m_agentMaxClimb;
			rcVcopy(params.bmin, m_pmesh->bmin);
			rcVcopy(params.bmax, m_pmesh->bmax);
			params.cs = m_cfg.cs;
			params.ch = m_cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
				return false;
			}

			m_navMesh = dtAllocNavMesh();
			if (!m_navMesh)
			{
				dtFree(navData);
				m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
				return false;
			}

			dtStatus status;

			status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status))
			{
				dtFree(navData);
				m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
				return false;
			}

			status = m_navQuery->init(m_navMesh, 2048);
			if (dtStatusFailed(status))
			{
				m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
				return false;
			}
		}
		GLOBAL_MAINWINDOW->progressTick();
		GLOBAL_MAINWINDOW->progressEnd();
		m_ctx->stopTimer(RC_TIMER_TOTAL);



		// init crowd
		m_crowd->init(MAX_AGENTS, rcparams.m_agentRadius, m_navMesh);
		m_crowd->getEditableFilter(0)->setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);
		// Setup local avoidance params to different qualities.
		dtObstacleAvoidanceParams params;
		// Use mostly default settings, copy from dtCrowd.
		memcpy(&params, m_crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

		// Low (11)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 1;
		m_crowd->setObstacleAvoidanceParams(0, &params);

		// Medium (22)
		params.velBias = 0.5f;
		params.adaptiveDivs = 5;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 2;
		m_crowd->setObstacleAvoidanceParams(1, &params);

		// Good (45)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 2;
		params.adaptiveDepth = 3;
		m_crowd->setObstacleAvoidanceParams(2, &params);

		// High (66)
		params.velBias = 0.5f;
		params.adaptiveDivs = 7;
		params.adaptiveRings = 3;
		params.adaptiveDepth = 3;

		m_crowd->setObstacleAvoidanceParams(3, &params);


		return true;
	}

	void RCScheduler::handelRender(VkCommandBuffer cmdBuf, int currentImage)
	{
		if (m_polymesh == nullptr) return;

		

		if (isRenderDetailMesh)
		{
			// draw detailmesh
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipelineLayout, 0, 1, &GLOBAL_VULKAN_CONTEXT->rcDescriptorSets[currentImage], 0, nullptr);
			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipeline);
			VkBuffer vertexBuffers[] = { m_polymesh->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(m_polymesh->m_verts.size()), 1, 0, 0);
		}
		
		if (isRenderHeightField)
		{
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipelineLayout, 0, 1, &GLOBAL_VULKAN_CONTEXT->rcDescriptorSets[currentImage], 0, nullptr);
			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipeline);
			// heightfield
			VkBuffer solidVertexBuffers[] = { m_heightFieldSolid->vertexBuffer };
			VkDeviceSize solidOffsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, solidVertexBuffers, solidOffsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(m_heightFieldSolid->m_verts.size()), 1, 0, 0);
		}

		if (isRenderContour)
		{
			// draw contour
			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcContourPipeline);
			VkBuffer icontourVertexBuffers[] = { m_polyContourMesh->internalVertexBuffer };
			VkDeviceSize icontourOffsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, icontourVertexBuffers, icontourOffsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(m_polyContourMesh->internalVerts.size()), 1, 0, 0);

			VkBuffer econtourVertexBuffers[] = { m_polyContourMesh->externalVertexBuffer };
			VkDeviceSize econtourOffsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, econtourVertexBuffers, econtourOffsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(m_polyContourMesh->externalVerts.size()), 1, 0, 0);
		}
		
		
	}

	bool RCScheduler::raycastMesh(float* src, float* dst, float& tmin)
	{
		// Prune hit ray.
		float btmin, btmax;
		if (!isectSegAABB(src, dst, m_meshBMin, m_meshBMax, btmin, btmax))
			return false;
		float p[2], q[2];
		p[0] = src[0] + (dst[0] - src[0]) * btmin;
		p[1] = src[2] + (dst[2] - src[2]) * btmin;
		q[0] = src[0] + (dst[0] - src[0]) * btmax;
		q[1] = src[2] + (dst[2] - src[2]) * btmax;

		int cid[512];
		const int ncid = rcGetChunksOverlappingSegment(m_chunkyMesh, p, q, cid, 512);
		if (!ncid)
			return false;

		tmin = 1.0f;
		bool hit = false;
		const float* verts = m_mesh.getVerts();

		for (int i = 0; i < ncid; ++i)
		{
			const rcChunkyTriMeshNode& node = m_chunkyMesh->nodes[cid[i]];
			const int* tris = &m_chunkyMesh->tris[node.i * 3];
			const int ntris = node.n;

			for (int j = 0; j < ntris * 3; j += 3)
			{
				float t = 1;
				if (intersectSegmentTriangle(src, dst,
					&verts[tris[j] * 3],
					&verts[tris[j + 1] * 3],
					&verts[tris[j + 2] * 3], t))
				{
					if (t < tmin)
						tmin = t;
					hit = true;
				}
			}
		}

		return hit;
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