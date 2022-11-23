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
}