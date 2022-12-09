#include "Animation.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/matrix_interpolation.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
					auto positionkey = pNodeAnim->mPositionKeys[k];

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
		/*if (size() < 2)
		{
			std::cout << "animation key size < 2" << std::endl;
			return glm::mat4(1);
		}

		int nextNo = -1;
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].time > timetick) nextNo = i;
		}

		if (nextNo == -1)
		{
			return glm::mat4(1);
		}

		glm::mat4 interpolationPos = glm::interpolate((*this)[nextNo - 1].position, (*this)[nextNo].position, timetick - (*this)[nextNo - 1].time);
		glm::mat4 interpolationRot = glm::interpolate((*this)[nextNo - 1].rotation, (*this)[nextNo].rotation, timetick - (*this)[nextNo - 1].time);

		return interpolationPos * interpolationRot;*/

		return glm::mat4(1);
	}
}