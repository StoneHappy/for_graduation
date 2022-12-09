#include "Animation.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Utils/AssimpUtils.h>
#include <Core/UUID.h>
namespace GU
{
	void buildActiontree(const aiScene* scene,const aiNode* node, std::shared_ptr<ActionTree> actionTree)
	{
		if (node->mNumChildren == 0)
		{
			actionTree->isLeft = true;
			return;
		}
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			std::shared_ptr<ActionTree> actree = std::make_shared<ActionTree>();
			actree->nodeName = std::string(node->mChildren[i]->mName.data);
			actree->parent = actionTree;
			actionTree->children.push_back(actree);
			buildActiontree(scene, node->mChildren[i], actree);
		};
	}

	const aiNode* findArmaturNode(const aiNode* node)
	{
		const aiNode* rnt = nullptr;
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			const aiNode* childnode = node->mChildren[i];
			if (std::string(childnode->mName.data) == "Armature")
			{
				return childnode;
			}
			rnt = findArmaturNode(childnode);
		}

		return rnt;
	}

	std::shared_ptr<ActionTree> findActionTreeNodeWithName(std::shared_ptr<ActionTree> parent ,const std::string& name)
	{
		for (auto&& child : parent->children)
		{
			if (child->nodeName == name) return child;

			auto rnt = findActionTreeNodeWithName(child, name);

			if (rnt !=nullptr && rnt->nodeName == name) return rnt;
		}

		return nullptr;
	}

	uint64_t AnimationManager::addAnimation(const aiScene* scene, const aiMesh* aimesh)
	{
		std::unordered_map<std::string, std::shared_ptr<Animation>> animations;
		for (size_t i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation* pAnimation = scene->mAnimations[i];
			std::shared_ptr<Animation> animation = std::make_shared<Animation>();
			animation->animationName = pAnimation->mName.C_Str();

			// build action tree
			animation->actiontree = std::make_shared<ActionTree>();
			animation->actiontree->isLeft = false;
			animation->actiontree->isRoot = true;
			animation->actiontree->nodeName = "Armature";
			auto armaturNode = findArmaturNode(scene->mRootNode);
			buildActiontree(scene, armaturNode, animation->actiontree);

			// get bone keys
			for (size_t j = 0; j < pAnimation->mNumChannels; j++)
			{
				std::shared_ptr<Action> action = std::make_shared<Action>();
				aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];
				// remove unnessary nodes
				if (std::string(pNodeAnim->mNodeName.data) == "Armature") continue;
				
				// add node name bone name
				action->nodeName = std::string(pNodeAnim->mNodeName.data);

				// root transform
				auto roottransform = scene->mRootNode->mTransformation;
				roottransform.Inverse();
				action->globalTransfrom = aiMat42glmMat4(roottransform);

				// bone offset and index
				for (size_t b = 0; b < aimesh->mNumBones; b++)
				{
					animation->boneIndexMap[aimesh->mBones[b]->mName.C_Str()] = b;
					if (aimesh->mBones[b]->mName.C_Str() == pNodeAnim->mNodeName.data) action->offset = aiMat42glmMat4(aimesh->mBones[b]->mOffsetMatrix);
				}

				// animation keys
				for (size_t k = 0; k < pNodeAnim->mNumPositionKeys; k++)
				{
					auto positionkeyValue = pNodeAnim->mPositionKeys[k].mValue;
					glm::mat4 glmPositionKeyMat = glm::translate(glm::mat4(1), { positionkeyValue.x, positionkeyValue.y ,positionkeyValue.z });
					float time = pNodeAnim->mPositionKeys[k].mTime;
					action->positionKeys.push_back({ time, glmPositionKeyMat });
				}

				for (size_t k = 0; k < pNodeAnim->mNumRotationKeys; k++)
				{
					auto rotaionkeyValue = aiMatrix4x4(pNodeAnim->mRotationKeys[k].mValue.GetMatrix());
					glm::mat4 glmRotationKeyMat = aiMat42glmMat4(rotaionkeyValue);
					float time = pNodeAnim->mRotationKeys[k].mTime;
					action->rotationKeys.push_back({ time, glmRotationKeyMat });
				}
				animation->actions[action->nodeName] = action;
			}
			animations[animation->animationName] = animation;
		}
		UUID uuid;
		animationMap[uuid] = animations;
		return uuid;
	}
	std::unordered_map<std::string, std::shared_ptr<Animation> >& AnimationManager::getAnimationWithUUID(uint64_t uuid)
	{
		return animationMap[uuid];
	}
	void Animation::updateSkeletalModelUBOWithUUID(SkeletalModelUBO& skeleltalmodeubo)
	{
		for (auto&& action : actions)
		{
			uint32_t boneIndex = boneIndexMap[action.first];
			skeleltalmodeubo.bones[boneIndex];
		}
	}
	glm::mat4 Animation::calculateBoneTransformMat(const std::string& actionname, float timetick)
	{
		std::shared_ptr<ActionTree> findingActionTree = findActionTreeNodeWithName(actiontree, actionname);

		if (findingActionTree == nullptr) return glm::mat4(1);

		glm::mat4 transform = glm::mat4(1);
		while (!findingActionTree->isRoot)
		{
			transform = actions[findingActionTree->nodeName]->interpolation(timetick) * transform;
			findingActionTree = findingActionTree->parent;
		}

		return transform;
	}
	glm::mat4 Action::interpolation(float timetick)
	{
		glm::mat4 interpolationpos = interpolatePostion(timetick);
		glm::mat4 interpolationrot = interpolateRotation(timetick);
		return  interpolationpos * interpolationrot;
	}
	glm::mat4 Action::interpolateRotation(float timetick)
	{
		if (rotationKeys.size() < 2)
		{
			return rotationKeys[0].second;
		}

		int nextno = -1;
		for (size_t i = 0; i < rotationKeys.size(); i++)
		{
			if (rotationKeys[i].first > timetick) nextno = i;
		}

		if (nextno == -1)
		{
			return glm::mat4(1);
		}

		glm::mat4 interpolationrot = glm::interpolate(rotationKeys[nextno - 1].second, rotationKeys[nextno].second, timetick - rotationKeys[nextno - 1].first);

		return interpolationrot;
	}
	glm::mat4 Action::interpolatePostion(float timetick)
	{
		if (positionKeys.size() < 2)
		{
			return positionKeys[0].second;
		}

		int nextno = -1;
		for (size_t i = 0; i < positionKeys.size(); i++)
		{
			if (positionKeys[i].first > timetick) nextno = i;
		}

		if (nextno == -1)
		{
			return glm::mat4(1);
		}

		glm::mat4 interpolationpos = glm::interpolate(positionKeys[nextno - 1].second, positionKeys[nextno].second, timetick - positionKeys[nextno - 1].first);

		return interpolationpos;
	}
}