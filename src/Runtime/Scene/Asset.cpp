#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
#include <MainWindow.h>
namespace GU
{
    UUID Asset::insertMesh(const std::filesystem::path& filepath)
    {
        DEBUG_LOG("loading mesh %s", filepath.string().c_str());
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end())
        {
            DEBUG_LOG("mesh(%s) has been loaded!", filepath.string().c_str());
            return found->second;
        }

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
        MeshNode::read(g_CoreContext.g_vulkanContext, meshnode, filepath);
        UUID id;
        GLOBAL_MAINWINDOW->importMesh2Table(filepath.filename().string().c_str(), id);
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    UUID Asset::insertMeshWithUUID(const std::filesystem::path& filepath, UUID uuid)
    {
        DEBUG_LOG("loading mesh %s", filepath.string().c_str());
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end())
        {
            DEBUG_LOG("mesh(%s) has been loaded!", filepath.string().c_str());
            return found->second;
        }

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
        MeshNode::read(g_CoreContext.g_vulkanContext, meshnode, filepath);
        UUID id = uuid;
        GLOBAL_MAINWINDOW->importMesh2Table(filepath.filename().string().c_str(), id);
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    std::shared_ptr<MeshNode> Asset::getMeshWithUUID(UUID uuid)
    {
        return m_meshMap[uuid];
    }
}
