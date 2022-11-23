#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
namespace GU
{
	class VulkanObject
	{
	public:
		VulkanObject() = default;
		virtual ~VulkanObject() = default;
		virtual VkVertexInputBindingDescription getBindingDescription() const = 0;
		virtual std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const = 0;
		virtual uint32_t getDataSize() const = 0;
	};

	class Vertex : public VulkanObject
	{
	public:
		Vertex() = default;
		~Vertex() = default;
		glm::vec2 pos;
		glm::vec3 color;

		VkVertexInputBindingDescription getBindingDescription() const override;
		std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() const override;
		virtual uint32_t getDataSize() const override;
	};
}