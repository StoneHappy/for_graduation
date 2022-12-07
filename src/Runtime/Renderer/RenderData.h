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

#define MAX_BONES 70
namespace GU
{
	// Vertex
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription();

		static ::std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
		}
	};

	// Vertex
	struct SkeletalVertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 texCoord;

		glm::ivec4	boneIDs; // Stream of vertex bone IDs
		glm::vec4	weights; // Stream of vertex weights

		static VkVertexInputBindingDescription getBindingDescription();

		static ::std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
		}
	};
	
	struct CameraUBO {
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct ModelUBO {
		glm::mat4 model;
	};

	struct SkeletalModelUBO {
		SkeletalModelUBO();
		glm::mat4 model;
		glm::mat4 bones[MAX_BONES]; // Bone transformations 
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

}
namespace std {
	template<> struct hash<GU::Vertex> {
		size_t operator()(GU::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}