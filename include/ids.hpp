#ifndef INCLUDE_ENTITY_H_
#define INCLUDE_ENTITY_H_

#include <cstdint>
#include <functional>

// A generic class that can be used as a unique identifier
// Tag's are used so that different Aliased IDs cannot be compared
template <typename T, typename Tag> class ID {
public:
  constexpr explicit ID(const T &identifier) : id_(identifier) {}

  constexpr bool operator==(const ID<T, Tag> &other) const {
    return id_ == other.Get();
  }

  const T &Get() const { return id_; }

private:
  T id_;
};

struct EntityTag {};
using EntityID = ID<uint64_t, EntityTag>;

template <typename ComponentName>
using ComponentID = ID<uint64_t, ComponentName>;

namespace std {
template<typename T, typename Tag> struct hash<ID<T, Tag>>
{
  size_t operator()(const ID<T, Tag>& id) const noexcept { return id.Get(); }
};
}// namespace std

#endif