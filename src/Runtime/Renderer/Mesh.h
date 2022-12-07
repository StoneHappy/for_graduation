#pragma once
#include <Renderer/RenderData.h>
#include <vulkan/vulkan.h>
#include <Renderer/VulkanImage.h>
#include <Renderer/VulkanContext.h>
#include <memory>
namespace GU
{
	struct MeshTree
	{
		uint32_t parentID = 0;
		uint32_t id = 0;
		std::vector<uint32_t> childrenIDs;
		std::vector<MeshTree> childrens;
		bool isRoot = false;
		bool isLeft = false;
	};
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
		std::vector<Mesh> meshs;
		MeshTree root;

		static bool read(VulkanContext& vulkanContext, std::shared_ptr<MeshNode> meshnode, const std::filesystem::path& filepath);
	};

	// Stores bone information
	struct BoneInfo
	{
		glm::mat4 FinalTransformation; // Final transformation to apply to vertices 
		glm::mat4 BoneOffset; // Initial offset from local to bone space. 
	};

	class SkeletalMesh
	{
	public:
		SkeletalMesh() = default;
		std::vector<SkeletalVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<BoneInfo> boneinfos;
		float bmin[3];
		float bmax[3];
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
		VkBuffer								indexBuffer;
		VkDeviceMemory							indexMemory;
		uint32_t id;
		uint64_t textureID;
	};

	

	class SkeletalMeshNode
	{
	public:
		SkeletalMeshNode();
		~SkeletalMeshNode();

	public:
		std::vector<SkeletalMesh> meshs;
		MeshTree root;

		static bool read(VulkanContext& vulkanContext, std::shared_ptr<SkeletalMeshNode> meshnode, const std::filesystem::path& filepath);
	};

}