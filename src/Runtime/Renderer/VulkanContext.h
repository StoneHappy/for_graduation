#pragma once
#include <vulkan/vulkan.h>
namespace GU
{
	class VulkanContext
	{
	public:
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		VkCommandPool commandPool;
		VkQueue graphicsQueue;

		VkExtent2D swapChainExtent;
	private:

	};
}