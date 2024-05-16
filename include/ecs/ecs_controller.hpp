#ifndef INCLUDE_ECS_CONTROLLER_H_
#define INCLUDE_ECS_CONTROLLER_H_

#include "ecs/component_manager.hpp"
#include "ecs/entity.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system_manager.hpp"
#include "ids.hpp"
#include "result.hpp"


#include <memory>


class ECSController
{
public:
  ECSController()
    : component_manager_(std::make_unique<ComponentManager>()), entity_manager_(std::make_unique<EntityManager>()),
      system_manager_(std::make_unique<SystemManager>(component_manager_.get(), entity_manager_.get()))
  {}

  Result<Entity> CreateEntity()
  {
    Result<EntityID> entity_id = entity_manager_->CreateEntity();
    if (entity_id.Bad()) {
      return entity_id.Error();
    }
    return Entity{ system_manager_.get(), component_manager_.get(), entity_manager_.get(), *entity_id };
  }

  template<typename ComponentName> [[nodiscard]] ComponentID<ComponentName> RegisterComponent()
  {
    return component_manager_->RegisterComponent<ComponentName>();
  }

  template<typename SystemName, typename... Args>
  [[nodiscard]] SystemID<SystemName> RegisterSystem(const SystemSignature& signature, Args... args)
  {
    return system_manager_->RegisterSystem<SystemName>(signature, args...);
  }

  template<typename SystemName> SystemName& GetSystem(SystemID<SystemName> system_id)
  {
    return system_manager_->GetSystem(system_id);
  }

  [[nodiscard]] uint64_t EntityCount() const { return entity_manager_->EntityCount(); }

  template<typename ComponentName>
  [[nodiscard]] size_t ComponentCount(ComponentID<ComponentName> id) const
  {
    return component_manager_->GetComponentCount(id);
  }

private:
  // Create these all on the heap because they could be quite large
  std::unique_ptr<ComponentManager> component_manager_;
  std::unique_ptr<EntityManager> entity_manager_;
  std::unique_ptr<SystemManager> system_manager_;
};

#endif