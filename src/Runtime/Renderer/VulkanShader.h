#pragma once
#include <vulkan/vulkan.h>
namespace GU
{
	VkShaderModule createShader(VkDevice device, const uint32_t* code, uint32_t size);
}