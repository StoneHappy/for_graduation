#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace GU
{
	class VulkanContext
	{
	public:
		VkPhysicalDevice						physicalDevice;
		VkDevice								logicalDevice;
		VkCommandPool							commandPool;
		VkQueue									graphicsQueue;
		VkExtent2D								swapChainExtent;
		VkRenderPass							renderPass;
		VkPipeline								graphicsPipeline;
		VkPipelineLayout						pipelineLayout;
		std::vector<VkDescriptorSetLayout>		descriptorSetLayouts;
		VkBuffer								vertexBuffer;
		VkDeviceMemory							vertexMemory;
		VkBuffer								indexBuffer;
		VkDeviceMemory							indexMemory;
		std::vector<VkBuffer>					uniformBuffers;
		std::vector<VkDeviceMemory>				uniformBuffersMemory;
		std::vector<void*>						uniformBuffersMapped;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStage;
		static int MAX_FRAMES_IN_FLIGHT;
	private:

	};
}