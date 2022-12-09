#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <Renderer/VulkanUniformBuffer.hpp>
#include <assimp/scene.h>
namespace GU
{
	class Action
	{
	public:
		std::string nodeName; // node name or bone name
		glm::mat4 interpolation(float timetick);

		std::vector<std::pair<float, glm::mat4> > positionKeys;
		std::vector<std::pair<float, glm::mat4> > rotationKeys;
	private:
		glm::mat4 interpolateRotation(float timetick);
		glm::mat4 interpolatePostion(float timetick);

	private:

	};

	class Animation
	{
	public:
		Animation() = default;
		~Animation() = default;

		uint64_t addAnimation(const aiScene* scene);
		void updateSkeletalModelUBOWithUUID(uint16_t, const std::string& actioNanme, SkeletalModelUBO& skeleltalmodeubo);
	private:
		std::unordered_map<uint64_t, std::vector<Action> > actionsMap;
	};

	
}