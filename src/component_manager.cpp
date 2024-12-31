#include "ecs/component_manager.hpp"

void ComponentManager::EntityDestroyed(EntityID entity_id)
{
  for (auto& component : components_) {
    component->RemoveComponent(entity_id);
  }
}