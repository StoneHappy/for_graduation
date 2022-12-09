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
namespace GU
{
	uint64_t Animation::addAnimation(const aiScene* scene)
	{
		for (size_t i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation* pAnimation = scene->mAnimations[i];
			for (size_t j = 0; j < pAnimation->mNumChannels; j++)
			{
				Action action;
				aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];
				action.nodeName = std::string(pNodeAnim->mNodeName.data);

				for (size_t k = 0; k < pNodeAnim->mNumPositionKeys; k++)
				{
					auto positionkeyValue = pNodeAnim->mPositionKeys[k].mValue;
					glm::mat4 glmPositionKeyMat = glm::translate(glm::mat4(1), { positionkeyValue.x, positionkeyValue.y ,positionkeyValue.z });
					float time = pNodeAnim->mPositionKeys[k].mTime;
					action.positionKeys.push_back({ time, glmPositionKeyMat });
				}

				for (size_t k = 0; k < pNodeAnim->mNumPositionKeys; k++)
				{
					auto rotaionkeyValue = aiMatrix4x4(pNodeAnim->mRotationKeys[k].mValue.GetMatrix());
					glm::mat4 glmRotationKeyMat = aiMat42glmMat4(rotaionkeyValue);
					float time = pNodeAnim->mRotationKeys[k].mTime;
					action.rotationKeys.push_back({ time, glmRotationKeyMat });
				}
			}
		}
		// Use the first animation 
		return 0;
	}
	void Animation::updateSkeletalModelUBOWithUUID(uint16_t, const std::string& actioNanme, SkeletalModelUBO& skeleltalmodeubo)
	{

	}
	glm::mat4 Action::interpolation(float timetick)
	{
		glm::mat4 interpolationpos = interpolatePostion(timetick);
		glm::mat4 interpolationrot = interpolateRotation(timetick);
		return interpolationpos * interpolationrot;
	}
	glm::mat4 Action::interpolateRotation(float timetick)
	{
		if (rotationKeys.size() < 2)
		{
			std::cout << "rotation key size < 2" << std::endl;
			return glm::mat4(1);
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
			std::cout << "rotation key size < 2" << std::endl;
			return glm::mat4(1);
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