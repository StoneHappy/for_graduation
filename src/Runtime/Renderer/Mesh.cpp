#include <Renderer/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{
	glm::mat4 aiMat42glmMat4(aiMatrix4x4 aimat4)
	{
		glm::mat4 rnt;

		rnt[0][0] = aimat4.a1; rnt[0][1] = aimat4.b1;  rnt[0][2] = aimat4.c1; rnt[0][3] = aimat4.d1;
		rnt[1][0] = aimat4.a2; rnt[1][1] = aimat4.b2;  rnt[1][2] = aimat4.c2; rnt[1][3] = aimat4.d2;
		rnt[2][0] = aimat4.a3; rnt[2][1] = aimat4.b3;  rnt[2][2] = aimat4.c3; rnt[2][3] = aimat4.d3;
		rnt[3][0] = aimat4.a4; rnt[3][1] = aimat4.b4;  rnt[3][2] = aimat4.c4; rnt[3][3] = aimat4.d4;

		return rnt;
	}

	void recursiveBuildMeshTree(aiNode* ainode, MeshTree& meshnode)
	{
		if (ainode->mNumChildren == 1)
		{
			meshnode.isLeft = true;
			return;
		}
		for (size_t i = 0; i < ainode->mNumMeshes; i++)
		{
			MeshTree childnode;
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

	bool buildMeshs(VulkanContext& vulkanContext, const aiScene* scene, std::vector<SkeletalMesh>& meshs)
	{
		meshs.reserve(scene->mNumMeshes);
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{
			SkeletalMesh mesh;
			aiMesh* aimesh = scene->mMeshes[i];
			memset(mesh.bmin, 0, sizeof(float) * 3);
			memset(mesh.bmax, 0, sizeof(float) * 3);
			memcpy(mesh.bmin, &aimesh->mAABB.mMin, sizeof(float) * 3);
			memcpy(mesh.bmax, &aimesh->mAABB.mMax, sizeof(float) * 3);
			//apply_material(sc->mMaterials[mesh->mMaterialIndex]);
			for (size_t j = 0; j < aimesh->mNumVertices; j++)
			{
				SkeletalVertex vertex{};
				vertex.pos = { aimesh->mVertices[j].x , aimesh->mVertices[j].y, aimesh->mVertices[j].z };
				vertex.normal = { aimesh->mNormals[j].x , aimesh->mNormals[j].y, aimesh->mNormals[j].z };
				vertex.texCoord = { aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].x,1 - (aimesh->mTextureCoords[0] == nullptr ? 0 : aimesh->mTextureCoords[0][j].y) };
				mesh.m_vertices.push_back(vertex);
			}

			// bone datas
			for (size_t i = 0; i < aimesh->mNumBones; i++)
			{

				// Push new bone info into bones vector. 
				aimesh->mBones[i]->mOffsetMatrix;
				BoneInfo bi;
				bi.boneOffset = aiMat42glmMat4(aimesh->mBones[i]->mOffsetMatrix);
				mesh.boneinfos.push_back(bi);
				// Iterate over all the affected vertices by this bone i.e weights. 
				for (unsigned int j = 0; j < aimesh->mBones[i]->mNumWeights; j++) {

					// Obtain an index to the affected vertex within the array of vertices.
					unsigned int vID = aimesh->mBones[i]->mWeights[j].mVertexId;
					// The value of how much this bone influences the vertex. 
					float weight = aimesh->mBones[i]->mWeights[j].mWeight;

					// Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex. 
					auto vertWeight = mesh.m_vertices[vID].weights;
					auto vertBonids = mesh.m_vertices[vID].boneIDs;
					mesh.m_vertices[vID].boneIDs = { vertWeight.x == 0 ? i : vertBonids.x, vertWeight.y == 0 ? i : vertBonids.y, vertWeight.z == 0 ? i : vertBonids.z,vertWeight.w == 0 ? i : vertBonids.w };
					mesh.m_vertices[vID].weights = { vertWeight.x == 0 ? weight : vertWeight.x, vertWeight.y == 0 ? weight : vertWeight.y, vertWeight.z == 0 ? weight : vertWeight.z,vertWeight.w == 0 ? weight : vertWeight.w };
				}
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

		auto na = scene->mNumAnimations;
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
	SkeletalMeshNode::SkeletalMeshNode()
	{
		root.isRoot = true;
	}
	SkeletalMeshNode::~SkeletalMeshNode()
	{
		
	}
	bool SkeletalMeshNode::read(VulkanContext& vulkanContext, std::shared_ptr<SkeletalMeshNode> meshnode, const std::filesystem::path& filepath)
	{
		::Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filepath.generic_string(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);
		aiNode* aimeshnode = scene->mRootNode;

		recursiveBuildMeshTree(aimeshnode, meshnode->root);
		return buildMeshs(vulkanContext, scene, meshnode->meshs);
	}
}