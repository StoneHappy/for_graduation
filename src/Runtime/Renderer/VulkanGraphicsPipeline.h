#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Core/Type.h>
namespace VulkanLib
{
	VkPipeline createGraphicsPipeline(VkDevice device,\
		VkRenderPass renderPass, \
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages,\
		VkPipelineLayout pipelineLayout, \
		VkExtent2D extent,\
		GU::LogFunc logFunc = GU::STDDebugLogFunction());
}