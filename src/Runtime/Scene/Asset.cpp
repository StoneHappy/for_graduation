#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
#include <MainWindow.h>
#include <Renderer/Texture.h>
namespace GU
{
    UUID Asset::insertMesh(const std::filesystem::path& filepath)
    {
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end())
        {
            return found->second;
        }

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
        MeshNode::read(g_CoreContext.g_vulkanContext, meshnode, (GLOBAL_MODEL_PATH / filepath).generic_string());
        UUID id;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Mesh);
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    UUID Asset::insertMeshWithUUID(const std::filesystem::path& filepath, UUID uuid)
    {
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end())
        {
            return found->second;
        }

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
        MeshNode::read(g_CoreContext.g_vulkanContext, meshnode, (GLOBAL_MODEL_PATH / filepath).generic_string());
        UUID id = uuid;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Mesh);
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    UUID Asset::insertTexture(const std::filesystem::path& filepath)
    {
        auto found = m_loadedTextureMap.find(filepath);
        if (found != m_loadedTextureMap.end())
        {
            return found->second;
        }

        std::shared_ptr<Texture> texture = Texture::read(filepath);
        UUID id;
        m_textureMap[id] = texture;
        m_loadedTextureMap[filepath] = id;
        return id;
    }
    std::filesystem::path Asset::getMeshPathWithUUID(UUID uuid)
    {
        for(auto && m : m_loadedModelMap)
        {
            if (m.second == uuid)
            {
                return m.first;
            }
        }
        return "";
    }
    UUID Asset::insertTextureWithUUID(const std::filesystem::path& filepath, UUID uuid)
    {
        auto found = m_loadedTextureMap.find(filepath);
        if (found != m_loadedTextureMap.end())
        {
            return found->second;
        }

        std::shared_ptr<Texture> texture = Texture::read(filepath);
        UUID id = uuid;
        m_textureMap[id] = texture;
        m_loadedTextureMap[filepath] = id;
        return id;
    }
    std::shared_ptr<MeshNode> Asset::getMeshWithUUID(UUID uuid)
    {
        return m_meshMap[uuid];
    }
}
