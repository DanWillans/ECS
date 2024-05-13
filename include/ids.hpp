#ifndef INCLUDE_ENTITY_H_
#define INCLUDE_ENTITY_H_

#include "ankerl/unordered_dense.h"

#include <cstdint>
#include <functional>

// A generic class that can be used as a unique identifier
// Tag's are used so that different Aliased IDs cannot be compared
template<typename T, typename Tag> class ID
{
public:
  constexpr explicit ID(const T& identifier) : id_(identifier) {}

  constexpr bool operator==(const ID<T, Tag>& other) const { return id_ == other.id_; }

  constexpr bool operator<(const ID<T, Tag>& other) const { return id_ < other.id_; }

  const T& Get() const { return id_; }

public:
  T id_;
};

struct EntityTag
{
};
using EntityID = ID<uint64_t, EntityTag>;

template<typename ComponentName> using ComponentID = ID<size_t, ComponentName>;

template<typename SystemName> using SystemID = ID<size_t, SystemName>;

namespace std {
template<typename T, typename Tag> struct hash<ID<T, Tag>>
{
  T operator()(const ID<T, Tag>& id) const noexcept { return id.Get(); }
};
}// namespace std

template <typename T, typename Tag>
struct ankerl::unordered_dense::hash<ID<T, Tag>> {
    using is_avalanching = void;

    [[nodiscard]] auto operator()(ID<T, Tag> const& x) const noexcept -> T {
        return detail::wyhash::hash(x.Get());
    }
};

#endif