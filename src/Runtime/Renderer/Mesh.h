#pragma once
#include <Renderer/RenderData.h>
#include <vulkan/vulkan.h>
#include <Renderer/VulkanImage.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
	class Mesh
	{
	public:
		Mesh() = default;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		VulkanImage m_Texture;
		VkPipeline m_Pipeline;
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
		VkBuffer								indexBuffer;
		VkDeviceMemory							indexMemory;

		bool isRoot = false;
		bool isLeaf = false;
		uint32_t id;
		uint32_t parent;
		std::vector<uint32_t> child;
	};
	class MeshNode
	{
	public:
		std::vector<Mesh> m_meshs;
	};

	bool readMesh(const VulkanContext& vulkanContext, MeshNode& meshnode, const char* filePath);
}