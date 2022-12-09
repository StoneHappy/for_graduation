#include "AssimpUtils.h"

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
}