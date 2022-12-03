#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <Global/CoreContext.h>
#include <vector>
namespace GU
{
	template <class T>
	class VulkanUniformBuffer {
	public:
		~VulkanUniformBuffer() = default;

		VulkanUniformBuffer()
		{
			createUniformBuffers(*GLOBAL_VULKAN_CONTEXT, uniformBuffers, uniformBuffersMemory, uniformBuffersMapped, sizeof(T));
		}

		void update(const T& p_data, int currentImage)
		{
			memcpy(uniformBuffersMapped[currentImage], &p_data, sizeof(p_data));
		}
		std::vector<VkBuffer>					uniformBuffers;
		std::vector<VkDeviceMemory>				uniformBuffersMemory;
		std::vector<void*>						uniformBuffersMapped;
	};
}