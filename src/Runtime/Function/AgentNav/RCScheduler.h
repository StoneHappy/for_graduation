#pragma once
#include <memory>
#include "rcMeshLoaderObj.h"
#include <Function/AgentNav/RCParams.h>
#include <Recast.h>
#include <vulkan/vulkan.h>
class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;

namespace GU
{
	class Mesh;
	class BuildContext;
	class RCMesh;
	class RCContour;
	class RCScheduler
	{
	public:
		RCScheduler();
		~RCScheduler() = default;

		bool handelBuild(const RCParams& rcparams, Mesh* mesh);
		void handelRender(VkCommandBuffer cmdBuf, int currentImage);

		RCMesh* m_polymesh = nullptr;
		RCContour* m_polyContourMesh = nullptr;
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
		rcMeshLoaderObj rcmesh;
		BuildContext* m_ctx;
		float m_meshBMin[3], m_meshBMax[3];


		class dtNavMesh* m_navMesh;
		class dtNavMeshQuery* m_navQuery;
		class dtCrowd* m_crowd;


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