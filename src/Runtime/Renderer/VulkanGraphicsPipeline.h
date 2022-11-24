#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Core/Type.h>
#include <Renderer/VulkanContext.h>
namespace GU
{
	void createGraphicsPipeline(const VulkanContext& vulkanContext, const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages, const VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline);

	void createPipelineLayout(const VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);

	void createDescriptorSetLayout(const VulkanContext& vulkanContext, VkDescriptorSetLayout& descriptorSetLayout);
}