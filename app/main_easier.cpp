#include "ecs/ecs_controller.hpp"
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
    for(auto& [_, entity] : entities){
      auto& animal_component = entity.GetComponent(animal_comp_id_);
      auto& animal_food_component = entity.GetComponent(animal_food_id_);
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
  ECSController ecs_controller;
  auto entity_id_0 = ecs_controller.CreateEntity();
  auto entity_id_1 = ecs_controller.CreateEntity();
  auto entity_id_2 = ecs_controller.CreateEntity();

  // Register components
  auto animal_component_id = ecs_controller.RegisterComponent<Animals>();
  auto animal_food_component_id = ecs_controller.RegisterComponent<AnimalFood>();

  // Create signature for our system
  SystemSignature animal_system_signature;
  animal_system_signature.SetComponent(animal_component_id);
  animal_system_signature.SetComponent(animal_food_component_id);

  // Register system
  auto animal_system_id = ecs_controller.RegisterSystem<MyAnimalSystem>(
    animal_system_signature, animal_component_id, animal_food_component_id);

  Animals animal_set_1;
  animal_set_1.cat = 2;
  animal_set_1.dog = 3;
  animal_set_1.fish = 4;
  Error err = entity_id_0->AddComponent(animal_component_id, animal_set_1);
  
  AnimalFood animal_food_0;
  animal_food_0.cat_food = { 100.0f };
  animal_food_0.dog_food = { 103838100.0f };
  animal_food_0.fish_food = { -13.3948f };
  err = entity_id_0->AddComponent(animal_food_component_id, animal_food_0);

  Animals animal_set_2;
  animal_set_2.cat = 3;
  animal_set_2.dog = 4;
  animal_set_2.fish = 5;
  err = entity_id_1->AddComponent(animal_component_id, animal_set_2);

  AnimalFood animal_food_1;
  animal_food_1.cat_food = { 100.0f };
  animal_food_1.dog_food = { 103838100.0f };
  animal_food_1.fish_food = { -13.3948f };
  err = entity_id_1->AddComponent(animal_food_component_id, animal_food_1);

  Animals animal_set_3;
  animal_set_3.cat = 4;
  animal_set_3.dog = 5;
  animal_set_3.fish = 6;
  err = entity_id_2->AddComponent(animal_component_id, animal_set_3);

  AnimalFood animal_food;
  animal_food.cat_food = { 100.0f };
  animal_food.dog_food = { 103838100.0f };
  animal_food.fish_food = { -13.3948f };
  err = entity_id_2->AddComponent(animal_food_component_id, animal_food);

  // Get system and update it
  auto& animal_system = ecs_controller.GetSystem(animal_system_id);
  printf("\nITERATE\n");
  animal_system.Iterate();
  printf("-----------------------------------\n");

  err = entity_id_0->RemoveComponent(animal_food_component_id);

  printf("\nITERATE\n");
  animal_system.Iterate();

  err = entity_id_1->RemoveComponent(animal_food_component_id);
  printf("\nITERATE\n");
  animal_system.Iterate();

  err = entity_id_2->RemoveComponent(animal_food_component_id);
  printf("\nITERATE\n");
  animal_system.Iterate();

  return 0;
}