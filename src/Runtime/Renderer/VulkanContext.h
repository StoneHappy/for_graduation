#pragma once
#include <vulkan/vulkan.h>
namespace GU
{
	class VulkanContext
	{
	public:
		VkPhysicalDevice	physicalDevice;
		VkDevice			logicalDevice;
		VkCommandPool		commandPool;
		VkQueue				graphicsQueue;
		VkExtent2D			swapChainExtent;
		VkPipeline			graphicsPipeline;
		VkBuffer			vertexBuffer;
		VkDeviceMemory		vertexMemory;
		VkBuffer			indexBuffer;
		VkDeviceMemory		indexMemory;
	private:

	};
}