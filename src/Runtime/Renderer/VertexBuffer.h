#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <Global/CoreContext.h>
#include <Renderer/VulkanUtil.h>
#include <vector>
#include <stdexcept>
namespace GU
{
	std::pair<VkCommandBuffer, VkDeviceMemory> createVertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, const std::vector<VulkanObject> vObjects)
	{
		GU::g_CoreContext.g_Log("正在创建VertexBuffer.");
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = vObjects[0].getDataSize() * vObjects.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}


		vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

		void* data;
		vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vObjects.data(), (size_t)bufferInfo.size);
		vkUnmapMemory(device, vertexBufferMemory);
	}
}