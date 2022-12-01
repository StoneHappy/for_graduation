#pragma once
#include <stb/stb_image.h>
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <stdexcept>
#include <Global/CoreContext.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
    void createTextureImage(VulkanContext& vulkanContext, const char* picturePath, VulkanImage& image);
    void createImage(VulkanContext& vulkanContext, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VulkanContext& vulkanContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VulkanContext& vulkanContext, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImageView(VulkanContext& vulkanContext, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView& imageView);
    void createTextureImageView(VulkanContext& vulkanContext, VulkanImage& vkImage);
    void createTextureSampler(VulkanContext& vulkanContext, VulkanImage& vkImage);
    void createDepthResources(VulkanContext& vulkanContext, VulkanImage& vkImage);
    void findSupportedFormat(VulkanContext& vulkanContext, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat& format);
    void findDepthFormat(VulkanContext& vulkanContext, VkFormat& format);
    bool hasStencilComponent(VkFormat format);
}