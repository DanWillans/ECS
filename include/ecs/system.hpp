#ifndef INCLUDE_ECS_SYSTEM_HPP_
#define INCLUDE_ECS_SYSTEM_HPP_

#include <unordered_map>

#include "ankerl/unordered_dense.h"
#include "entt/entt.hpp"

#include "ecs/entity.hpp"
#include "ecs/system_signature.hpp"
#include "ids.hpp"

// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
class System
{
public:
  // std::unordered_map<EntityID, Entity> entities;
  ankerl::unordered_dense::set<Entity> entities;
  SystemSignature signature;
};

#endif // !INCLUDE_ECS_SYSTEM_HPP_