#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace GU
{
	void createRCDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout);
	void createRCDescriptorSets(VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool, std::vector<VkDescriptorSet>& descriptorSets);
	void createRCPipelineLayout(VkDescriptorSetLayout& descriptorSetLayout, VkPipelineLayout& pipelineLayout);
	void createRCGraphicsPipeline(const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages, const VkPipelineLayout& pipelineLayout, VkPipeline& graphicsPipeline);

	void createRCGraphicsPieline();
}