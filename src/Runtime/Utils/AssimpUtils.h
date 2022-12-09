#pragma once
#include <glm/glm.hpp>
#include <assimp/matrix4x4.h>
namespace GU
{
	glm::mat4 aiMat42glmMat4(aiMatrix4x4 aimat4);
}