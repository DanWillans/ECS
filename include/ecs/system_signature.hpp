#ifndef INCLUDE_ECS_SYSTEM_SIGNATURE_HPP_
#define INCLUDE_ECS_SYSTEM_SIGNATURE_HPP_

#include <bitset>

#include "ecs/component_manager.hpp"
#include "ecs/ecs_constants.hpp"
#include "ids.hpp"

// Wrap a bitset but make it easier for users to use the ComponentID etc.
class SystemSignature
{
public:
  SystemSignature() = default;
  template<typename T> void SetComponent(ComponentID<T> component_id) { bitset_.set(component_id.Get()); }
  template<typename... Ts> void SetComponent() { (bitset_.set(type_index<Ts>::value()), ...); }
  template<typename T> void ResetComponent(ComponentID<T> component_id) { bitset_.reset(component_id.Get()); }
  template<typename... Ts> void ResetComponent() { (bitset_reset(type_index<Ts>::value()), ...); }
  SystemSignature operator&(const SystemSignature& rhs) const { return SystemSignature{ rhs.bitset_ & bitset_ }; }
  bool operator==(const SystemSignature& rhs) const { return rhs.bitset_ == bitset_; }

private:
  explicit SystemSignature(const std::bitset<MAX_COMPONENT_COUNT>& bitset) : bitset_(bitset) {}
  std::bitset<MAX_COMPONENT_COUNT> bitset_;
};

#endif // INCLUDE_ECS_SYSTEM_SIGNATURE_HPP_