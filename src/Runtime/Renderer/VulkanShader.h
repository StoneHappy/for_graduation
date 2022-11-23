#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace GU
{
	VkShaderModule createShader(VkDevice device, const uint32_t* code, uint32_t size);

	void createShaderStageInfo(const VkShaderModule& vertexShader,const VkShaderModule& fragShader, std::vector<VkPipelineShaderStageCreateInfo>& pipelineShaderStageCreateInfo);
}