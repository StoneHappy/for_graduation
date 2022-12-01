#include "Asset.h"
#include <Renderer/Mesh.h>
#include <Global/CoreContext.h>
namespace GU
{
    UUID Asset::insertMesh(std::filesystem::path filepath)
    {
        auto found = m_loadedModelMap.find(filepath);
        if (found != m_loadedModelMap.end()) return found->second;

        std::shared_ptr<MeshNode> meshnode = std::make_shared<MeshNode>();
    }
    UUID Asset::getMeshWithUUID(UUID uuid)
    {
        return UUID();
    }
}
