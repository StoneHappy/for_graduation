#pragma once
#include <unordered_map>
#include <Core/UUID.h>
#include <memory>
#include <filesystem>
namespace GU
{
	class MeshNode;
	class Texture;
	class Asset
	{
	public:
		friend class Project;

		enum class AssetType
		{
			Mesh = 0,
			Texture
		};

		Asset() = default;
		~Asset() = default;

		UUID insertMesh(const std::filesystem::path& filepath);
		UUID insertMeshWithUUID(const std::filesystem::path& filepath, UUID uuid);

		std::filesystem::path getMeshPathWithUUID(UUID uuid);

		UUID insertTexture(const std::filesystem::path& filepath);
		UUID insertTextureWithUUID(const std::filesystem::path& filepath, UUID uuid);

		std::shared_ptr<MeshNode> getMeshWithUUID(UUID uuid);
	private:
		std::unordered_map<UUID, std::shared_ptr<MeshNode>> m_meshMap;
		std::unordered_map<std::filesystem::path, UUID> m_loadedModelMap;

		std::unordered_map<UUID, std::shared_ptr<Texture>> m_textureMap;
		std::unordered_map<std::filesystem::path, UUID> m_loadedTextureMap;
	};
}