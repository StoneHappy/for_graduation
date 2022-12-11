#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
#include <MainWindow.h>
#include <Renderer/Texture.h>
#include <Core/Project.h>
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
        MeshNode::read(*GLOBAL_VULKAN_CONTEXT, meshnode, (GLOBAL_MODEL_PATH / filepath).generic_string());
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
        MeshNode::read(*GLOBAL_VULKAN_CONTEXT, meshnode, (GLOBAL_MODEL_PATH / filepath).generic_string());
        UUID id = uuid;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Mesh);
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    UUID Asset::insertSkeletalMesh(const std::filesystem::path& filepath)
    {
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end())
        {
            return found->second;
        }

        std::shared_ptr<SkeletalMeshNode> meshnode = std::make_shared<SkeletalMeshNode>();
        SkeletalMeshNode::read(*GLOBAL_VULKAN_CONTEXT, meshnode, (GLOBAL_MODEL_PATH / filepath).generic_string());
        UUID id;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Mesh);
        m_skeletalMeshMap[id] = meshnode;
        m_loadedSkeletalModelMap[filepath] = id;
        return id;
    }
    UUID Asset::insertSkeletalMeshWithUUID(const std::filesystem::path& filepath, UUID uuid)
    {
        return UUID();
    }
    UUID Asset::insertTexture(const std::filesystem::path& filepath)
    {
        auto found = m_loadedTextureMap.find(filepath);
        if (found != m_loadedTextureMap.end())
        {
            return found->second;
        }

        std::shared_ptr<Texture> texture = Texture::read((GLOBAL_TEXTURE_PATH / filepath).generic_string());
        UUID id;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Texture);
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

    std::filesystem::path Asset::getTexturePathWithUUID(UUID uuid)
    {
        for (auto&& t : m_loadedTextureMap)
        {
            if (t.second == uuid)
            {
                return t.first;
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

        std::shared_ptr<Texture> texture = Texture::read((GLOBAL_TEXTURE_PATH / filepath).generic_string());
        UUID id = uuid;
        GLOBAL_MAINWINDOW->importResource2Table(filepath.filename().string().c_str(), id, (int)AssetType::Texture);
        m_textureMap[id] = texture;
        m_loadedTextureMap[filepath] = id;
        return id;
    }
    std::shared_ptr<MeshNode> Asset::getMeshWithUUID(UUID uuid)
    {
        return m_meshMap[uuid];
    }

    std::shared_ptr<SkeletalMeshNode> Asset::getSkeletalMeshWithUUID(UUID uuid)
    {
        return m_skeletalMeshMap[uuid];
    }

    std::shared_ptr<Texture> Asset::getTextureWithUUID(UUID uuid)
    {
        return m_textureMap[uuid];
    }

}
