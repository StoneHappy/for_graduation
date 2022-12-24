#include "RCScheduler.h"
#include <Renderer/Mesh.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <DetourCrowd.h>
#include <DetourCommon.h>
#include <Recast.h>
#include <Function/AgentNav/RCData.h>
#include <MainWindow.h>
#include <Function/AgentNav/ChunkyTriMesh.h>
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Component.h>
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

	static void calcVel(float* vel, const float* pos, const float* tgt, const float speed)
	{
		dtVsub(vel, tgt, pos);
		vel[1] = 0.0;
		dtVnormalize(vel);
		dtVscale(vel, vel, speed);
	}

	static bool inRange(const float* v1, const float* v2, const float r, const float h)
	{
		const float dx = v2[0] - v1[0];
		const float dy = v2[1] - v1[1];
		const float dz = v2[2] - v1[2];
		return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
	}

	static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
		const float minTargetDist,
		const dtPolyRef* path, const int pathSize,
		float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
		float* outPoints = 0, int* outPointCount = 0)
	{
		// Find steer target.
		static const int MAX_STEER_POINTS = 3;
		float steerPath[MAX_STEER_POINTS * 3];
		unsigned char steerPathFlags[MAX_STEER_POINTS];
		dtPolyRef steerPathPolys[MAX_STEER_POINTS];
		int nsteerPath = 0;
		navQuery->findStraightPath(startPos, endPos, path, pathSize,
			steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
		if (!nsteerPath)
			return false;

		if (outPoints && outPointCount)
		{
			*outPointCount = nsteerPath;
			for (int i = 0; i < nsteerPath; ++i)
				dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
		}


		// Find vertex far enough to steer to.
		int ns = 0;
		while (ns < nsteerPath)
		{
			// Stop at Off-Mesh link or when point is further than slop away.
			if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
				!inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
				break;
			ns++;
		}
		// Failed to find good point to steer to.
		if (ns >= nsteerPath)
			return false;

		dtVcopy(steerPos, &steerPath[ns * 3]);
		steerPos[1] = startPos[1];
		steerPosFlag = steerPathFlags[ns];
		steerPosRef = steerPathPolys[ns];

		return true;
	}

	static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
		const dtPolyRef* visited, const int nvisited)
	{
		int furthestPath = -1;
		int furthestVisited = -1;

		// Find furthest common polygon.
		for (int i = npath - 1; i >= 0; --i)
		{
			bool found = false;
			for (int j = nvisited - 1; j >= 0; --j)
			{
				if (path[i] == visited[j])
				{
					furthestPath = i;
					furthestVisited = j;
					found = true;
				}
			}
			if (found)
				break;
		}

		// If no intersection found just return current path. 
		if (furthestPath == -1 || furthestVisited == -1)
			return npath;

		// Concatenate paths.	

		// Adjust beginning of the buffer to include the visited.
		const int req = nvisited - furthestVisited;
		const int orig = rcMin(furthestPath + 1, npath);
		int size = rcMax(0, npath - orig);
		if (req + size > maxPath)
			size = maxPath - req;
		if (size)
			memmove(path + req, path + orig, size * sizeof(dtPolyRef));

		// Store visited
		for (int i = 0; i < req; ++i)
			path[i] = visited[(nvisited - 1) - i];

		return req + size;
	}

	// This function checks if the path has a small U-turn, that is,
	// a polygon further in the path is adjacent to the first polygon
	// in the path. If that happens, a shortcut is taken.
	// This can happen if the target (T) location is at tile boundary,
	// and we're (S) approaching it parallel to the tile edge.
	// The choice at the vertex can be arbitrary, 
	//  +---+---+
	//  |:::|:::|
	//  +-S-+-T-+
	//  |:::|   | <-- the step can end up in here, resulting U-turn path.
	//  +---+---+
	static int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
	{
		if (npath < 3)
			return npath;

		// Get connected polygons
		static const int maxNeis = 16;
		dtPolyRef neis[maxNeis];
		int nneis = 0;

		const dtMeshTile* tile = 0;
		const dtPoly* poly = 0;
		if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
			return npath;

		for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
		{
			const dtLink* link = &tile->links[k];
			if (link->ref != 0)
			{
				if (nneis < maxNeis)
					neis[nneis++] = link->ref;
			}
		}

		// If any of the neighbour polygons is within the next few polygons
		// in the path, short cut to that polygon directly.
		static const int maxLookAhead = 6;
		int cut = 0;
		for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
			for (int j = 0; j < nneis; j++)
			{
				if (path[i] == neis[j]) {
					cut = i;
					break;
				}
			}
		}
		if (cut > 1)
		{
			int offset = cut - 1;
			npath -= offset;
			for (int i = 1; i < npath; i++)
				path[i] = path[i + offset];
		}

		return npath;
	}


	RCScheduler::RCScheduler()
	{
		m_ctx = new BuildContext();
		// navmesh
		m_navQuery = dtAllocNavMeshQuery();

		// crowd
		m_crowd = dtAllocCrowd();
		m_targetRef = 0;
		m_vod = dtAllocObstacleAvoidanceDebugData();
		m_vod->init(2048);
		memset(&m_agentDebug, 0, sizeof(m_agentDebug));
		m_agentDebug.idx = -1;
		m_agentDebug.vod = m_vod;
	}
	bool RCScheduler::handelBuild(const RCParams& rcparams, Mesh* mesh)
	{
		m_rcparams = rcparams;
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


		auto entity = GLOBAL_SCENE->createEntity();

		targetModelId = entity.getComponent<IDComponent>().ID;
		auto&& transform = entity.getComponent<TransformComponent>();
		transform.Translation = {-9999.0, -9999.0, -9999.0 };
		entity.addComponent<MaterialComponent>(GLOBAL_VULKAN_CONTEXT->targetModel, GLOBAL_VULKAN_CONTEXT->targetModelTexture);

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
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipelineLayout, 0, 1, &GLOBAL_VULKAN_CONTEXT->rcDescriptorSets[currentImage], 0, nullptr);
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
		

		for (size_t i = 0; i < rcAgentPath.size(); i++)
		{
			// draw contour
			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcContourPipeline);
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipelineLayout, 0, 1, &GLOBAL_VULKAN_CONTEXT->rcDescriptorSets[currentImage], 0, nullptr);
			VkBuffer vertexBuffers[] = { rcAgentPath[i]->vertexBuffer};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(rcAgentPath[i]->m_verts.size()), 1, 0, 0);
		}

		for (size_t i = 0; i < rcStraightPath.size(); i++)
		{
			// draw contour
			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcContourPipeline);
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->rcPipelineLayout, 0, 1, &GLOBAL_VULKAN_CONTEXT->rcDescriptorSets[currentImage], 0, nullptr);
			VkBuffer vertexBuffers[] = { rcStraightPath[i]->vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
			vkCmdDraw(cmdBuf, static_cast<uint32_t>(rcStraightPath[i]->m_verts.size()), 1, 0, 0);
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

	glm::vec3 RCScheduler::getAgentPosWithId(int idx)
	{
		auto agent = m_crowd->getAgent(idx);
		float x = agent->npos[0];
		float y = agent->npos[1];
		float z = agent->npos[2];
		return { x, y, z };
	}

	void RCScheduler::getAgentRotationWithId(int idx, glm::vec3& rotation)
	{
		auto agent = m_crowd->getAgent(idx);

		auto vel = agent->vel;
		auto glmvel = glm::vec3(vel[0], vel[1], vel[2]);
		
		if (glm::length(glmvel) == 0)
		{
			return;
		}
		glm::vec3 org = glm::normalize(glm::vec3(0, 0, 1.0));
		glm::vec3 dest = glm::normalize(glmvel);
		auto rot = glm::rotation(org, dest);
		auto euler = glm::eulerAngles(rot);

		rotation = euler;
	}

	int RCScheduler::addAgent(const glm::vec3& pos, const dtCrowdAgentParams& ap)
	{
		int idx = m_crowd->addAgent(glm::value_ptr(pos), &ap);
		if (idx != -1)
		{
			if (m_targetRef)
				m_crowd->requestMoveTarget(idx, m_targetRef, m_targetPos);
		}
		return idx;
	}

	float RCScheduler::getVelLength(int idx)
	{
		auto agent = m_crowd->getAgent(idx);
		auto vel = agent->vel;
		glm::vec3 glmvel = { vel[0], vel[1] , vel[2] };
		
		return glm::length(glmvel);
	}

	static inline int bit(int a, int b)
	{
		return (a & (1 << b)) >> b;
	}
	static inline unsigned int duRGBA(int r, int g, int b, int a)
	{
		return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
	}

	static unsigned int duIntToCol(int i, int a)
	{
		int	r = bit(i, 1) + bit(i, 3) * 2 + 1;
		int	g = bit(i, 2) + bit(i, 4) * 2 + 1;
		int	b = bit(i, 0) + bit(i, 5) * 2 + 1;
		return duRGBA(r * 63, g * 63, b * 63, a);
	}
	glm::vec3 RCScheduler::getAgentColor(int idx)
	{
		auto color = duIntToCol(idx, 1.0);
		glm::u8vec4 tmpcolor;
		memcpy(&tmpcolor, &color, 4 * sizeof(uint8_t));

		glm::vec3 rnt;
		rnt.r = (float)tmpcolor.r / 255.0f;
		rnt.g = (float)tmpcolor.g / 255.0f;
		rnt.b = (float)tmpcolor.b / 255.0f;

		return rnt;
	}
	void RCScheduler::setAgent(const glm::vec3& pos)
	{
		if (GLOBAL_RCSCHEDULER->isSetTarget || !GLOBAL_RCSCHEDULER->isSetAgent) return;

		auto entity = GLOBAL_SCENE->createEntity();
		auto&& transform = entity.getComponent<GU::TransformComponent>();
		transform.Translation = GLOBAL_RCSCHEDULER->hitPos;

		GLOBAL_RCSCHEDULER->agentParams = agentParams;

		int idx = GLOBAL_RCSCHEDULER->addAgent(GLOBAL_RCSCHEDULER->hitPos, agentParams);
		GLOBAL_RCSCHEDULER->setMoveTarget(idx, agentTargetPos);
		auto&& agentcomponent = entity.addComponent<::GU::AgentComponent>(idx, agentTargetPos);
		GLOBAL_RCSCHEDULER->calAgentPath(GLOBAL_RCSCHEDULER->hitPos, agentTargetPos);
	}
	void RCScheduler::setMoveTarget(int idx, const glm::vec3& pos)
	{
		const dtQueryFilter* filter = m_crowd->getFilter(0);
		const float* halfExtents = m_crowd->getQueryExtents();
		m_navQuery->findNearestPoly(glm::value_ptr(pos), halfExtents, filter, &m_targetRef, m_targetPos);
		if (idx != -1)
		{
			dtCrowdAgent const * ag = m_crowd->getAgent(idx);
			if (ag && ag->active)
			{
				m_crowd->requestMoveTarget(idx, m_targetRef, m_targetPos);
			}
		}
	}

	void RCScheduler::crowUpdatTick(float delatTime)
	{	
		if (m_crowd == nullptr) return;

		int numActiveAgents = 0;
		numActiveAgents = m_crowd->getActiveAgents(agents, MAX_AGENTS);
		if (numActiveAgents == 0) return;

		m_crowd->update(delatTime, &m_agentDebug);
	}
	void RCScheduler::setCurrentTarget(const glm::vec3& pos)
	{
		if (!GLOBAL_RCSCHEDULER->isSetTarget || GLOBAL_RCSCHEDULER->isSetAgent) return;

		agentTargetPos = pos;
		auto entity = GLOBAL_SCENE->getEntityByUUID(targetModelId);
		targetModelId = entity.getComponent<IDComponent>().ID;
		auto&& transform = entity.getComponent<TransformComponent>();
		transform.Translation = { pos };
	}

	void RCScheduler::calAgentPath(const glm::vec3& p_start, const glm::vec3& p_end)
	{
		float m_spos[3] = { p_start.x, p_start.y, p_start.z };
		float m_epos[3] = { p_end.x, p_end.y, p_end.z };

		float m_polyPickExt[3];

		m_polyPickExt[0] = 2;
		m_polyPickExt[1] = 4;
		m_polyPickExt[2] = 2;
		dtPolyRef m_startRef;
		dtPolyRef m_endRef;
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef m_parent[MAX_POLYS];
		dtQueryFilter m_filter;

		float m_smoothPath[MAX_SMOOTH * 3];
		int m_nsmoothPath = 0;

		int m_npolys;

		if (!m_navMesh)
			return;

		m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);

		m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);


		//m_pathFindStatus = DT_FAILURE;

		//if (m_toolMode == TOOLMODE_PATHFIND_FOLLOW)
		{
			//m_pathIterNum = 0;
			if (m_startRef && m_endRef)
			{
#ifdef DUMP_REQS
				printf("pi  %f %f %f  %f %f %f  0x%x 0x%x\n",
					m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
					m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif

				m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);

				m_nsmoothPath = 0;

				if (m_npolys)
				{
					// Iterate over the path to find smooth path on the detail mesh surface.
					dtPolyRef polys[MAX_POLYS];
					memcpy(polys, m_polys, sizeof(dtPolyRef) * m_npolys);
					int npolys = m_npolys;

					float iterPos[3], targetPos[3];
					m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos, 0);
					m_navQuery->closestPointOnPoly(polys[npolys - 1], m_epos, targetPos, 0);

					static const float STEP_SIZE = 0.5f;
					static const float SLOP = 0.01f;

					m_nsmoothPath = 0;

					dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
					m_nsmoothPath++;

					// Move towards target a small advancement at a time until target reached or
					// when ran out of memory to store the path.
					while (npolys && m_nsmoothPath < MAX_SMOOTH)
					{
						// Find location to steer towards.
						float steerPos[3];
						unsigned char steerPosFlag;
						dtPolyRef steerPosRef;

						if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
							polys, npolys, steerPos, steerPosFlag, steerPosRef))
							break;

						bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
						bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

						// Find movement delta.
						float delta[3], len;
						dtVsub(delta, steerPos, iterPos);
						len = dtMathSqrtf(dtVdot(delta, delta));
						// If the steer target is end of path or off-mesh link, do not move past the location.
						if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
							len = 1;
						else
							len = STEP_SIZE / len;
						float moveTgt[3];
						dtVmad(moveTgt, iterPos, delta, len);

						// Move
						float result[3];
						dtPolyRef visited[16];
						int nvisited = 0;
						m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
							result, visited, &nvisited, 16);

						npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
						npolys = fixupShortcuts(polys, npolys, m_navQuery);

						float h = 0;
						m_navQuery->getPolyHeight(polys[0], result, &h);
						result[1] = h;
						dtVcopy(iterPos, result);

						// Handle end of path and off-mesh links when close enough.
						if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
						{
							// Reached end of path.
							dtVcopy(iterPos, targetPos);
							if (m_nsmoothPath < MAX_SMOOTH)
							{
								dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
								m_nsmoothPath++;
							}
							break;
						}
						else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
						{
							// Reached off-mesh connection.
							float startPos[3], endPos[3];

							// Advance the path up to and over the off-mesh connection.
							dtPolyRef prevRef = 0, polyRef = polys[0];
							int npos = 0;
							while (npos < npolys && polyRef != steerPosRef)
							{
								prevRef = polyRef;
								polyRef = polys[npos];
								npos++;
							}
							for (int i = npos; i < npolys; ++i)
								polys[i - npos] = polys[i];
							npolys -= npos;

							// Handle the connection.
							dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
							if (dtStatusSucceed(status))
							{
								if (m_nsmoothPath < MAX_SMOOTH)
								{
									dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
									m_nsmoothPath++;
									// Hack to make the dotted path not visible during off-mesh connection.
									if (m_nsmoothPath & 1)
									{
										dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
										m_nsmoothPath++;
									}
								}
								// Move position at the other side of the off-mesh link.
								dtVcopy(iterPos, endPos);
								float eh = 0.0f;
								m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
								iterPos[1] = eh;
							}
						}

						// Store results.
						if (m_nsmoothPath < MAX_SMOOTH)
						{
							dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
							m_nsmoothPath++;
						}
					}
				}

			}
			else
			{
				m_npolys = 0;
				m_nsmoothPath = 0;
			}
		}
		int m_straightPathOptions = DT_STRAIGHTPATH_ALL_CROSSINGS;
		float m_straightPath[MAX_POLYS * 3];
		unsigned char m_straightPathFlags[MAX_POLYS];
		dtPolyRef m_straightPathPolys[MAX_POLYS];
		int m_nstraightPath;
		if (m_startRef && m_endRef)
		{
#ifdef DUMP_REQS
			printf("ps  %f %f %f  %f %f %f  0x%x 0x%x\n",
				m_spos[0], m_spos[1], m_spos[2], m_epos[0], m_epos[1], m_epos[2],
				m_filter.getIncludeFlags(), m_filter.getExcludeFlags());
#endif
			m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
			m_nstraightPath = 0;
			if (m_npolys)
			{
				// In case of partial path, make sure the end point is clamped to the last polygon.
				float epos[3];
				dtVcopy(epos, m_epos);
				if (m_polys[m_npolys - 1] != m_endRef)
					m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], m_epos, epos, 0);

				m_navQuery->findStraightPath(m_spos, epos, m_polys, m_npolys,
					m_straightPath, m_straightPathFlags,
					m_straightPathPolys, &m_nstraightPath, MAX_POLYS, m_straightPathOptions);
			}
		}
		else
		{
			m_npolys = 0;
			m_nstraightPath = 0;
		}

		/*numbAgentPaths.push_back(m_nsmoothPath);

		std::array<float, MAX_SMOOTH * 3> inputpath;
		for (size_t i = 0; i < m_nsmoothPath * 3; i++)
		{
			inputpath[i] = m_smoothPath[i];
		}
		agentPaths.push_back(inputpath);*/
		std::shared_ptr<RCAgentPath> inputPath = std::make_shared<RCAgentPath>(m_smoothPath, m_nsmoothPath);
		rcAgentPath.push_back(inputPath);

		std::shared_ptr<RCStraightPath> inputStraightPath = std::make_shared<RCStraightPath>(m_straightPath, m_npolys);
		rcStraightPath.push_back(inputStraightPath);
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