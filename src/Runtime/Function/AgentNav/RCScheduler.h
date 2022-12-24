#pragma once
#include <memory>
#include "rcMeshLoaderObj.h"
#include <Function/AgentNav/RCParams.h>
#include <Recast.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <DetourCrowd.h>
#include <vector>
class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
struct rcChunkyTriMesh;

namespace GU
{
	class Mesh;
	class BuildContext;
	class RCMesh;
	class RCContour;
	class RCHeightfieldSolid;
	class RCAgentPath;
	class RCStraightPath;
	class RCScheduler
	{
	public:
		
		RCScheduler();
		~RCScheduler() = default;

		bool handelBuild(const RCParams& rcparams, Mesh* mesh);
		void handelRender(VkCommandBuffer cmdBuf, int currentImage);
		bool raycastMesh(float* src, float* dst, float& tmin);

		/* crowd */
		// add agent by params
		glm::vec3 getAgentPosWithId(int idx);
		void getAgentRotationWithId(int idx, glm::vec3& rotation);
		float getVelLength(int idx);
		glm::vec3 getAgentColor(int idx);
		int addAgent(const glm::vec3& pos, const dtCrowdAgentParams& ap);
		void setAgent(const glm::vec3& pos);

		void setMoveTarget(int idx, const glm::vec3& pos);
		void setCurrentTarget(const glm::vec3& pos);
		void crowUpdatTick(float delatTime);
		dtPolyRef m_targetRef;
		float m_targetPos[3];
		dtCrowdAgent* agents[MAX_AGENTS];
		dtCrowdAgentDebugInfo m_agentDebug;
		dtObstacleAvoidanceDebugData* m_vod;
		class dtCrowd* m_crowd;
		dtCrowdAgentParams agentParams;
		glm::vec3 agentTargetPos;
		// path
		std::vector<int> numbAgentPaths;
		std::vector<std::array<float, MAX_SMOOTH*3> > agentPaths;

		std::vector<std::shared_ptr<RCAgentPath>> rcAgentPath;
		std::vector<std::shared_ptr<RCStraightPath>> rcStraightPath;

		void calAgentPath(const glm::vec3& start, const glm::vec3& end);

		bool isSetTarget = false;
		bool isSetAgent = false;
		/* crowd */
		bool isRenderHeightField = true;
		bool isRenderContour = true;
		bool isRenderDetailMesh = true;

		RCMesh* m_polymesh = nullptr;
		RCContour* m_polyContourMesh = nullptr;
		RCHeightfieldSolid* m_heightFieldSolid = nullptr;
		glm::vec3 hitPos;
		RCParams m_rcparams;

		uint64_t targetModelId;
	private:
		void createRCMesh(Mesh* mesh, rcMeshLoaderObj& rcMesh);
	private:
		unsigned char* m_triareas;
		rcHeightfield* m_solid;
		rcCompactHeightfield* m_chf;
		rcContourSet* m_cset;
		rcPolyMesh* m_pmesh;
		rcConfig m_cfg;
		rcPolyMeshDetail* m_dmesh;
		rcMeshLoaderObj m_mesh;
		rcChunkyTriMesh* m_chunkyMesh;
		BuildContext* m_ctx;
		float m_meshBMin[3], m_meshBMax[3];


		class dtNavMesh* m_navMesh;
		class dtNavMeshQuery* m_navQuery;


		enum PartitionType
		{
			SAMPLE_PARTITION_WATERSHED,
			SAMPLE_PARTITION_MONOTONE,
			SAMPLE_PARTITION_LAYERS
		};

		enum PolyFlags
		{
			SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
			SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
			SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
			SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
			SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
			SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
		};

		/// These are just sample areas to use consistent values across the samples.
		/// The use should specify these base on his needs.
		enum PolyAreas
		{
			SAMPLE_POLYAREA_GROUND,
			SAMPLE_POLYAREA_WATER,
			SAMPLE_POLYAREA_ROAD,
			SAMPLE_POLYAREA_DOOR,
			SAMPLE_POLYAREA_GRASS,
			SAMPLE_POLYAREA_JUMP
		};
	};


	class BuildContext : public rcContext
	{
	public:
		BuildContext() = default;
		~BuildContext() = default;
	};
}