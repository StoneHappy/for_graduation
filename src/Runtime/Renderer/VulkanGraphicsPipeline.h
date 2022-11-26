#pragma once
#include <Renderer/VulkanContext.h>
#include <Renderer/VulkanImage.h>
#include <vulkan/vulkan.h>
namespace GU
{
    void createBackgroundPipeline(const VulkanContext& vulkanContext, VkPipeline& backgroudPipeline);
    void createBoundingBoxPipeline(const VulkanContext& vulkanContext, VkPipeline& wireframePipeline);
    void createGraphicsPipeline(const VulkanContext& vulkanContext, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages, const VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline);
    void createPipelineLayout(const VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);
    void createDescriptorSetLayout(const VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout);
    void createDescriptorPool(const VulkanContext& vkContext, VkDescriptorPool& descriptorPool);
    void createDescriptorSets(const VulkanContext& vkContext, VulkanImage& vulkanImage, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets);
}