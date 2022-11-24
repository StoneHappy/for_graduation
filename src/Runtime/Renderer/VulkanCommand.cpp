#include "VulkanCommand.h"

namespace GU
{
    VkCommandBuffer beginSingleTimeCommands(const VulkanContext& vulkanContext) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkanContext.commandPool;
        allocInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(vulkanContext.logicalDevice, &allocInfo, &commandBuffer);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    void endSingleTimeCommands(const VulkanContext& vulkanContext, VkCommandBuffer& commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(vulkanContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(vulkanContext.graphicsQueue);

        vkFreeCommandBuffers(vulkanContext.logicalDevice, vulkanContext.commandPool, 1, &commandBuffer);
    }
}