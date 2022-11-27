#include <Scene/Entity.h>

namespace GU
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_entityHandle(handle), m_scene(scene)
	{
	}
}