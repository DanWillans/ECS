#ifndef INCLUDE_ECS_SYSTEM_HPP_
#define INCLUDE_ECS_SYSTEM_HPP_

#include "ecs/component_manager.hpp"
#include "ecs/entity.hpp"
#include "ids.hpp"
#include "ecs/system_signature.hpp"
#include "ecs/system_manager_interface.hpp"

#include "ankerl/unordered_dense.h"


using EntitySet = ankerl::unordered_dense::set<Entity>;

// A system at the minute is a simple class that tracks a std::unordered_set of
// EntityIDs and a SystemSignature that represents the types of components that the
// system is interested in.
// It also includes some helper functions to the System.
// The set of EntityIDs are entities that are guaranteed to have the set of components
// that this system is interested in.
// It is recommended to use an Initialise() function instead of doing logic in the constructor. This is because
// additional configuration to the system is done after construction.
// NOLINTNEXTLINE
class System
{
public:
  static constexpr uint8_t MAXIMUM_ENTITY_SETS{ 20 };
  virtual ~System() = default;
  // A handle to the system manager
  ISystemManager* system_manager{ nullptr };
  // A set of entities that this system is interested in based on the main SystemSignature
  // ankerl::unordered_dense::set<Entity> entities;
  // The signature of components this system cares about.
  SystemSignature signature;
  // A handle to the component manager
  // Do not use until the system has been registered.
  ComponentManager* component_manager{ nullptr };

  /**
   * @brief Register an additional SystemSignature to track in the ECS. Must be called prior to any entities being
   * created. This function currently doesn't support adding existing entities to the set only entities made afterward
   *
   * @param signature The system signature of the components this system is interested in.
   * @return ankerl::unordered_dense::set<Entity>* A handle to the entity set to iterate over.
   */
  [[nodiscard]] Result<EntitySet*> RegisterSystemSignature(const SystemSignature& signature);

  /**
   * @brief This should be called by the game. Internally it will call the user implemented Update() function.
   *
   * @param delta_time The time between previous frame and current.
   */
  void UpdateSystem(const float& delta_time);

  // Get a specific ComponentID vector.
  // You may want to access entities that don't reflect your system signature.
  // You must not use this function until after the system has been Registered with the system manager.
  template<typename ComponentName>
  std::vector<ComponentWrapper<ComponentName>>& GetComponentVector(ComponentID<ComponentName> identifier)
  {
    return component_manager->GetComponentVector(identifier);
  }

  /**
   * @brief Get the main entities associated to the system signature initially registered.
   *
   * @return ankerl::unordered_dense::set<Entity>& entity set
   */
  ankerl::unordered_dense::set<Entity>& GetEntities()
  {
    assert(!entity_sets.empty());
    return entity_sets[0].second;
  }

protected:
  void MarkEntityForDeletion(const Entity& entity);

private:
  // Let SystemManager access private members to set.
  friend class SystemManager;

  /**
   * @brief This is for the developer to implement. This is where entities should be marked as deleted if they should be
   * removed.
   *
   * @param delta_time The time between previous frame and current.
   */
  virtual void Update([[maybe_unused]] const float& delta_time) = 0;

  // A vector of additional entity sets registered via RegisterSystemSignature. This shouldn't be accessed directly, the
  // handle should be kept from RegisterSystemSignature().
  // Right now you can only have 20 maximum entity sets so that handles/pointers don't become invalidated.
  std::array<std::pair<SystemSignature, EntitySet>, MAXIMUM_ENTITY_SETS> entity_sets;

// We don't expose std::vector in the API so just disable the warning here.
#pragma warning(disable : 4251)
  ankerl::unordered_dense::set<Entity> entities_to_delete_;

  uint8_t entity_set_count_{ 0 };
};

#endif // !INCLUDE_ECS_SYSTEM_HPP_