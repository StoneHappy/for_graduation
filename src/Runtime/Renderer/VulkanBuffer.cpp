#include "VulkanBuffer.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <Renderer/VulkanCommand.h>
#include <Global/CoreContext.h>
namespace GU
{
	void createVertexBuffer(VulkanContext& vkContext, const std::vector<Vertex>& vertices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
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

	void createIndexBuffer(VulkanContext& vkContext, const std::vector<uint32_t>& indices, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
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

	void createUniformBuffers(VulkanContext& vkContext, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped, VkDeviceSize bufferSize)
	{
		uniformBuffers.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMemory.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);
		uniformBuffersMapped.resize(VulkanContext::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < VulkanContext::MAX_FRAMES_IN_FLIGHT; i++) {
			createBuffer(vkContext, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

			vkMapMemory(vkContext.logicalDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
		}
	}

	void destoryUniformBuffer(VulkanContext& vkContext, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped)
	{
		for (size_t i = 0; i < VulkanContext::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(vkContext.logicalDevice, uniformBuffers[i], nullptr);
			vkUnmapMemory(vkContext.logicalDevice, uniformBuffersMemory[i]);
			vkFreeMemory(vkContext.logicalDevice, uniformBuffersMemory[i], nullptr);
		}
	}

	void createBuffer(VulkanContext& vkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
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

	void copyBuffer(VulkanContext& vkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(vkContext);
		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		endSingleTimeCommands(vkContext, commandBuffer);
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
	void updateUniformBuffer(VulkanContext& vkContext, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped) {
		
		CameraUBO ubo{};
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), vkContext.swapChainExtent.width / (float)vkContext.swapChainExtent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void updateUniformBuffer(VulkanContext& vkContext, EditCamera& camera, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped)
	{
		float rotateGreed = 1.0f;
		CameraUBO ubo{};
		ubo.view = camera.getViewMatrix();
		ubo.proj = camera.getProjectionMatrix();
		ubo.proj[1][1] *= -1;
		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
	void updateMeshUniformBuffer(VulkanContext& vkContext, const glm::mat4& model, uint32_t currentImage, std::vector<void*>& uniformBuffersMapped)
	{
		float rotateGreed = 1.0f;
		ModelUBO ubo{};
		ubo.model = model;
		memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}
}