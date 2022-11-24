#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
    VkCommandBuffer beginSingleTimeCommands(const VulkanContext& vulkanContext);
    void endSingleTimeCommands(const VulkanContext& vulkanContext, VkCommandBuffer& commandBuffer);
}