#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <Recast.h>
#include <vulkan/vulkan.h>
#include <array>
namespace GU
{
	struct RCVertex
	{
		glm::vec3 pos;
		glm::vec4 color;

		static VkVertexInputBindingDescription getBindingDescription();

		static ::std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	struct RCMesh
	{
		RCMesh(const rcPolyMesh& mesh);
		RCMesh(const rcPolyMeshDetail& mesh);
		~RCMesh() = default;

		std::vector<RCVertex> m_verts;
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
	};

	struct RCContour
	{
		RCContour(const rcPolyMeshDetail& mesh);
		~RCContour() = default;

		std::vector<RCVertex> internalVerts;
		std::vector<RCVertex> externalVerts;
		VkBuffer								internalVertexBuffer;
		VkBuffer								externalVertexBuffer;
		VkDeviceMemory							internalVertexMemory;
		VkDeviceMemory							externalVertexMemory;
	};
}