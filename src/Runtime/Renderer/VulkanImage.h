#pragma once
#include <stb/stb_image.h>
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <stdexcept>
#include <Global/CoreContext.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
    void createTextureImage(const VulkanContext& vulkanContext, const char* picturePath, VulkanImage& image);
    void createImage(const VulkanContext& vulkanContext, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(const VulkanContext& vulkanContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(const VulkanContext& vulkanContext, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImageView(const VulkanContext& vulkanContext, VkImage image, VkFormat format, VkImageView& imageView);
    void createTextureImageView(const VulkanContext& vulkanContext, VulkanImage& vkImage);
    void createTextureSampler(const VulkanContext& vulkanContext, VulkanImage& vkImage);
    void findSupportedFormat(const VulkanContext& vulkanContext, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkFormat& format);
    void findDepthFormat(const VulkanContext& vulkanContext, VkFormat& format);
    bool hasStencilComponent(VkFormat format);
}