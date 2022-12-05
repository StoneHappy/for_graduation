#include "RCScheduler.h"
#include <Renderer/Mesh.h>
namespace GU
{
	void RCScheduler::handelConfig(rcConfig rcconfig, Mesh* mesh)
	{
		rc_cfg = rcconfig;
		
		createRCMesh(mesh, rcmesh);
	}
	void RCScheduler::handelBuild()
	{

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