#ifndef INCLUDE_ECS_ENTITY_HPP_
#define INCLUDE_ECS_ENTITY_HPP_

#include "ecs/component_array.hpp"
#include "ecs/component_manager.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system_manager_interface.hpp"


// A convenience class for handling an Entity. It shouldn't hold any state other than an EntityID
// We want to be able to add and remove Components from Entities
class Entity
{
public:
  [[nodiscard]] EntityID GetID() const { return id_; }

  template<typename ComponentName> [[nodiscard]] Error AddComponent(const ComponentName& component = ComponentName{})
  {
    // Add this entities component to the component manager
    auto err = component_manager_->AddComponent<ComponentName>(id_, component);
    if (err.Good()) {
      // Update the system manager with the entities new system signature.
      SystemSignature& current_signature = system_manager_->GetEntitySystemSignature(id_);
      auto comp_id = component_manager_->GetComponentID<ComponentName>();
      if (comp_id.Good()) {
        current_signature.SetComponent(*comp_id);
        system_manager_->EntitySignatureChanged(id_, current_signature);
      } else {
        err = comp_id.Error();
      }
    }
    return err;
  }

  template<typename ComponentName> [[nodiscard]] Error RemoveComponent()
  {
    // Remove this entities component from the component manager
    auto err = component_manager_->RemoveComponent<ComponentName>(id_);
    if (err.Good()) {
      // Update the system manager with the entities new system signature
      SystemSignature current_signature = system_manager_->GetEntitySystemSignature(id_);
      auto comp_id = component_manager_->GetComponentID<ComponentName>();
      if (comp_id.Good()) {
        current_signature.ResetComponent(*comp_id);
        system_manager_->EntitySignatureChanged(id_, current_signature);
      } else {
        err = comp_id.Error();
      }
    }
    return err;
  }

  template<typename ComponentName> [[nodiscard]] Result<ComponentName*> GetComponent() const
  {
    return component_manager_->GetComponent<ComponentName>(id_);
  }

  void Destroy() const
  {
    component_manager_->EntityDestroyed(id_);
    system_manager_->EntityDestroyed(id_);
    entity_manager_->DestroyEntity(id_);
  }

  void MoveEntity(Entity*& other)
  {
    void* mem = malloc(sizeof(Entity));
    memcpy(mem, this, sizeof(Entity));
    other = static_cast<Entity*>(mem);
  }

  bool operator==(const Entity& rhs) const { return rhs.id_ == this->id_; }

private:
  friend class ECSController;
  friend class SystemManager;
  Entity(ISystemManager* system_manager,
    ComponentManager* component_manager,
    EntityManager* entity_manager,
    EntityID entity_id)
    : id_(entity_id), system_manager_(system_manager), component_manager_(component_manager),
      entity_manager_(entity_manager)
  {}
  Entity(EntityID id) : id_(id) {}
  EntityID id_;
  ISystemManager* system_manager_;
  ComponentManager* component_manager_;
  EntityManager* entity_manager_;
};

namespace std {
template<> struct hash<Entity>
{
  uint64_t operator()(const Entity& id) const noexcept { return id.GetID().Get(); }
};
}// namespace std

template<> struct ankerl::unordered_dense::hash<Entity>
{
  using is_avalanching = void;

  [[nodiscard]] auto operator()(Entity const& x) const noexcept -> uint64_t
  {
    return detail::wyhash::hash(x.GetID().Get());
  }
};

#endif// !INCLUDE_ECS_ENTITY_HPP_