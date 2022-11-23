#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
namespace VulkanLib
{
	class VulkanObject
	{
	public:
		virtual VkVertexInputBindingDescription getBindingDescription() = 0;
		virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() = 0;
	};

	class Vertex : public VulkanObject
	{
	public:
		glm::vec2 pos;
		glm::vec3 color;

		VkVertexInputBindingDescription getBindingDescription() override;
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override;
	};
}