#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <array>
#include <vector>
#include <vulkan/vulkan.h>
#include <stb/stb_image.h>
#include <unordered_map>
#include <filesystem>
namespace GU
{
	// Vertex
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription();

		static ::std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};
	
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	// Image
	struct VulkanImage
	{
		VkImage			image;
		VkDeviceMemory	imageMemory;
		VkDeviceSize	size;
		VkImageView		view;
		VkSampler		sampler;
	};

	void loadModel(::std::filesystem::path filePath, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
}
namespace std {
	template<> struct hash<GU::Vertex> {
		size_t operator()(GU::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}