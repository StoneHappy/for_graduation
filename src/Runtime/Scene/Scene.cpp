#include <Scene/Scene.h>
#include <Scene/Entity.h>
#include <Scene/Component.h>
#include <Renderer/VulkanBuffer.h>
namespace GU
{

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
		m_registry.destroy(entity);
		m_entityMap.erase(entity.getUUID());
	}

	void Scene::renderTick(VulkanContext& vulkanContext, VkCommandBuffer& cmdBuf, int currImageIndex, float deltaTime)
	{
		auto view = m_registry.view<MeshComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto&& [meshComponet, transform] = view.get<MeshComponent, TransformComponent>(entity);
			updateMeshUniformBuffer(GLOBAL_VULKANCONTEXT, transform.getTransform(), currImageIndex, GLOBAL_VULKANCONTEXT.meshUniformBuffersMapped);
			for (auto& mesh : GLOBAL_ASSET.getMeshWithUUID(meshComponet.meshID)->meshs)
			{
				vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.graphicsPipeline);
				vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext.pipelineLayout, 0, 1, &vulkanContext.descriptorSets[currImageIndex], 0, nullptr);
				VkBuffer vertexBuffers[] = { mesh.vertexBuffer };
				VkDeviceSize offsets[] = { 0 };
				vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(cmdBuf, mesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(cmdBuf, mesh.m_indices.size(), 1, 0, 0, 0);
			}
			
		}
	}
}