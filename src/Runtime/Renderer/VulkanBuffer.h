#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <Renderer/VulkanContext.h>
#include <Global/CoreContext.h>
#include <vector>
#include <stdexcept>
namespace GU
{
	void createVertexBuffer(const VulkanContext& vkContext, const std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createIndexBuffer(const VulkanContext& vkContext, const std::vector<uint16_t>& indices, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createBuffer(const VulkanContext& vkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(const VulkanContext& vkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
}