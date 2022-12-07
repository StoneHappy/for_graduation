#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <Renderer/VulkanContext.h>
#include <Global/CoreContext.h>
#include <vector>
#include <stdexcept>
#include <Renderer/EditCamera.h>
namespace GU
{
	void createVertexBuffer(VulkanContext& vkContext, const std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createVertexBuffer(VulkanContext& vkContext, const std::vector<SkeletalVertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createIndexBuffer(VulkanContext& vkContext, const std::vector<uint32_t>& indices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createUniformBuffers(VulkanContext& vkContext, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped, VkDeviceSize bufferSize);
	void destoryUniformBuffer(VulkanContext& vkContext, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped);
	void createBuffer(VulkanContext& vkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VulkanContext& vkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void updateUniformBuffer(VulkanContext& vkContext, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped);
	void updateUniformBuffer(VulkanContext& vkContext, EditCamera& camera, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped);
	void updateMeshUniformBuffer(VulkanContext& vkContext, const glm::mat4& model, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped);
}