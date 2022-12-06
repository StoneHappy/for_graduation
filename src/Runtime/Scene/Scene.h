#pragma once
#include <string>
#include <entt/entt.hpp>
#include <Core/UUID.h>
#include <Renderer/VulkanContext.h>
#include <QVulkanDeviceFunctions>
namespace GU
{

	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string());

		Entity createEntityWithUUID(UUID uuid, const std::string& name);

		Entity findEntityByName(std::string_view name);
		Entity getEntityByUUID(UUID uuid);
		void destroyEntity(Entity entity);

		void renderTick(VulkanContext& vulkanContext, VkCommandBuffer& cmdBuf, int currImageIndex, float deltaTime);

		void initEntityResource();
		void releaseEntityResource();

		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	public:
		entt::registry m_registry;
		std::unordered_map<UUID, entt::entity> m_entityMap;
		
		friend class Entity;
	};


	

}