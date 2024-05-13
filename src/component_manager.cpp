#include "ecs/component_manager.hpp"

  void ComponentManager::EntityDestroyed(EntityID entity_id){
    for(size_t i = 0; i < component_index_count_; ++i){
      components_[i]->RemoveComponent(entity_id);
    }
  }