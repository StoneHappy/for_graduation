#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
#include <Global/CoreContext.h>
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
        m_meshMap[id] = meshnode;
        m_loadedModelMap[filepath] = id;
        return id;
    }
    std::shared_ptr<MeshNode> Asset::getMeshWithUUID(UUID uuid)
    {
        return m_meshMap[uuid];
    }
}
