#include "ecs/component_manager.hpp"

void ComponentManager::EntityDestroyed(EntityID entity_id)
{
  for (auto& component : components_map_) {
    component.second->RemoveComponent(entity_id);
  }
}