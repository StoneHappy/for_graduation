#pragma once
#include <Renderer/RenderData.h>
#include <vulkan/vulkan.h>
#include <Renderer/VulkanImage.h>
#include <Renderer/VulkanContext.h>
#include <memory>
namespace GU
{
	class Mesh
	{
	public:
		Mesh() = default;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		float bmin[3];
		float bmax[3];
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
		VkBuffer								indexBuffer;
		VkDeviceMemory							indexMemory;
		uint32_t id;
		uint64_t textureID;
	};
	class MeshNode
	{
	public:
		MeshNode();
		~MeshNode();

	public:
		struct MeshTree
		{
			uint32_t parentID = 0;
			uint32_t id = 0;
			std::vector<uint32_t> childrenIDs;
			std::vector<MeshTree> childrens;
			bool isRoot = false;
			bool isLeft = false;
		};
		std::vector<Mesh> meshs;
		MeshTree root;

		static bool read(VulkanContext& vulkanContext, std::shared_ptr<MeshNode> meshnode, const std::filesystem::path& filepath);
	};

}