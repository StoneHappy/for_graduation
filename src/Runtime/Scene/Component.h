#pragma once
#include <Core/UUID.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Renderer/Mesh.h>
#include <memory>
#include <Renderer/Material.h>
class rcPolyMesh;

namespace GU
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 getTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct MaterialComponent
	{
		MaterialComponent(uint64_t modelid, uint64_t textureid);

		Material material;
		MaterialComponent() = default;

		void createDescritorSets();
		void destoryUBO();
	};

	struct NavMeshComponent
	{
		NavMeshComponent(rcPolyMesh* m_pmesh);
		~NavMeshComponent() = default;

		uint64_t pmesh_uuid;
	};

	struct SkeletalMeshComponent
	{
		SkeletalMeshComponent(uint64_t modelid, uint64_t textureid);

		SkeletalMaterial material;
		SkeletalMeshComponent() = default;
		SkeletalMeshComponent(const SkeletalMeshComponent&);
		void createDescritorSets();
		void destoryUBO();
		std::string currentAnimation;
		float timeintgal = 1.0;
		float speed = 24.0;
	};

	struct AgentComponent
	{
		AgentComponent() = default;
		AgentComponent(int idx, const glm::vec3& targetpos);
		void createDescritorSets();
		void destoryUBO();

		int idx;
		glm::vec3 targetPos;
		std::vector<VkDescriptorSet> descriptorSets;
		std::shared_ptr<VulkanUniformBuffer<AgentModelUBO> > modelUBO;

		std::string currentAnimation;
		float timeintgal = 1.0;
		float speed = 24.0;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, MaterialComponent,
		SkeletalMeshComponent>;
}