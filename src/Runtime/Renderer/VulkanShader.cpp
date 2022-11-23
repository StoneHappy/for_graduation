#include "VulkanShader.h"
#include <stdexcept>
#include <QDebug>
namespace GU
{
	VkShaderModule createShader(VkDevice device, const uint32_t* code, uint32_t size)
	{
        VkShaderModule rnt;
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = code;

        if (vkCreateShaderModule(device, &createInfo, nullptr, &rnt) != VK_SUCCESS) {
            qErrnoWarning("failed to create shader module!");
            throw std::runtime_error("failed to create shader module!");
        }

        return rnt;
	}
    void createShaderStageInfo(const VkShaderModule& vertexShader, const VkShaderModule& fragShader, std::vector<VkPipelineShaderStageCreateInfo>& pipelineShaderStageCreateInfo)
    {
		pipelineShaderStageCreateInfo = {
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_VERTEX_BIT,
			vertexShader,
			"main",
			nullptr
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			fragShader,
			"main",
			nullptr
		}
		};
    }
}