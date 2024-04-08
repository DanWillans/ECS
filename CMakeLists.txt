cmake_minimum_required(VERSION 3.0.0)
project(ECS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

include(CTest)
enable_testing()

add_library(ECS src/entity_manager.cpp)
