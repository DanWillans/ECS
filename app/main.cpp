#include "ecs/component_manager.hpp"
#include "ecs/entity_manager.hpp"
#include "ecs/system_manager.hpp"
#include "ids.hpp"

struct Animals
{
  int dog{ 1 };
  int cat{ 2 };
  int fish{ 3 };
};

struct AnimalFood
{
  float dog_food{ 1.3f };
  float cat_food{ 10398.38f };
  float fish_food{ 99.0 };
};

class MyAnimalSystem : public System
{
public:
  MyAnimalSystem(ComponentID<Animals>& animal_comp_id, ComponentID<AnimalFood>& animal_food_id)
    : animal_comp_id_(animal_comp_id), animal_food_id_(animal_food_id)
  {}

  void Iterate()
  {
    for (const auto& entity : entities) {
      auto animal_component = component_manager->GetComponent(entity, animal_comp_id_);
      auto animal_food_component = component_manager->GetComponent(entity, animal_food_id_);
      printf("---------\n");
      printf("Dog: %d\n", animal_component.dog);
      printf("Cat: %d\n", animal_component.cat);
      printf("Fish: %d\n", animal_component.fish);
      printf("Dog Food: %f\n", animal_food_component.dog_food);
      printf("Cat Food: %f\n", animal_food_component.cat_food);
      printf("Fish Food: %f\n", animal_food_component.fish_food);
      printf("---------\n");
    }
  }

private:
  ComponentID<Animals> animal_comp_id_;
  ComponentID<AnimalFood> animal_food_id_;
};

int main(int argc, const char** argv)
{

  // Create our entities
  EntityManager entity_manager;
  auto entity_id_0 = entity_manager.CreateEntity();
  auto entity_id_1 = entity_manager.CreateEntity();
  auto entity_id_2 = entity_manager.CreateEntity();

  // Register our components
  ComponentManager component_manager;

  // Register our components
  auto animals_comp_id = component_manager.RegisterComponent<Animals>();
  auto animal_food_comp_id = component_manager.RegisterComponent<AnimalFood>();

  // Create our system manager
  SystemManager system_manager(&component_manager);

  // Create signature for my animal system
  SystemSignature animal_system_signature;
  animal_system_signature.SetComponent(animals_comp_id);
  animal_system_signature.SetComponent(animal_food_comp_id);

  // Register system
  auto animal_sys_id =
    system_manager.RegisterSystem<MyAnimalSystem>(animal_system_signature, animals_comp_id, animal_food_comp_id);

  // Now let's start creating components to add to our entities
  Animals animal_set_1;
  animal_set_1.cat = 3;
  animal_set_1.dog = 4;
  animal_set_1.fish = 5;
  // Add component to entity_id_0
  auto res = component_manager.AddComponent(*entity_id_0, animals_comp_id, animal_set_1);

  // Update the system manager to inform systems that this entities components have changed. They can then decide
  // whether to track this entity or not.
  SystemSignature animal_signature;
  animal_signature.SetComponent(animals_comp_id);
  system_manager.EntitySignatureChanged(*entity_id_0, animal_signature);

  // Make another component for our entity
  Animals animal_set_2;
  res = component_manager.AddComponent(*entity_id_1, animals_comp_id, animal_set_2);
  // Update the system manager to inform systems that this entity changed.
  system_manager.EntitySignatureChanged(*entity_id_1, animal_signature);

  Animals animal_set_3;
  animal_set_3.cat = 99;
  animal_set_3.dog = 199;
  animal_set_3.fish = 399;
  res = component_manager.AddComponent(*entity_id_2, animals_comp_id, animal_set_3);
  AnimalFood animal_food_3{
    .cat_food{3008.0f},
  };
  res = component_manager.AddComponent(*entity_id_2, animal_food_comp_id, animal_food_3);
  // Update the system manager to inform systems that this entity changed.
  system_manager.EntitySignatureChanged(*entity_id_2, animal_system_signature);

  // Grab the system and iterate
  auto animal_system = system_manager.GetSystem(animal_sys_id);
  animal_system.Iterate();

  return 0;
}