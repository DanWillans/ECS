#include <doctest/doctest.h>

#include "ecs/component_manager.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system_manager.hpp"
#include "ids.hpp"


TEST_CASE("Test system manager")
{
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
  struct System4 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct System5 : public System
  {
    void Update(const float& delta_time) override { std::ignore = delta_time; }
  };
  struct TestComponent1
  {
  };
  struct TestComponent2
  {
  };
  struct TestComponent3
  {
  };
  struct TestComponent4
  {
  };
  struct TestComponent5
  {
  };

  // Create entities
  EntityManager ent_man;
  auto ent_id_1 = ent_man.CreateEntity();
  REQUIRE(ent_id_1.Good());
  auto ent_id_2 = ent_man.CreateEntity();
  REQUIRE(ent_id_2.Good());
  auto ent_id_3 = ent_man.CreateEntity();
  REQUIRE(ent_id_3.Good());
  auto ent_id_4 = ent_man.CreateEntity();
  REQUIRE(ent_id_4.Good());
  auto ent_id_5 = ent_man.CreateEntity();
  REQUIRE(ent_id_5.Good());

  // Create components
  ComponentManager comp_man;
  REQUIRE(comp_man.RegisterComponent<TestComponent1>());
  REQUIRE(comp_man.RegisterComponent<TestComponent2>());
  REQUIRE(comp_man.RegisterComponent<TestComponent3>());
  REQUIRE(comp_man.RegisterComponent<TestComponent4>());
  REQUIRE(comp_man.RegisterComponent<TestComponent5>());

  // Create systems
  SystemManager sys_man(&comp_man, &ent_man);
  SystemSignature signature_1;
  signature_1.SetComponent<TestComponent1, TestComponent2>();
  auto sys_id_1 = sys_man.RegisterSystem<System1>(signature_1);

  SystemSignature signature_2;
  signature_2.SetComponent<TestComponent1, TestComponent2, TestComponent3>();
  auto sys_id_2 = sys_man.RegisterSystem<System2>(signature_2);

  SystemSignature signature_3;
  signature_3.SetComponent<TestComponent1, TestComponent2, TestComponent3>();
  auto sys_id_3 = sys_man.RegisterSystem<System3>(signature_3);

  SystemSignature signature_4;
  signature_4.SetComponent<TestComponent1>();
  auto sys_id_4 = sys_man.RegisterSystem<System4>(signature_4);

  SystemSignature signature_5;
  signature_5.SetComponent<TestComponent1, TestComponent2, TestComponent3, TestComponent4, TestComponent5>();
  auto sys_id_5 = sys_man.RegisterSystem<System5>(signature_5);

  // Inform system manager about an entity signature
  sys_man.EntitySignatureChanged(*ent_id_1, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_2, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_3, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_4, signature_1);
  sys_man.EntitySignatureChanged(*ent_id_5, signature_1);

  // Check Systems have the correct entities
  auto& system_1 = sys_man.GetSystem(sys_id_1);
  REQUIRE_EQ(system_1.GetEntities().size(), 5);
  auto& system_2 = sys_man.GetSystem(sys_id_2);
  REQUIRE_EQ(system_2.GetEntities().size(), 0);
  auto& system_3 = sys_man.GetSystem(sys_id_3);
  REQUIRE_EQ(system_3.GetEntities().size(), 0);
  auto& system_4 = sys_man.GetSystem(sys_id_4);
  REQUIRE_EQ(system_4.GetEntities().size(), 5);
  auto& system_5 = sys_man.GetSystem(sys_id_5);
  REQUIRE_EQ(system_5.GetEntities().size(), 0);

  // Let's change the signature of some entities and recheck the systems
  sys_man.EntitySignatureChanged(*ent_id_1, signature_5);
  sys_man.EntitySignatureChanged(*ent_id_2, signature_4);

  // Check Systems have the correct entities
  REQUIRE_EQ(system_1.GetEntities().size(), 4);
  REQUIRE_EQ(system_2.GetEntities().size(), 1);
  REQUIRE_EQ(system_3.GetEntities().size(), 1);
  REQUIRE_EQ(system_4.GetEntities().size(), 5);
  REQUIRE_EQ(system_5.GetEntities().size(), 1);
}
