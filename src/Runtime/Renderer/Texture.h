#pragma once
#include <filesystem>
#include <memory>
namespace GU
{
	class VulkanImage;
	class Texture
	{
	public:
		Texture();
		~Texture();
		static std::shared_ptr<Texture> read(const std::filesystem::path& filepath);
		std::shared_ptr<VulkanImage> image;
	};
}