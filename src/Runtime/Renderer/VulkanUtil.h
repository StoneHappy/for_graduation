#pragma once
#include <vulkan/vulkan.h>
#include <Core/Type.h>
namespace VulkanLib
{
	uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
}