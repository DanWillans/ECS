#ifndef INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_
#define INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_

#include "ecs/system_signature.hpp"
#include "ids.hpp"

class Entity;

class ISystemManager {
  public:
  virtual void EntityDestroyed(EntityID entity) = 0;
  virtual void EntitySignatureChanged(EntityID entity_id, const SystemSignature& new_entity_signature) = 0;
  [[nodiscard]] virtual SystemSignature& GetEntitySystemSignature(EntityID entity_id) = 0;
};

#endif // !INCLUDE_ECS_SYSTEM_MANAGER_INTERFACE_HPP_