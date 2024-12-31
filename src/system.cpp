#include "result.hpp"
#include "ecs/system.hpp"
#include "ecs/system_signature.hpp"

[[nodiscard]] Result<EntitySet*> System::RegisterSystemSignature(const SystemSignature& signature)
{
  if (entity_set_count_ > MAXIMUM_ENTITY_SETS) {
    return Error{ "Maximum entity set" };
  }
  entity_sets[entity_set_count_].first = signature;
  return &(entity_sets[entity_set_count_++].second);
}

void System::UpdateSystem(const float& delta_time)
{
  // Call user implemented Update() function first
  Update(delta_time);

  // Iterate over entities marked for deletion and delete them.
  for (auto& entity : entities_to_delete_) {
    entity.Destroy();
  }

  // Clear deleted entities for next iteration
  entities_to_delete_.clear();
}

void System::MarkEntityForDeletion(const Entity& entity) { entities_to_delete_.insert(entity); }