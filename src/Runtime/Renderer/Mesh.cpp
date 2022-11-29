#include <Renderer/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{
	bool readMesh(const VulkanContext& vulkanContext, MeshNode& meshnode, const char* filePath)
	{
		::Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh mesh;
			aiMesh* aimesh = scene->mMeshes[i];
			//apply_material(sc->mMaterials[mesh->mMaterialIndex]);
			for (size_t j = 0; j < aimesh->mNumVertices; j++)
			{
				Vertex vertex{};
				vertex.pos = { aimesh->mVertices[j].x , aimesh->mVertices[j].y, aimesh->mVertices[j].z };
				vertex.texCoord = { aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].x,1- (aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].y) };
				mesh.m_vertices.push_back(vertex);
			}

			for (size_t i = 0; i < aimesh->mNumFaces; i++)
			{
				const aiFace* face = &aimesh->mFaces[i];

				if (face->mNumIndices != 3)
				{
						return false;
				}
				mesh.m_indices.push_back(face->mIndices[0]);
				mesh.m_indices.push_back(face->mIndices[1]);
				mesh.m_indices.push_back(face->mIndices[2]);
			}

			createVertexBuffer(vulkanContext, mesh.m_vertices, mesh.vertexBuffer, mesh.vertexMemory);
			createIndexBuffer(vulkanContext, mesh.m_indices, mesh.indexBuffer, mesh.indexMemory);
			mesh.id = meshnode.m_meshs.size();
			meshnode.m_meshs.emplace_back(std::move(mesh));
		}
		return true;
	}
}