#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Renderer/RenderData.h>
#include <Renderer/VulkanUniformBuffer.hpp>
namespace GU
{
	class Texture;
	template <class T>
	class VulkanUniformBuffer;
	class ModelUBO;

	class Material
	{
	public:
		Material();
		~Material();
		std::shared_ptr<Texture> diffuseTexture;
		std::vector<VkDescriptorSet> descriptorSets;
		std::shared_ptr<VulkanUniformBuffer<ModelUBO> > modelUBO;
	};
}