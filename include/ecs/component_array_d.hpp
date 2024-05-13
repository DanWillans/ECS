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
    if (free_slot_amount_) {
      auto back_slot_index = components_.size() - (free_slot_amount_);
      entity_index_map_[entity_id] = back_slot_index;
      auto& array_component = components_[back_slot_index];
      array_component.component = component;
      array_component.entity_id = entity_id;
      array_component.dirty = false;
      free_slot_amount_--;
    } else {
      components_.emplace_back(component, entity_id);
      entity_index_map_[entity_id] = components_.size() - 1;
    }
  }

  void RemoveComponent(EntityID entity_id) override
  {
    auto entity_it = entity_index_map_.find(entity_id);
    if (entity_it != entity_index_map_.end()) {
      // Swap the last entry in the vector with this removed slot to keep the vector packed.
      // 0 1 2 3 4
      // 0 | 2 3 4 -> 0 4 2 3 |
      // 0 | | 4 5 -> 0 4 3 | |
      // 
      size_t free_slot_index{ 0 };
      if (free_slot_amount_ > 0) {
        free_slot_index = components_.size() - 1 - (free_slot_amount_);
      } else {
        free_slot_index = components_.size() - 1;
      }
      // Move the back component to the removed slot
      auto& back_component = components_[free_slot_index];
      components_[entity_it->second] = back_component;
      entity_index_map_[back_component.entity_id] = entity_it->second;
      // // Catch edge case adjusting initial index to avoid the above copy.
      // if(free_slot_index == 0){
      //   back_component.dirty = true;
      //   return;
      // }
      back_component.dirty = true;
      free_slot_amount_++;
      entity_index_map_.erase(entity_id);
      // // Invalidate the component in the vector.
      // components_[entity_it->second].dirty = true;
      // free_slots_.push(entity_it->second);
      // // Remove the component from the entity map.
      // entity_index_map_.erase(entity_id);
    }
  }

  [[nodiscard]] size_t Size() const { return components_.size() - free_slot_amount_; }

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

  // Track amount of free slots
  size_t free_slot_amount_{ 0 };

  // ComponentID that this array represents
  ComponentID<T> id_;
};

#endif