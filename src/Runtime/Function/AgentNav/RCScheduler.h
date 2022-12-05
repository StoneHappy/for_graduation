#pragma once
#include <Recast.h>
#include <memory>
#include "rcMeshLoaderObj.h"
namespace GU
{
	class Mesh;
	class BuildContext;

	class RCScheduler
	{
	public:
		RCScheduler();
		~RCScheduler() = default;

		void handelConfig(rcConfig rcconfig,Mesh* mesh);

		bool handelBuild();

	private:
		void createRCMesh(Mesh* mesh, rcMeshLoaderObj& rcMesh);

	private:
		rcConfig rc_cfg;

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
	};


	class BuildContext : public rcContext
	{
	public:
		BuildContext() = default;
		~BuildContext() = default;
	};
}