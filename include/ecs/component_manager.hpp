#ifndef INCLUDE_COMPONENT_MANAGER_H_
#define INCLUDE_COMPONENT_MANAGER_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "ankerl/unordered_dense.h"
#include "ecs/component_array.hpp"
#include "ecs/ecs_constants.hpp"
#include "error.hpp"
#include "ids.hpp"
#include "result.hpp"

namespace internal {
struct type_index final
{
  [[nodiscard]] static uint32_t next() noexcept
  {
    static uint32_t value{};
    return value++;
  }
};
}// namespace internal

template<typename Type, typename = void> struct type_index final
{
  static uint32_t value() noexcept
  {
    static const uint32_t value = internal::type_index::next();
    return value;
  }
  constexpr operator uint32_t() const noexcept { return value(); }
};

class ComponentManager
{
public:
  ComponentManager() = default;
  ComponentManager(const ComponentManager&) = delete;
  ComponentManager& operator=(const ComponentManager&) = delete;

  // Chose to use ComponentID so that we don't have to do any fancy
  // type resolution with typeid to get the correct IComponentArray.
  template<typename ComponentName> Error RegisterComponent()
  {
    Error err = Error::OK();
    int id = type_index<ComponentName>::value();
    if (id < MAX_COMPONENT_COUNT) {
      ComponentID<ComponentName> comp_id(id);
      components_.emplace_back(std::make_unique<ComponentArray<ComponentName>>(comp_id));
      component_index_count_ = id;
    } else {
      err = Error{ "Maximum components registered exceeded" };
    }
    return err;
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

  template<typename ComponentName> Result<ComponentID<ComponentName>> GetComponentID()
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return comp_array->GetID();
    } else {
      return comp_array.Error();
    }
  }

  template<typename ComponentName>
  Result<std::vector<ComponentWrapper<ComponentName>>>& GetComponentVector(ComponentID<ComponentName> component_id)
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return comp_array->GetComponentVector();
    } else {
      return comp_array.Error();
    }
  }

  template<typename ComponentName> bool HasComponent(EntityID identifier, ComponentID<ComponentName> component_id)
  {
    auto comp_array = GetComponentArray<ComponentName>();
    if (comp_array.Good()) {
      return comp_array->HasComponent();
    } else {
      return comp_array.Error();
    }
  }

private:
  template<typename ComponentName> Result<ComponentArray<ComponentName>*> GetComponentArray()
  {
    auto id = type_index<ComponentName>::value();
    if (id > component_index_count_) {
      return Error{ "Component hasn't been registered" };
    }
    return static_cast<ComponentArray<ComponentName>*>(components_[id].get());
  }
  // A count to store how "full" or components_ array is.
  size_t component_index_count_{ 0 };

  std::vector<std::unique_ptr<IComponentArray>> components_;
};

#endif