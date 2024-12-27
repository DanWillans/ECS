#ifndef INCLUDE_COMPONENT_MANAGER_H_
#define INCLUDE_COMPONENT_MANAGER_H_

#include <array>
#include <cstddef>
#include <memory>
#include <thread>
#include <typeindex>

#include "ankerl/unordered_dense.h"
#include "ecs/component_array.hpp"
#include "ecs/ecs_constants.hpp"
#include "error.hpp"
#include "ids.hpp"
#include "result.hpp"

// template<typename ComponentName>
// using ComponentIterator = std::vector<ComponentName>::iterator;

struct custom_hash_avalanching {
    using is_avalanching = void;

    auto operator()(std::type_index const& x) const noexcept -> uint64_t {
        return ankerl::unordered_dense::detail::wyhash::hash(x.hash_code());
    }
};

class ComponentManager
{
public:
  ComponentManager() = default;
  ComponentManager(const ComponentManager&) = delete;
  ComponentManager& operator=(const ComponentManager&) = delete;

  // Chose to use ComponentID so that we don't have to do any fancy
  // type resolution with typeid to get the correct IComponentArray.
  template<typename ComponentName> ComponentID<ComponentName> RegisterComponent()
  {
    ComponentID<ComponentName> comp_id(component_index_count_++);
    components_map_[std::type_index(typeid(ComponentName))] = std::make_unique<ComponentArray<ComponentName>>(comp_id);
    return comp_id;
  }

  template<typename ComponentName> Error AddComponent(EntityID entity_id, const ComponentName& comp)
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      comp_array->AddComponent(entity_id, comp);
      return Error::OK();
    } else {
      return comp_array.Error();
    }
  }

  template<typename ComponentName> Error RemoveComponent(EntityID entity_id)
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      comp_array->RemoveComponent(entity_id);
      return Error::OK();
    } else {
      return comp_array.Error();
    }
  }

  void EntityDestroyed(EntityID entity_id);

  template<typename ComponentName> Result<ComponentName*> GetComponent(EntityID entity_id)
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return &comp_array->GetComponent(entity_id);
    } else {
      return comp_array.Error();
    }
  }

  template<typename ComponentName> Result<size_t> GetComponentCount()
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return comp_array->Size();
    } else {
      return comp_array.Error();
    }
  }

  template<typename ComponentName> Result<ComponentID<ComponentName>> GetComponentID() {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return comp_array->GetID();
    } else {
      return comp_array.Error();
    }
  }

private:
  template<typename ComponentName> Result<ComponentArray<ComponentName>*> GetComponentArray()
  {
    auto comp_it = components_map_.find(std::type_index(typeid(ComponentName)));
    if (comp_it == components_map_.end()) {
      return Error{ "Component hasn't been registered" };
    }
    return static_cast<ComponentArray<ComponentName>*>(comp_it->second.get());
  }
  // A count to store how "full" or components_ array is.
  size_t component_index_count_{ 0 };

  // std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> components_map_;
  ankerl::unordered_dense::map<std::type_index, std::unique_ptr<IComponentArray>, custom_hash_avalanching> components_map_;
};

#endif