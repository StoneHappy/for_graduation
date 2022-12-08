#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <string>
namespace GU
{
	class aiScene;
	struct SkeletalModelUBO;

	struct AnimationKey
	{
		std::string boneName;
		glm::mat4 position = glm::mat4(1);
		glm::mat4 rotation = glm::mat4(1);
		float time;
	};

	class Action : public std::vector<AnimationKey>
	{
	public:
		std::string name;
		glm::mat4 interpolation(float timetick);
	};

	class Actions : public std::vector<Action> {};

	class Animation
	{
	public:
		Animation();
		~Animation();

		uint64_t insert(aiScene* scene);

		void updateSkeletalModelUBOWithUUID(uint16_t, const std::string& actioNanme, SkeletalModelUBO& skeleltalmodeubo);
	private:
		std::unordered_map<uint64_t, Actions> actionsMap;
	};

	
}