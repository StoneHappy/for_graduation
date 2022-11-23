#include "VulkanBuffer.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
namespace GU
{
	void createVertexBuffer(const VulkanContext& vkContext, const std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		GU::g_CoreContext.g_Log("正在创建VertexBuffer...");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// map memory to cpu host
		void* data;
		vkMapMemory(vkContext.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkContext.logicalDevice, stagingBufferMemory);

		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		copyBuffer(vkContext, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(vkContext.logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkContext.logicalDevice, stagingBufferMemory, nullptr);
	}

	void createIndexBuffer(const VulkanContext& vkContext, const std::vector<uint16_t>& indices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		GU::g_CoreContext.g_Log("正在创建IndexBuffer...");
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		// map memory to cpu host
		void* data;
		vkMapMemory(vkContext.logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(vkContext.logicalDevice, stagingBufferMemory);

		createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);

		copyBuffer(vkContext, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(vkContext.logicalDevice, stagingBuffer, nullptr);
		vkFreeMemory(vkContext.logicalDevice, stagingBufferMemory, nullptr);
	}

	void createUniformBuffers(const VulkanContext& vkContext, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped)
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		uniformBuffers.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < VulkanContext::MAX_FRAMES_IN_FLIGHT; i++) {
			createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(vkContext.logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void createBuffer(const VulkanContext& vkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vkContext.logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vkContext.logicalDevice, buffer, &memRequirements);

		VkMemoryAllocateInfo  allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(vkContext.physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vkContext.logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(vkContext.logicalDevice, buffer, bufferMemory, 0);
	}

	void copyBuffer(const VulkanContext& vkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vkContext.commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vkContext.logicalDevice, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyBuffer = {};
		copyBuffer.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyBuffer);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vkContext.graphicsQueue);

		vkFreeCommandBuffers(vkContext.logicalDevice, vkContext.commandPool, 1, &commandBuffer);
	}

	uint32_t findMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

		for (size_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) return i;
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void updateUniformBuffer(const VulkanContext& vkContext, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), vkContext.swapChainExtent.width / (float)vkContext.swapChainExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
	
}