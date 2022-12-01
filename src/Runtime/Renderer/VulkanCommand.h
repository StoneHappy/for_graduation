#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
    VkCommandBuffer beginSingleTimeCommands(VulkanContext& vulkanContext);
    void endSingleTimeCommands(VulkanContext& vulkanContext, VkCommandBuffer& commandBuffer);
}