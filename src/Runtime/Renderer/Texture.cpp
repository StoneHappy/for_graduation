#include "Texture.h"
#include <Renderer/VulkanImage.h>
#include <Global/CoreContext.h>
namespace GU
{
	Texture::Texture()
	{
		m_image = std::make_shared<VulkanImage>();
	}
	Texture::~Texture()
	{

	}

	std::shared_ptr<Texture> Texture::read(const std::filesystem::path& filepath)
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		createTextureImage(GLOBAL_VULKAN_CONTEXT, filepath.string().c_str(), *texture->m_image);
		return texture;
	}
}


