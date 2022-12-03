#pragma once
#include <memory>
#include <Renderer/Mesh.h>
#include <Recast.h>
#include <Function/AgentNav/rcMeshLoaderObj.h>
namespace GU
{
	class NavMesh
	{
	public:
		NavMesh();
		~NavMesh();
		static std::shared_ptr<NavMesh> createFromGUMesh(const Mesh& mesh);

		rcMeshLoaderObj rcMesh;
	};
}