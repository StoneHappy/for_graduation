#pragma once
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <Renderer/VulkanUniformBuffer.hpp>
#include <assimp/scene.h>
#include <memory>
namespace GU
{
	struct ActionTree
	{
		std::string nodeName;

		std::vector<std::shared_ptr<ActionTree> > children;
		std::shared_ptr<ActionTree> parent;

		bool isRoot = false;
		bool isLeft = false;
	};

	class Action
	{
	public:
		std::string nodeName; // node name or bone name
		glm::mat4 interpolation(float timetick);

		std::vector<std::pair<float, glm::mat4> > positionKeys;
		std::vector<std::pair<float, glm::mat4> > rotationKeys;

		glm::mat4 finalTransfrom; // Final transformation to apply to vertices 
		glm::mat4 globalTransfrom;
		glm::mat4 offset; // Initial offset from local to bone space. 
	private:
		glm::mat4 interpolateRotation(float timetick);
		glm::mat4 interpolatePostion(float timetick);

	private:

	};

	class Animation
	{
	public:
		std::string animationName;

		std::unordered_map<std::string, std::shared_ptr<Action>> actions;
		
		std::unordered_map<std::string, uint32_t> boneIndexMap;

		void updateSkeletalModelUBOWithUUID(SkeletalModelUBO& skeleltalmodeubo, float timetick);
		glm::mat4 calculateBoneTransformMat(const std::string& actionname, float timetick);

		std::shared_ptr<ActionTree> actiontree;
	};

	class AnimationManager
	{
	public:
		AnimationManager() = default;
		~AnimationManager() = default;

		uint64_t addAnimation(const aiScene* scene, const aiMesh* aimesh);
		std::unordered_map<std::string, std::shared_ptr<Animation> >& getAnimationsWithUUID(uint64_t uuid);
	private:
		std::unordered_map<uint64_t, std::unordered_map<std::string, std::shared_ptr<Animation> > > animationMap;
	};

	
}