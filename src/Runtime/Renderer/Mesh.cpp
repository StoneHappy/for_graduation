#include <Renderer/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Renderer/VulkanBuffer.h>
#include <Global/CoreContext.h>
#include <Function/Animation/Animation.h>
#include <Utils/AssimpUtils.h>
namespace GU
{
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

			// import animations
			aiMesh* aimesh = scene->mMeshes[i];
			mesh.animationID = GLOBAL_ANIMATION->addAnimation(scene, aimesh);
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
			glm::mat4 globaltransfrom;
			// bone datas
			for (size_t i = 0; i < aimesh->mNumBones; i++)
			{

				// Push new bone info into bones vector. 
				BoneInfo bi;
				auto roottransform = scene->mRootNode->mTransformation;
				roottransform.Inverse();
				globaltransfrom = aiMat42glmMat4(roottransform);
				auto offset = aimesh->mBones[i]->mOffsetMatrix;
				bi.boneOffset = aiMat42glmMat4(offset);
				mesh.boneinfos.push_back(bi);
				// Iterate over all the affected vertices by this bone i.e weights. 
				for (unsigned int j = 0; j < aimesh->mBones[i]->mNumWeights; j++) {

					// Obtain an index to the affected vertex within the array of vertices.
					unsigned int vID = aimesh->mBones[i]->mWeights[j].mVertexId;
					// The value of how much this bone influences the vertex. 
					float weight = aimesh->mBones[i]->mWeights[j].mWeight;

					// Insert bone data for particular vertex ID. A maximum of 4 bones can influence the same vertex. 
					for (unsigned int k = 0; k < 4; k++) {
						// Check to see if there are any empty weight values. 
						if (mesh.m_vertices[vID].weights[k] == 0.0) {
							// Add ID of bone. 
							mesh.m_vertices[vID].weights[k] = weight;
							mesh.m_vertices[vID].boneIDs[k] = i;
							break;
						}

					}
				}
			}
			// Use the first animation 
			const aiAnimation* pAnimation = scene->mAnimations[0];

			const aiNodeAnim* pNodeAnim = NULL;
			std::vector<aiNodeAnim*> aaa;
			
			auto bone0 = pAnimation->mChannels[1];
			auto bone1 = pAnimation->mChannels[2];
			auto pbon0 = glm::translate(glm::mat4(1), { bone0->mPositionKeys[bone0->mNumPositionKeys - 1].mValue.x, bone0->mPositionKeys[bone0->mNumPositionKeys - 1].mValue.y, bone0->mPositionKeys[bone0->mNumPositionKeys - 1].mValue.z });
			auto pbon1 = glm::translate(glm::mat4(1), { bone1->mPositionKeys[bone1->mNumPositionKeys - 1].mValue.x, bone1->mPositionKeys[bone1->mNumPositionKeys - 1].mValue.y, bone1->mPositionKeys[bone1->mNumPositionKeys - 1].mValue.z });

			auto rbon0 = aiMat42glmMat4(aiMatrix4x4(bone0->mRotationKeys[bone0->mNumRotationKeys - 1].mValue.GetMatrix()));
			auto rbon1 = aiMat42glmMat4(aiMatrix4x4(bone1->mRotationKeys[bone1->mNumRotationKeys - 1].mValue.GetMatrix()));

			auto animations = GLOBAL_ANIMATION->getAnimationWithUUID(mesh.animationID);
			auto animation = animations["Armature|Action0"];
			//mesh.boneinfos[0].boneOffset = globaltransfrom * animation->actions["Bone"]->interpolation(5.0) * mesh.boneinfos[0].boneOffset;
			mesh.boneinfos[0].boneOffset = globaltransfrom * animation->calculateBoneTransformMat("Bone", 5.0) * mesh.boneinfos[0].boneOffset;
			mesh.boneinfos[1].boneOffset = globaltransfrom * animation->calculateBoneTransformMat("Bone.001", 5.0) * mesh.boneinfos[1].boneOffset;

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