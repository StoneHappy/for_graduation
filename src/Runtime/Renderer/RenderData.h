#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
namespace GU
{
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}