#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <Renderer/RenderData.h>
namespace GU
{
	class Texture;
	class Material
	{
	public:

	private:
		std::shared_ptr<Texture> m_diffuseTexture;
		std::vector<VkDescriptorSet> m_descriptorSets;
	};
}