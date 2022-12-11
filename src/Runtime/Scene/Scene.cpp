#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Component.h>
#include <Renderer/VulkanBuffer.h>
#include <Scene/Asset.h>
#include <Function/Animation/Animation.h>
namespace GU
{
	template<typename... Component>
	static void copyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
			{
				if (src.hasComponent<Component>())
				dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
			}(), ...);
	}

	template<typename... Component>
	static void copyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		copyComponentIfExists<Component...>(dst, src);
	}


	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}
	Entity Scene::createEntity(const std::string& name)
	{
		return createEntityWithUUID(UUID(), name);
	}

	Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_registry.create(), this };
		entity.addComponent<IDComponent>(uuid);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_entityMap[uuid] = entity;
		return entity;
	}

	Entity Scene::duplicateEntity(Entity entity)
	{
		Entity newEntity = createEntity(entity.getName());
		copyComponentIfExists(AllComponents{}, newEntity, entity);
		return newEntity;
	}

	Entity Scene::findEntityByName(std::string_view name)
	{
		auto view = m_registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	Entity Scene::getEntityByUUID(UUID uuid)
	{
		if (m_entityMap.find(uuid) != m_entityMap.end())
			return { m_entityMap.at(uuid), this };

		return {};
	}

	void Scene::destroyEntity(Entity entity)
	{
		auto uuid = entity.getUUID();
		m_registry.destroy(entity);
		m_entityMap.erase(uuid);
	}

	void Scene::renderTick(VulkanContext& vulkanContext, VkCommandBuffer& cmdBuf, int currImageIndex, float deltaTime)
	{
		// mesh
		{
			auto view = m_registry.view<MaterialComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto&& [materialComponent, transformComponent] = view.get<MaterialComponent, TransformComponent>(entity);
				vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.pipelineLayout, 0, 1, &materialComponent.material.descriptorSets[currImageIndex], 0, nullptr);
				materialComponent.material.modelUBO->update({ transformComponent.getTransform() }, currImageIndex);
				for (auto& mesh : GLOBAL_ASSET->getMeshWithUUID(materialComponent.material.meshUUID)->meshs)
				{
					vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.graphicsPipeline);
					VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(cmdBuf, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(cmdBuf, mesh.m_indices.size(), 1, 0, 0, 0);
				}
			}
		}
		
		// skeletal mesh
		{
			auto view = m_registry.view<SkeletalMeshComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto&& [skeletalComponent, transformComponent] = view.get<SkeletalMeshComponent, TransformComponent>(entity);
				// skeletal animation
				vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->skeletalPipeline);
				vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, GLOBAL_VULKAN_CONTEXT->skeletalPipelineLayout, 0, 1, &skeletalComponent.material.descriptorSets[currImageIndex], 0, nullptr);
				SkeletalModelUBO skeletalubo{};
				
				std::shared_ptr<SkeletalMeshNode> testmeshnode = GLOBAL_ASSET->getSkeletalMeshWithUUID(skeletalComponent.material.skeletalMeshUUID);
				skeletalubo.model = transformComponent.getTransform();
				auto&& animations = GLOBAL_ANIMATION->getAnimationsWithUUID(testmeshnode->meshs[0].animationID);
				//auto&& animation = animations["Armature|Idle"];
				auto&& animation = animations[skeletalComponent.currentAnimation];
				if (animation == nullptr)
				{
					break;
				}
				skeletalComponent.timeintgal += skeletalComponent.speed * GLOBAL_DELTATIME;
				
				if (skeletalComponent.timeintgal > animation->duration)
				{
					skeletalComponent.timeintgal = 1.0;
				}

				animation->updateSkeletalModelUBOWithUUID(skeletalubo, skeletalComponent.timeintgal);
				skeletalComponent.material.modelUBO->update(skeletalubo, currImageIndex);
				for (auto mesh : testmeshnode->meshs)
				{
					VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(cmdBuf, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
					vkCmdDrawIndexed(cmdBuf, mesh.m_indices.size(), 1, 0, 0, 0);
				}
			}
		}
	}

	void Scene::initEntityResource()
	{
		auto view = m_registry.view<MaterialComponent>();
		for (auto entity : view)
		{
			auto&& materialComponent = view.get<MaterialComponent>(entity);
			materialComponent.createDescritorSets();
		}
	}

	void Scene::releaseEntityResource()
	{
		{
			auto view = m_registry.view<MaterialComponent>();
			for (auto entity : view)
			{
				auto&& materialComponent = view.get<MaterialComponent>(entity);
				materialComponent.material.modelUBO.reset();
			}
		}
		
		{
			auto view = m_registry.view<SkeletalMeshComponent>();
			for (auto entity : view)
			{
				auto&& materialComponent = view.get<SkeletalMeshComponent>(entity);
				materialComponent.material.modelUBO.reset();
			}
		}
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}
	template<>
	void Scene::OnComponentAdded<SkeletalMeshComponent>(Entity entity, SkeletalMeshComponent& component)
	{
	}
}