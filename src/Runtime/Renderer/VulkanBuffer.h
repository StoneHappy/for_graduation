#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <Global/CoreContext.h>
#include <Renderer/VulkanUtil.h>
#include <vector>
#include <stdexcept>
namespace GU
{
	std::pair<VkBuffer, VkDeviceMemory> createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<Vertex> vertices);
}