cmake_minimum_required(VERSION 3.0.0)
project(ECS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include(CTest)
enable_testing()

add_subdirectory(vendor/unordered_dense)

add_library(ECS src/entity_manager.cpp src/system_manager.cpp src/component_manager.cpp)
target_link_libraries(ECS unordered_dense::unordered_dense)

include_directories(vendor)

add_subdirectory(app)

add_subdirectory(test)
