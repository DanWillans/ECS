#ifndef INCLUDE_ECS_COMPONENT_ARRAY_H_
#define INCLUDE_ECS_COMPONENT_ARRAY_H_

#include <queue>
#include <unordered_map>
#include <vector>


#include "ids.hpp"

template<typename T> struct ComponentWrapper
{
  ComponentWrapper(T component, EntityID entity_id) : component(component), entity_id(entity_id) {}
  bool dirty{ false };
  T component;
  EntityID entity_id;
};

class IComponentArray
{
public:
  virtual void RemoveComponent(EntityID) = 0;
  virtual ~IComponentArray() = default;
};

template<typename T> class ComponentArray : public IComponentArray
{
public:
  explicit ComponentArray(ComponentID<T> id) : id_(id) {}
  ComponentArray(ComponentArray&&) = default;
  ComponentArray(const ComponentArray&) = default;
  ComponentArray& operator=(ComponentArray&&) = default;
  ComponentArray& operator=(const ComponentArray&) = default;
  void AddComponent(EntityID entity_id, const T& component)
  {
    // Check if there are any free slots in the components_ vector we can use
    // first.
    if (!free_slots_.empty()) {
      // There is a free slot, let's use it instead of allocating more space
      // into the components_ vector.
      const auto& index = free_slots_.front();
      auto& array_component = components_[index];
      // Update the entity position map before we overwrite the component
      entity_index_map_[entity_id] = index;
      // Update the component
      array_component.component = component;
      array_component.entity_id = entity_id;
      array_component.dirty = false;
      // Pop now we're not using the references from the front of the queue
      free_slots_.pop();
    } else {
      components_.emplace_back(component, entity_id);
      entity_index_map_[entity_id] = components_.size() - 1;
    }
  }

  void RemoveComponent(EntityID entity_id) override
  {
    auto entity_it = entity_index_map_.find(entity_id);
    if (entity_it != entity_index_map_.end()) {
      // Invalidate the component in the vector.
      components_[entity_it->second].dirty = true;
      free_slots_.push(entity_it->second);
      // Remove the component from the entity map.
      entity_index_map_.erase(entity_id);
    } else {
      printf("EntityID: %I64d - doesn't exist for component id: %zd", entity_id.Get(), id_.Get());
    }
  }

  [[nodiscard]] size_t Size() const { return components_.size() - free_slots_.size(); }

  T& GetComponent(EntityID entity_id) { return components_[entity_index_map_[entity_id]].component; }

  ~ComponentArray<T>() override = default;

private:
  // This unordered_map will map entities to component indexes in the
  // components_ vector. This map should only be used for inserting and deleting
  // entities from components.
  std::unordered_map<EntityID, size_t> entity_index_map_;

  // This vector will hold our component entries. Not all components entries are
  // valid. When iterating over the components it is necessary to check the
  // dirty flag in the ComponentWrapper.
  std::vector<ComponentWrapper<T>> components_;

  // Track free slots in the components_ vector.
  std::queue<size_t> free_slots_;

  // ComponentID that this array represents
  ComponentID<T> id_;
};

#endif