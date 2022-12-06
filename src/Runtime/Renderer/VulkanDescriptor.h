#pragma once
#include <Renderer/VulkanContext.h>
#include <Renderer/VulkanImage.h>
namespace GU
{
    void createDescriptorSetLayout(VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout);
    void createDescriptorPool(VulkanContext& vkContext, VkDescriptorPool& descriptorPool);
    void createDescriptorSets(VulkanContext& vkContext, VulkanImage& vulkanImage, std::vector<VkBuffer>& modelUBOs, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets);
    void destoryDescriptorPool(VulkanContext& vkContext, VkDescriptorPool& descriptorPool);
    void destoryDescriptorSetLayout(VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout);
}