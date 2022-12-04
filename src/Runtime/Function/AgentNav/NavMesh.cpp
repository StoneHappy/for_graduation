#include "NavMesh.h"
namespace GU
{
    NavMesh::NavMesh()
    {

    }
    NavMesh::~NavMesh()
    {
    }
    std::shared_ptr<NavMesh> NavMesh::createFromGUMesh(const Mesh& mesh)
    {
        std::shared_ptr<NavMesh> nvMesh = std::make_shared<NavMesh>();
        int vcap = 0;
        int fcap = 0;
        for (size_t i = 0; i < mesh.m_vertices.size(); i++)
        {
            nvMesh->rcMesh.addVertex(mesh.m_vertices[i].pos.x, mesh.m_vertices[i].pos.y, mesh.m_vertices[i].pos.z, vcap);
        }

        for (size_t i = 0; i < mesh.m_indices.size(); i+=3)
        {
            nvMesh->rcMesh.addTriangle(mesh.m_indices[i], mesh.m_indices[i+1], mesh.m_indices[i+2], fcap);
        }
        return nvMesh;
    }
}
