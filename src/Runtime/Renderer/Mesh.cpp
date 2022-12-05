#include <Renderer/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{
	void recursiveBuildMeshTree(aiNode* ainode, MeshNode::MeshTree& meshnode)
	{
		if (ainode->mNumChildren == 1)
		{
			meshnode.isLeft = true;
			return;
		}
		for (size_t i = 0; i < ainode->mNumMeshes; i++)
		{
			MeshNode::MeshTree childnode;
			childnode.id = ainode->mMeshes[i];
			childnode.parentID = meshnode.id;
			meshnode.childrenIDs.push_back(ainode->mMeshes[i]);
		}
	}

	bool buildMeshs(VulkanContext& vulkanContext, const aiScene* scene, std::vector<Mesh>& meshs)
	{
		meshs.reserve(scene->mNumMeshes);
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh mesh;
			aiMesh* aimesh = scene->mMeshes[i];
			memset(mesh.bmin, 0, sizeof(float) * 3);
			memset(mesh.bmax, 0, sizeof(float) * 3);
			memcpy(mesh.bmin, &aimesh->mAABB.mMin, sizeof(float) * 3);
			memcpy(mesh.bmax, &aimesh->mAABB.mMax, sizeof(float) * 3);
			//apply_material(sc->mMaterials[mesh->mMaterialIndex]);
			for (size_t j = 0; j < aimesh->mNumVertices; j++)
			{
				Vertex vertex{};
				vertex.pos = { aimesh->mVertices[j].x , aimesh->mVertices[j].y, aimesh->mVertices[j].z };
				vertex.normal = { aimesh->mNormals[j].x , aimesh->mNormals[j].y, aimesh->mNormals[j].z};
				vertex.texCoord = { aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].x,1 - (aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].y) };
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

			// generate render buuffer
			createVertexBuffer(vulkanContext, mesh.m_vertices, mesh.vertexBuffer, mesh.vertexMemory);
			createIndexBuffer(vulkanContext, mesh.m_indices, mesh.indexBuffer, mesh.indexMemory);
			mesh.id = meshs.size();
			meshs.emplace_back(std::move(mesh));
		}
		return true;
	}

	MeshNode::MeshNode()
	{
		root.isRoot = true;
	}

	MeshNode::~MeshNode()
	{
	}

	bool MeshNode::read(VulkanContext& vulkanContext, std::shared_ptr<MeshNode> meshnode,const std::filesystem::path& filepath)
	{
		::Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filepath.generic_string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
		aiNode* aimeshnode = scene->mRootNode;
		
		recursiveBuildMeshTree(aimeshnode, meshnode->root);
		return buildMeshs(vulkanContext, scene, meshnode->meshs);
	}
}