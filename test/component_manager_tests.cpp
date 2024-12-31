#include <doctest/doctest.h>

#include <vector>

#include "ids.hpp"
#include "ecs/component_array.hpp"
#include "ecs/component_manager.hpp"

namespace {
struct TestComponent1
{
  int a;
};
struct TestComponent2
{
  int a;
};
struct TestComponent3
{
  int a;
};
struct TestComponent4
{
  int a;
};
}// namespace

TEST_CASE("Test ComponentArray")
{
  ComponentArray<TestComponent1> comp_array(ComponentID<TestComponent1>(0));
  EntityID id_0{ 0 };
  EntityID id_1{ 1 };
  EntityID id_2{ 2 };
  EntityID id_3{ 3 };
  EntityID id_4{ 4 };
  comp_array.AddComponent(id_0, { 0 });
  comp_array.AddComponent(id_1, { 1 });
  comp_array.AddComponent(id_2, { 2 });
  comp_array.AddComponent(id_3, { 3 });
  comp_array.AddComponent(id_4, { 4 });
  auto ValidCheckAndNext = [&](EntityID id, auto val) {
    REQUIRE(comp_array.GetComponent(id).a ==  val);
  };
  REQUIRE_EQ(comp_array.Size(), 5);
  // Check all components exist
  ValidCheckAndNext(id_0, 0);
  ValidCheckAndNext(id_1, 1);
  ValidCheckAndNext(id_2, 2);
  ValidCheckAndNext(id_3, 3);
  ValidCheckAndNext(id_4, 4);
  // Remove components
  comp_array.RemoveComponent(id_1);
  comp_array.RemoveComponent(id_2);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Check left over components exist
  ValidCheckAndNext(id_0, 0);
  ValidCheckAndNext(id_4, 4);
  ValidCheckAndNext(id_3, 3);
  REQUIRE_EQ(comp_array.Size(), 3);
  // Add components back in a different order
  comp_array.AddComponent(id_2, { 2 });
  REQUIRE_EQ(comp_array.Size(), 4);
  comp_array.AddComponent(id_1, { 1 });
  REQUIRE_EQ(comp_array.Size(), 5);
  // Now get a new iterator and check that the order is 0,2,1,3,4
  // This order is because we removed 1, 2 and then put back 2, 1 and the free slots
  // would have been used up first before allocating new ones
  ValidCheckAndNext(id_0, 0);
  ValidCheckAndNext(id_4, 4);
  ValidCheckAndNext(id_3, 3);
  ValidCheckAndNext(id_2, 2);
  ValidCheckAndNext(id_1, 1);
  EntityID id_5{ 5 };
  comp_array.AddComponent(id_5, { 5 });
  REQUIRE_EQ(comp_array.Size(), 6);
  ValidCheckAndNext(id_0, 0);
  ValidCheckAndNext(id_4, 4);
  ValidCheckAndNext(id_3, 3);
  ValidCheckAndNext(id_2, 2);
  ValidCheckAndNext(id_1, 1);
  ValidCheckAndNext(id_5, 5);
}

template<typename T>
void ValidCheckAndNextFree(ComponentManager& comp_manager, EntityID entity_id, int val){
  auto comp = comp_manager.GetComponent<T>(entity_id);
  REQUIRE(comp.Good());
  REQUIRE(comp->a == val);
}

TEST_CASE("Test ComponentManager")
{
  ComponentManager comp_manager;
  EntityID id_0{ 0 };
  EntityID id_1{ 1 };
  EntityID id_2{ 2 };
  EntityID id_3{ 3 };
  EntityID id_4{ 4 };
  // Register all our components first
  REQUIRE(comp_manager.RegisterComponent<TestComponent1>());
  REQUIRE(comp_manager.RegisterComponent<TestComponent2>());
  REQUIRE(comp_manager.RegisterComponent<TestComponent3>());
  REQUIRE(comp_manager.RegisterComponent<TestComponent4>());
  // Register components to entities
  auto RegEntityAndCheck = [&]<typename T>(EntityID id, T comp) {
    auto err = comp_manager.AddComponent(id, comp);
    REQUIRE(err.Good());
  };
  // Register entity_0 components
  RegEntityAndCheck(id_0, TestComponent1{ 1 });
  RegEntityAndCheck(id_0, TestComponent2{ 2 });
  RegEntityAndCheck(id_0, TestComponent3{ 3 });
  RegEntityAndCheck(id_0, TestComponent4{ 4 });
  // Register entity 1 components
  RegEntityAndCheck(id_1, TestComponent1{ 5 });
  RegEntityAndCheck(id_1, TestComponent2{ 6 });
  // Register entity 2 components
  RegEntityAndCheck(id_2, TestComponent3{ 7 });
  RegEntityAndCheck(id_2, TestComponent4{ 8 });
  // Register entity 3 components
  RegEntityAndCheck(id_3, TestComponent1{ 9 });
  RegEntityAndCheck(id_3, TestComponent4{ 10 });
  // Register entity 4 components
  RegEntityAndCheck(id_4, TestComponent2{ 11 });
  RegEntityAndCheck(id_4, TestComponent3{ 12 });

  auto ValidCheckAndNext = [&]<typename T>(EntityID entity_id, int val) {
    auto comp = comp_manager.GetComponent<T>(entity_id);
    REQUIRE(comp.Good());
    REQUIRE(comp->a == val);
  };
  // Check all the components that we registered exist
  ValidCheckAndNextFree<TestComponent1>(comp_manager, id_0, 1);
  ValidCheckAndNextFree<TestComponent1>(comp_manager, id_1, 5);
  ValidCheckAndNextFree<TestComponent1>(comp_manager, id_3, 9);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent1>() == 3);

  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_0, 2);
  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_1, 6);
  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_4, 11);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent2>() == 3);

  ValidCheckAndNextFree<TestComponent3>(comp_manager, id_0, 3);
  ValidCheckAndNextFree<TestComponent3>(comp_manager, id_2, 7);
  ValidCheckAndNextFree<TestComponent3>(comp_manager, id_4, 12);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent3>() == 3);

  ValidCheckAndNextFree<TestComponent4>(comp_manager, id_0, 4);
  ValidCheckAndNextFree<TestComponent4>(comp_manager, id_2, 8);
  ValidCheckAndNextFree<TestComponent4>(comp_manager, id_3, 10);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent4>() == 3);

  // Now inform the component manager that the entity 0 has been destroyed.
  // All of it's component entries should have disappeared.
  comp_manager.EntityDestroyed(id_0);

  ValidCheckAndNextFree<TestComponent1>(comp_manager, id_1, 5);
  ValidCheckAndNextFree<TestComponent1>(comp_manager, id_3, 9);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent1>() == 2);

  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_1, 6);
  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_4, 11);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent2>() == 2);

  ValidCheckAndNextFree<TestComponent3>(comp_manager, id_2, 7);
  ValidCheckAndNextFree<TestComponent3>(comp_manager, id_4, 12);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent3>() == 2);

  ValidCheckAndNextFree<TestComponent4>(comp_manager, id_2, 8);
  ValidCheckAndNextFree<TestComponent4>(comp_manager, id_3, 10);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent4>() == 2);

  // Now remove just entity 1 TestComponent2
  comp_manager.RemoveComponent<TestComponent2>(id_1);

  ValidCheckAndNextFree<TestComponent2>(comp_manager, id_4, 11);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent2>() == 1);

  // And the same with entity 4
  comp_manager.RemoveComponent<TestComponent2>(id_4);
  REQUIRE(*comp_manager.GetComponentCount<TestComponent2>() == 0);
}