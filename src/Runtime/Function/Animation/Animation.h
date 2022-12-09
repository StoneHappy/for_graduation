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
	struct PositionKey
	{
		glm::mat4 position = glm::mat4(1);
		float time;
	};
	struct RotationKey
	{
		glm::mat4 rotation = glm::mat4(1);
		float time;
	};

	class Action
	{
	public:
		std::string nodeName; // node name or bone name
		glm::mat4 interpolation(float timetick);

		std::vector<PositionKey> positionKeys;
		std::vector<RotationKey> rotationKeys;
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