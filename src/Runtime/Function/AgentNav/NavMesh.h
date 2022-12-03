#pragma once
#include <memory>
#include <Renderer/Mesh.h>
#include <Recast.h>
namespace GU
{
	class NavMesh
	{
	public:
		NavMesh();
		~NavMesh();
		std::shared_ptr<NavMesh> createFromGUMesh(const Mesh& mesh);


	};
}