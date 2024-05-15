#ifndef INCLUDE_ECS_SYSTEM_HPP_
#define INCLUDE_ECS_SYSTEM_HPP_

#include <unordered_map>

#include "ankerl/unordered_dense.h"
#include "ecs/component_manager.hpp"
#include "entt/entt.hpp"

#include "ecs/entity.hpp"
#include "ecs/system_signature.hpp"
#include "ids.hpp"

// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// It also includes some helper functions to the System.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
class System
{
public:
  ankerl::unordered_dense::set<Entity> entities;
  // std::unordered_set<Entity> entities;
  SystemSignature signature;
};

#endif // !INCLUDE_ECS_SYSTEM_HPP_