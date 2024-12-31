#include "doctest/doctest.h"

#include "ecs/ecs_controller.hpp"

TEST_CASE("Test ECS Controller")
{
  struct TestComponent1
  {
  };
  struct TestComponent2
  {
  };
  struct TestComponent3
  {
  };
  struct System1 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct System2 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct System3 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  ECSController ecs;
  auto entity_1 = ecs.CreateEntity();
  REQUIRE(entity_1);
  auto entity_2 = ecs.CreateEntity();
  REQUIRE(entity_2);
  auto entity_3 = ecs.CreateEntity();
  REQUIRE(entity_3);
  // Make sure all entities are unique
  REQUIRE(*entity_1 != *entity_2);
  REQUIRE(*entity_2 != *entity_3);
  REQUIRE(*entity_1 != *entity_3);

  // Register component types
  REQUIRE(ecs.RegisterComponent<TestComponent1>());
  REQUIRE(ecs.RegisterComponent<TestComponent2>());
  REQUIRE(ecs.RegisterComponent<TestComponent3>());

  // Register systems and their signatures
  SystemSignature signature_1;
  signature_1.SetComponent<TestComponent1, TestComponent2>();
  auto system_id_1 = ecs.RegisterSystem<System1>(signature_1);

  SystemSignature signature_2;
  signature_2.SetComponent<TestComponent1>();
  auto system_id_2 = ecs.RegisterSystem<System2>(signature_2);

  SystemSignature signature_3;
  signature_3.SetComponent<TestComponent1, TestComponent2, TestComponent3>();
  auto system_id_3 = ecs.RegisterSystem<System3>(signature_3);

  // Now we can use Entity objects to add components for that entity
  REQUIRE(entity_1->AddComponent<TestComponent1>());
  REQUIRE(entity_1->AddComponent<TestComponent2>());
  REQUIRE(entity_1->AddComponent<TestComponent3>());

  REQUIRE(entity_2->AddComponent<TestComponent1>());
  REQUIRE(entity_2->AddComponent<TestComponent2>());

  REQUIRE(entity_3->AddComponent<TestComponent1>());
  REQUIRE(entity_3->AddComponent<TestComponent3>());

  // Check systems have the correct entities
  auto& sys_1 = ecs.GetSystem(system_id_1);
  auto& sys_2 = ecs.GetSystem(system_id_2);
  auto& sys_3 = ecs.GetSystem(system_id_3);

  // Check system entity sizes
  REQUIRE_EQ(sys_1.GetEntities().size(), 2);
  REQUIRE_EQ(sys_2.GetEntities().size(), 3);
  REQUIRE_EQ(sys_3.GetEntities().size(), 1);

  // Remove components on entities
  REQUIRE(entity_1->RemoveComponent<TestComponent2>());
  REQUIRE(entity_1->RemoveComponent<TestComponent3>());
  REQUIRE(entity_3->RemoveComponent<TestComponent3>());

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.GetEntities().size(), 1);
  REQUIRE_EQ(sys_2.GetEntities().size(), 3);
  REQUIRE_EQ(sys_3.GetEntities().size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 3);

  // Destroy entity
  entity_1->Destroy();

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.GetEntities().size(), 1);
  REQUIRE_EQ(sys_2.GetEntities().size(), 2);
  REQUIRE_EQ(sys_3.GetEntities().size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 2);

  // Destroy entity
  entity_2->Destroy();

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.GetEntities().size(), 0);
  REQUIRE_EQ(sys_2.GetEntities().size(), 1);
  REQUIRE_EQ(sys_3.GetEntities().size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 1);

  // Destroy entity
  entity_3->Destroy();

  // Check systems have correct entities again
  REQUIRE_EQ(sys_1.GetEntities().size(), 0);
  REQUIRE_EQ(sys_2.GetEntities().size(), 0);
  REQUIRE_EQ(sys_3.GetEntities().size(), 0);

  REQUIRE_EQ(ecs.EntityCount(), 0);
}