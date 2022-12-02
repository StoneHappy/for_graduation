#pragma once
#include <Renderer/VulkanContext.h>
#include <Renderer/VulkanImage.h>
#include <vulkan/vulkan.h>
namespace GU
{
    void createBackgroundPipeline(VulkanContext& vulkanContext, VkPipeline& backgroudPipeline);
    void createBoundingBoxPipeline(VulkanContext& vulkanContext, VkPipeline& wireframePipeline);
    void createGraphicsPipeline(VulkanContext& vulkanContext, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages, const VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline);
    void createPipelineLayout(VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);
    
}