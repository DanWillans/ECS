#include "ecs/component_manager.hpp"
#include "ecs/ecs_controller.hpp"
#include "entt/entt.hpp"
#include "ids.hpp"

#include "Windows.h"
#include <iostream>
#include <profileapi.h>
#include <random>
#include <winnt.h>

// This application consists of 5 performance tests/comparisons
// Each test will compare the ECS in this repo, a vector of Malloc'd entity structs and EnTT (a very popular and high
// performing Entity Component System).
//
// ----- Tests -----
// 1. Iterating over N number of entities with 1 specific component.
// 2. Delete approximately 60% of the entities and then iterate of N number of entities with 1 specific component.
// 3. Iterate over N number of entities and get 2 components from that entity.
// 4. Iterate over N number of entities and get 3 components from that entity.
// 5. Iterate over N number of entities and get 4 components from that entity.

// There are plenty more tests that could be be done:
//  - Insertion time
//  - Deletion time
//  - Entity destroy time
//  - ......
//  - Fragmentation tests
// We're not trying to create a world class ECS here. We're purely trying to have fun and write our own to see where the
// downfalls are and most importantly understand how to improve performance.

namespace {

std::random_device rd;// Only used once to initialise (seed) engine
std::mt19937 rng(rd());// Random-number engine used (Mersenne-Twister in this case)

/**
 * @brief Randomly roll a number between min and max
 *
 * @param min Minimum number that can be rolled
 * @param max Maximum number that can be rolled
 * @return int The randomly rolled number
 */
int roll(int min, int max)
{
  std::uniform_int_distribution<int> uni(min, max);// Guaranteed unbiased
  return uni(rng);
}

}// namespace

/**
 * @brief A timer class to capture time using the performance counter API.
 *
 */
class Timer
{
public:
  Timer(const std::string& name) : timer_name_(name) {}
  void Start()
  {
    QueryPerformanceFrequency(&frequency_);
    QueryPerformanceCounter(&start_time_);
  }

  void CaptureTimePoint(bool print = false)
  {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    LARGE_INTEGER ElapsedMicroseconds;
    ElapsedMicroseconds.QuadPart = now.QuadPart - start_time_.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= frequency_.QuadPart;
    if (print) {
      std::cout << "Time Taken: " << ElapsedMicroseconds.QuadPart << "\n";
    }
    time_point_vec.push_back(ElapsedMicroseconds.QuadPart);
  }

  void PrintAverageTime()
  {
    long long sum = 0;
    for (const auto& time : time_point_vec) {
      sum += time;
    }
    std::cout << timer_name_ << " - Average Time Taken: " << sum / time_point_vec.size() << "\n";
  }

private:
  std::string timer_name_;
  LARGE_INTEGER start_time_;
  LARGE_INTEGER frequency_;
  std::vector<long long> time_point_vec;
};

// Alias for float matrix
using mat4 = float[4][4];

// Animal component
struct Animals
{
  mat4 dog{};
  mat4 cat{};
  mat4 fish{};
};

// AnimalFood component
struct AnimalFood
{
  float dog_food{ 1.3f };
  float cat_food{ 10398.38f };
  float fish_food{ 99.0 };
};

// AnimalHabitat Component
struct AnimalHabitat
{
  float habitat{ 0 };
};

// AnimalHairStyle Component
struct AnimalHairStyle
{
  float bald{ 0 };
  float curly{ 0 };
  float mohawk{ 1 };
};

// An entity struct to act as an alternative to Entities. This is used to compare malloc'd entities/structs vs ECS.
// This is to show that using this pattern is perfectly fine if you don't need thousands of entities and don't want to
// use the ECS composition pattern and code structure.
struct AnimalCollectorEntity
{
  EntityID id;
  std::optional<Animals> animal_component;
  std::optional<AnimalFood> animal_food_component;
  std::optional<AnimalHabitat> animal_habitat_component;
  std::optional<AnimalHairStyle> animal_hair_style_component;
};

/**
 * @brief The system to be used that's registered with our ECS.
 *
 */
class MyAnimalSystem : public System
{
public:
  MyAnimalSystem(ComponentID<Animals>& animal_comp_id,
    ComponentID<AnimalFood>& animal_food_id,
    ComponentID<AnimalHabitat>& animal_habitat_comp_id,
    ComponentID<AnimalHairStyle>& animal_hair_style_comp_id)
    : animal_comp_id_(animal_comp_id), animal_food_comp_id_(animal_food_id),
      animal_habitat_comp_id_(animal_habitat_comp_id), animal_hair_style_comp_id_(animal_hair_style_comp_id)
  {}

  void Iterate4Components()
  {
    for (auto& entity : entities) {
      auto animal_component = entity.GetComponent<Animals>();
      auto animal_food_component = entity.GetComponent<AnimalFood>();
      auto animal_hair_component = entity.GetComponent<AnimalHairStyle>();
      auto animal_habitat_component = entity.GetComponent<AnimalHabitat>();
      animal_component->cat[0][1] = 1.0;
      animal_component->dog[2][2] = 1.0;
      animal_component->fish[3][3] = animal_component->dog[2][2] + 2.0f;
      animal_food_component->cat_food = 1.0;
      animal_food_component->dog_food = 2.0;
      animal_food_component->fish_food = 3.0;
      animal_hair_component->bald = 1.0;
      animal_hair_component->curly = 2.0;
      animal_hair_component->mohawk = 3.0;
      animal_habitat_component->habitat = 540.0;
    }
  }

  void Iterate3Components()
  {
    for (auto& entity : entities) {
      auto animal_component = entity.GetComponent<Animals>();
      auto animal_food_component = entity.GetComponent<AnimalFood>();
      auto animal_hair_component = entity.GetComponent<AnimalHairStyle>();
      animal_component->cat[0][1] = 1.0;
      animal_component->dog[2][2] = 1.0;
      animal_component->fish[3][3] = animal_component->dog[2][2] + 2.0f;
      animal_food_component->cat_food = 1.0;
      animal_food_component->dog_food = 2.0;
      animal_food_component->fish_food = 3.0;
      animal_hair_component->bald = 1.0;
      animal_hair_component->curly = 2.0;
      animal_hair_component->mohawk = 3.0;
    }
  }

  void Iterate2Components()
  {
    for (auto& entity : entities) {
      auto animal_component = entity.GetComponent<Animals>();
      auto animal_food_component = entity.GetComponent<AnimalFood>();
      animal_component->cat[0][1] = 1.0;
      animal_component->dog[2][2] = 1.0;
      animal_component->fish[3][3] = animal_component->dog[2][2] + 2.0f;
      animal_food_component->cat_food = 1.0;
      animal_food_component->dog_food = 2.0;
      animal_food_component->fish_food = 3.0;
    }
  }

  void Iterate()
  {
    for (auto& entity : entities) {
      auto animal_component = entity.GetComponent<Animals>();
      animal_component->cat[0][1] = 1.0;
      animal_component->dog[2][2] = 1.0;
      animal_component->fish[3][3] = animal_component->dog[2][2] + 2.0f;
    }
  }

private:
  ComponentID<Animals> animal_comp_id_;
  ComponentID<AnimalFood> animal_food_comp_id_;
  ComponentID<AnimalHabitat> animal_habitat_comp_id_;
  ComponentID<AnimalHairStyle> animal_hair_style_comp_id_;
};

int main(int argc, const char** argv)
{
  // Instantiate our ECS
  ECSController ecs_controller;

  // Register components
  auto animal_component_id = ecs_controller.RegisterComponent<Animals>();
  auto animal_food_component_id = ecs_controller.RegisterComponent<AnimalFood>();
  auto animal_habitat_component_id = ecs_controller.RegisterComponent<AnimalHabitat>();
  auto animal_hair_style_comp_id = ecs_controller.RegisterComponent<AnimalHairStyle>();

  // Create signature for our system
  SystemSignature animal_system_signature;
  animal_system_signature.SetComponent(animal_component_id);
  animal_system_signature.SetComponent(animal_food_component_id);
  animal_system_signature.SetComponent(animal_habitat_component_id);
  animal_system_signature.SetComponent(animal_hair_style_comp_id);

  // Register system
  auto animal_system_id = ecs_controller.RegisterSystem<MyAnimalSystem>(animal_system_signature,
    animal_component_id,
    animal_food_component_id,
    animal_habitat_component_id,
    animal_hair_style_comp_id);

  // ---- Test Constants ----
  // Create 10000 entities
  constexpr int entity_count = 10000;
  constexpr int iteration_count = 10000;
  // !--- Test Constants ---!


  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 1 BEGIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n-------------------------------------------------------------\n");
  printf("Iterating over N number of entities with 1 specific component");
  printf("\n-------------------------------------------------------------\n");

  // ========= Our ECS =========
  // Create entities for our ECS
  std::vector<Entity> entities;
  for (int i = 0; i < entity_count; ++i) {
    Result<Entity> entity = ecs_controller.CreateEntity();
    if (entity.Bad()) {
      printf("%s", entity.Error().Message());
      std::terminate();
    }
    Error err = entity->AddComponent<Animals>();
    if (err.Bad()) {
      printf("%s", err.Message());
      std::terminate();
    }
    err = entity->AddComponent<AnimalFood>();
    if (err.Bad()) {
      printf("%s", err.Message());
      std::terminate();
    }
    err = entity->AddComponent<AnimalHabitat>();
    if (err.Bad()) {
      printf("%s", err.Message());
      std::terminate();
    }
    err = entity->AddComponent<AnimalHairStyle>();
    if (err.Bad()) {
      printf("%s", err.Message());
      std::terminate();
    }
    entities.push_back(*entity);
  }

  // Get the AnimalSystem, create a timer and time each iteration of the system entities.
  auto& animal_system = ecs_controller.GetSystem(animal_system_id);
  Timer timer("Our ECS");
  for (int i = 0; i < iteration_count; i++) {
    timer.Start();
    animal_system.Iterate();
    timer.CaptureTimePoint(false);
  }
  timer.PrintAverageTime();
  // !======== Our ECS ========!

  // ========= Vector of Entity pointers =========
  std::vector<AnimalCollectorEntity*> animal_collectors;
  for (size_t i = 0; i < entity_count; ++i) {
    EntityID id{ i };
    animal_collectors.emplace_back(new AnimalCollectorEntity{ id });
    animal_collectors.back()->animal_component = Animals{};
    animal_collectors.back()->animal_food_component = AnimalFood{};
    animal_collectors.back()->animal_habitat_component = AnimalHabitat{};
    animal_collectors.back()->animal_hair_style_component = AnimalHairStyle{};
  }

  Timer timer_3("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_3.Start();
    for (auto& entity : animal_collectors) {
      if (entity) {
        if (entity->animal_component != std::nullopt) {
          auto& animal_component = entity->animal_component.value();
          animal_component.cat[0][1] = 1.0;
          animal_component.dog[2][2] = 1.0;
          animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
        }
      }
    }
    timer_3.CaptureTimePoint(false);
  }
  timer_3.PrintAverageTime();
  // !======== Vector of Entity pointers ========!

  // ======== EnTT ========
  entt::registry registry;
  for (int i = 0; i < entity_count; ++i) {
    const auto entity = registry.create();
    registry.emplace<Animals>(entity);
    registry.emplace<AnimalFood>(entity);
    registry.emplace<AnimalHairStyle>(entity);
    registry.emplace<AnimalHabitat>(entity);
  }

  Timer timer_4("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_4.Start();
    auto view = registry.view<Animals>();
    for (auto entity : view) {
      auto& animals = view.get<Animals>(entity);
      animals.cat[0][1] = 1.0;
      animals.dog[2][2] = 1.0;
      animals.fish[3][3] = animals.dog[2][2] + 2.0f;
    }
    timer_4.CaptureTimePoint(false);
  }
  timer_4.PrintAverageTime();
  // !======= EnTT =======!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 1 END !!!!!!!!!!!!!!!!!!!!!!!!!!!


  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 2 BEGIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf(
    "\n-----------------------------------------------------------------------------------------------------------\n");
  printf(
    "Delete approximately 60%% of the entities and then iterate of N number of entities with 1 specific component");
  printf(
    "\n-----------------------------------------------------------------------------------------------------------\n");

  // Delete entities in all three solutions
  for (size_t i = 0; i < entity_count; ++i) {
    int num = roll(0, 10);
    if (num <= 6) {
      // Destroy our entities
      entities[i].Destroy();
      // Destroy EnTT entities
      registry.destroy(static_cast<entt::entity>(i));
      // Delete pointer entity
      delete animal_collectors[i];
      animal_collectors[i] = nullptr;
    }
  }

  // ========= Our ECS =========
  Timer timer_5("Our ECS");
  for (int i = 0; i < iteration_count; i++) {
    timer_5.Start();
    animal_system.Iterate();
    timer_5.CaptureTimePoint(false);
  }
  timer_5.PrintAverageTime();
  // !======== Our ECS ========!

  // ========= Vector of Entity pointers =========
  Timer timer_6("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_6.Start();
    for (auto& entity : animal_collectors) {
      if (entity != nullptr) {
        if (entity->animal_component != std::nullopt) {
          auto& animal_component = entity->animal_component.value();
          animal_component.cat[0][1] = 1.0;
          animal_component.dog[2][2] = 1.0;
          animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
        }
      }
    }
    timer_6.CaptureTimePoint(false);
  }
  timer_6.PrintAverageTime();
  // !======== Vector of Entity pointers ========!

  // ======== EnTT ========
  Timer timer_7("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_7.Start();
    auto view = registry.view<Animals>();
    for (auto entity : view) {
      auto& animals = view.get<Animals>(entity);
      animals.cat[0][1] = 1.0;
      animals.dog[2][2] = 1.0;
      animals.fish[3][3] = animals.dog[2][2] + 2.0f;
    }
    timer_7.CaptureTimePoint(false);
  }
  timer_7.PrintAverageTime();
  // !======= EnTT =======!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 2 END !!!!!!!!!!!!!!!!!!!!!!!!!!!


  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 3 BEGIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n---------------------------------------------------------------------\n");
  printf("Iterate over N number of entities and get 2 components from that entity");
  printf("\n---------------------------------------------------------------------\n");

  // ========= Our ECS =========
  Timer timer_8("Our ECS");
  for (int i = 0; i < iteration_count; i++) {
    timer_8.Start();
    animal_system.Iterate2Components();
    timer_8.CaptureTimePoint(false);
  }
  timer_8.PrintAverageTime();
  // !======== Our ECS ========!

  // ========= Vector of Entity pointers =========
  Timer timer_9("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_9.Start();
    for (auto& entity : animal_collectors) {
      if (entity) {
        if (entity->animal_component != std::nullopt) {
          auto& animal_component = entity->animal_component.value();
          animal_component.cat[0][1] = 1.0;
          animal_component.dog[2][2] = 1.0;
          animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
        }
        if (entity->animal_food_component != std::nullopt) {
          auto& animal_food_component = entity->animal_food_component.value();
          animal_food_component.cat_food = 1.0;
          animal_food_component.dog_food = 2.0;
          animal_food_component.fish_food = 3.0;
        }
      }
    }
    timer_9.CaptureTimePoint(false);
  }
  timer_9.PrintAverageTime();
  // !======== Vector of Entity pointers ========!

  // ======== EnTT ========
  Timer timer_10("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_10.Start();
    auto view = registry.view<Animals, AnimalFood>();
    for (auto entity : view) {
      auto& animal_component = view.get<Animals>(entity);
      auto& animal_food_component = view.get<AnimalFood>(entity);
      animal_component.cat[0][1] = 1.0;
      animal_component.dog[2][2] = 1.0;
      animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
      animal_food_component.cat_food = 1.0;
      animal_food_component.dog_food = 2.0;
      animal_food_component.fish_food = 3.0;
    }
    timer_10.CaptureTimePoint(false);
  }
  timer_10.PrintAverageTime();
  // !======= EnTT =======!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 3 END !!!!!!!!!!!!!!!!!!!!!!!!!!!


  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 4 BEGIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n---------------------------------------------------------------------\n");
  printf("Iterate over N number of entities and get 3 components from that entity");
  printf("\n---------------------------------------------------------------------\n");
  // ========= Our ECS =========
  Timer timer_11("Our ECS");
  for (int i = 0; i < iteration_count; i++) {
    timer_11.Start();
    animal_system.Iterate3Components();
    timer_11.CaptureTimePoint(false);
  }
  timer_11.PrintAverageTime();
  // !======== Our ECS ========!

  // ========= Vector of Entity pointers =========
  Timer timer_12("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_12.Start();
    for (auto& entity : animal_collectors) {
      if (entity) {
        if (entity->animal_component != std::nullopt) {
          auto& animal_component = entity->animal_component.value();
          animal_component.cat[0][1] = 1.0;
          animal_component.dog[2][2] = 1.0;
          animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
        }
        if (entity->animal_food_component != std::nullopt) {
          auto& animal_food_component = entity->animal_food_component.value();
          animal_food_component.cat_food = 1.0;
          animal_food_component.dog_food = 2.0;
          animal_food_component.fish_food = 3.0;
        }
        if (entity->animal_hair_style_component != std::nullopt) {
          auto& animal_hair_component = entity->animal_hair_style_component.value();
          animal_hair_component.bald = 1.0;
          animal_hair_component.curly = 2.0;
          animal_hair_component.mohawk = 3.0;
        }
      }
    }
    timer_12.CaptureTimePoint(false);
  }
  timer_12.PrintAverageTime();
  // !======== Vector of Entity pointers ========!

  // ======== EnTT ========
  Timer timer_13("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_13.Start();
    auto view = registry.view<Animals, AnimalFood, AnimalHairStyle>();
    for (auto entity : view) {
      auto& animal_component = view.get<Animals>(entity);
      auto& animal_food_component = view.get<AnimalFood>(entity);
      auto& animal_hair_component = view.get<AnimalHairStyle>(entity);
      animal_component.cat[0][1] = 1.0;
      animal_component.dog[2][2] = 1.0;
      animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
      animal_food_component.cat_food = 1.0;
      animal_food_component.dog_food = 2.0;
      animal_food_component.fish_food = 3.0;
      animal_hair_component.bald = 1.0;
      animal_hair_component.curly = 2.0;
      animal_hair_component.mohawk = 3.0;
    }
    timer_13.CaptureTimePoint(false);
  }
  timer_13.PrintAverageTime();
  // !======= EnTT =======!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 4 END !!!!!!!!!!!!!!!!!!!!!!!!!!!


  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 5 BEGIN !!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n---------------------------------------------------------------------\n");
  printf("Iterate over N number of entities and get 4 components from that entity");
  printf("\n---------------------------------------------------------------------\n");
  // ========= Our ECS =========
  Timer timer_14("Our ECS");
  for (int i = 0; i < iteration_count; i++) {
    timer_14.Start();
    animal_system.Iterate4Components();
    timer_14.CaptureTimePoint(false);
  }
  timer_14.PrintAverageTime();
  // !======== Our ECS ========!

  // ========= Vector of Entity pointers =========
  Timer timer_15("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_15.Start();
    for (auto& entity : animal_collectors) {
      if (entity) {
        if (entity->animal_component != std::nullopt) {
          auto& animal_component = entity->animal_component.value();
          animal_component.cat[0][1] = 1.0;
          animal_component.dog[2][2] = 1.0;
          animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
        }
        if (entity->animal_food_component != std::nullopt) {
          auto& animal_food_component = entity->animal_food_component.value();
          animal_food_component.cat_food = 1.0;
          animal_food_component.dog_food = 2.0;
          animal_food_component.fish_food = 3.0;
        }
        if (entity->animal_hair_style_component != std::nullopt) {
          auto& animal_hair_component = entity->animal_hair_style_component.value();
          animal_hair_component.bald = 1.0;
          animal_hair_component.curly = 2.0;
          animal_hair_component.mohawk = 3.0;
        }
        if (entity->animal_habitat_component != std::nullopt) {
          auto& animal_habitat = entity->animal_habitat_component.value();
          animal_habitat.habitat = 540.0;
        }
      }
    }
    timer_15.CaptureTimePoint(false);
  }
  timer_15.PrintAverageTime();
  // !======== Vector of Entity pointers ========!

  // ======== EnTT ========
  Timer timer_16("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_16.Start();
    auto view = registry.view<Animals, AnimalFood, AnimalHairStyle, AnimalHabitat>();
    for (auto entity : view) {
      auto& animal_component = view.get<Animals>(entity);
      auto& animal_food_component = view.get<AnimalFood>(entity);
      auto& animal_hair_component = view.get<AnimalHairStyle>(entity);
      auto& animal_habitat_component = view.get<AnimalHabitat>(entity);
      animal_component.cat[0][1] = 1.0;
      animal_component.dog[2][2] = 1.0;
      animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0f;
      animal_food_component.cat_food = 1.0;
      animal_food_component.dog_food = 2.0;
      animal_food_component.fish_food = 3.0;
      animal_hair_component.bald = 1.0;
      animal_hair_component.curly = 2.0;
      animal_hair_component.mohawk = 3.0;
      animal_habitat_component.habitat = 540;
    }
    timer_16.CaptureTimePoint(false);
  }
  timer_16.PrintAverageTime();
  // !======= EnTT =======!
  // !!!!!!!!!!!!!!!!!!!!!!!!!!! TEST 5 END !!!!!!!!!!!!!!!!!!!!!!!!!!!

  printf("\n--------------\n");
  printf("Tests Complete");
  printf("\n--------------\n");

  return 0;
}