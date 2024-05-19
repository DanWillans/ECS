#include "ecs/entity_manager.hpp"
#include "ecs/ecs_constants.hpp"
#include "ids.hpp"
#include "result.hpp"


Result<EntityID> EntityManager::CreateEntity()
{
  if (entity_count_ == MAX_ENTITY_COUNT + 1 && free_slots_.empty()) {
    return "Max entity count reached, cannot create anymore entities";
  }
  // Check if there are any free entity slots to use first
  if (!free_slots_.empty()) {
    auto entity_id = free_slots_.front();
    free_slots_.pop();
    return EntityID(entity_id);
  }
  // No free slots so increment the entity_count
  return EntityID(entity_count_++);
}

void EntityManager::DestroyEntity(EntityID entity_id) { free_slots_.push(entity_id.Get()); }

uint64_t EntityManager::EntityCount() const { return entity_count_ - 1 - free_slots_.size(); }