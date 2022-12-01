#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
namespace GU
{
    UUID Asset::insertMesh(const std::filesystem::path& filepath)
    {
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end()) return found->second;

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
        MeshNode::read(g_CoreContext.g_vulkanContext, meshnode, filepath);
        UUID id;
        m_meshMap[id] = meshnode;
        return id;
    }
    std::shared_ptr<MeshNode> Asset::getMeshWithUUID(UUID uuid)
    {
        return m_meshMap[uuid];
    }
}
