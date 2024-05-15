#ifndef INCLUE_ECS_ENTITY_MANAGER_H_
#define INCLUE_ECS_ENTITY_MANAGER_H_

#include <queue>

#include "ids.hpp"
#include "result.hpp"

class EntityManager
{
public:
  Result<EntityID> CreateEntity();
  void DestroyEntity(EntityID entity_id);
  [[nodiscard]] uint64_t EntityCount() const;

private:
  size_t entity_count_{ 0 };
  std::queue<size_t> free_slots_;
};

#endif