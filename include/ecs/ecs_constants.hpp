#ifndef INCLUDE_ECS_CONSTANTS_H_
#define INCLUDE_ECS_CONSTANTS_H_

#include <cstdint>

// int32_max assigned to int64_t on purpose. Leaving room incase we need to expand. Do we really need int64_t max
// entities ever?!
constexpr int64_t MAX_ENTITY_COUNT = 1000000;
constexpr int64_t MAX_COMPONENT_COUNT = 1024;

#endif