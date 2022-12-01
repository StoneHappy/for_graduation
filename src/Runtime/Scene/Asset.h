#pragma once
#include <unordered_map>
#include <Core/UUID.h>
#include <memory>
#include <filesystem>
namespace GU
{
	class MeshNode;
	class Asset
	{
	public:
		Asset() = default;
		~Asset() = default;

		UUID insertMesh(const std::filesystem::path& filepath);
		std::shared_ptr<MeshNode> getMeshWithUUID(UUID uuid);
	private:
		std::unordered_map<UUID, std::shared_ptr<MeshNode>> m_meshMap;
		std::unordered_map<std::filesystem::path, UUID> m_loadedModelMap;
	};
}