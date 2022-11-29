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
		uint32_t id;
	};
	class MeshNode
	{
	public:
		std::vector<Mesh> m_meshs;
	};

	bool readMesh(const VulkanContext& vulkanContext, MeshNode& meshnode, const char* filePath);
}