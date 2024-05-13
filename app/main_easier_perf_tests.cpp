#include "ecs/ecs_controller.hpp"
#include "entt/entt.hpp"
#include "ids.hpp"

#include "Windows.h"
#include <iostream>
#include <profileapi.h>
#include <random>
#include <winnt.h>

// This application consists of 4 performance tests/comparisons
// 1. Iterating over N number of entities and getting 1 component out of it

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

// An entity struct to act as an alternative to Entities. This is used to compare malloc'd entities/structs vs ECS.
// This is to show that using this pattern is perfectly fine if you don't need thousands of entities and don't rate the
// ECS composition pattern and code structure.
struct AnimalCollector
{
  EntityID id;
  std::optional<Animals> animal_component;
  std::optional<AnimalFood> animal_food_component;
};

/**
 * @brief The system to be used that's registered with this ECS.
 *
 */
class MyAnimalSystem : public System
{
public:
  MyAnimalSystem(ComponentID<Animals>& animal_comp_id, ComponentID<AnimalFood>& animal_food_id)
    : animal_comp_id_(animal_comp_id), animal_food_id_(animal_food_id)
  {}

  void Iterate2comps()
  {
    for (auto& entity : entities) {
      auto& animal_component = entity.GetComponent(animal_comp_id_);
      auto& animal_food_component = entity.GetComponent(animal_food_id_);
      animal_component.cat[0][1] = 1.0;
      animal_component.dog[2][2] = 1.0;
      animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
      animal_food_component.cat_food = 1.0;
      animal_food_component.dog_food = 1.0;
      animal_food_component.fish_food = 1.0;
    }
  }

  void Iterate()
  {
    for (auto& entity : entities) {
      auto& animal_component = entity.GetComponent(animal_comp_id_);
      animal_component.cat[0][1] = 1.0;
      animal_component.dog[2][2] = 1.0;
      animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
    }
  }

  // void Iterate()
  // {
  //   bool done{ false };
  //   std::vector<Animals>::iterator it;
  //   for (auto& entity : entities) {
  //     if (!done) {
  //       // Get the iterator directly and iterate through the component vec.
  //       it = entity.GetComponentIterator(animal_comp_id_);
  //       done = true;
  //     }
  //     auto& animal_component = *it;
  //     animal_component.cat[0][1] = 1.0;
  //     animal_component.dog[2][2] = 1.0;
  //     animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
  //     it++;
  //   }
  // }

  Timer get_component_timer{ "getcomponent" };

private:
  ComponentID<Animals> animal_comp_id_;
  ComponentID<AnimalFood> animal_food_id_;
};

int main(int argc, const char** argv)
{
  // Instantiate our ECS
  ECSController ecs_controller;

  // Register components
  auto animal_component_id = ecs_controller.RegisterComponent<Animals>();
  auto animal_food_component_id = ecs_controller.RegisterComponent<AnimalFood>();

  // Create signature for our system
  SystemSignature animal_system_signature;
  animal_system_signature.SetComponent(animal_component_id);

  // Register system
  auto animal_system_id = ecs_controller.RegisterSystem<MyAnimalSystem>(
    animal_system_signature, animal_component_id, animal_food_component_id);

  // ---- Test Constants ----
  // Create 10000 entities
  constexpr int entity_count = 10000;
  constexpr int iteration_count = 10000;
  // !--- Test Constants ---!

  // ========= Our ECS =========
  // Create entities for our ECS
  std::vector<Entity> entities;
  for (int i = 0; i < entity_count; ++i) {
    Result<Entity> entity = ecs_controller.CreateEntity();
    if (entity.Bad()) {
      printf("%s", entity.Error().Message());
      std::terminate();
    }
    Error err = entity->AddComponent(animal_component_id);
    if (err.Bad()) {
      printf("%s", err.Message());
      std::terminate();
    }
    err = entity->AddComponent(animal_food_component_id);
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
  std::vector<AnimalCollector*> animal_collectors;
  for (int i = 0; i < entity_count; ++i) {
    EntityID id{ i };
    animal_collectors.emplace_back(new AnimalCollector{ id });
    animal_collectors.back()->animal_component = Animals{};
    animal_collectors.back()->animal_food_component = AnimalFood{};
  }

  Timer timer_3("Vector of Entity pointers");
  for (int i = 0; i < iteration_count; i++) {
    timer_3.Start();
    for (auto& entity : animal_collectors) {
      if (entity->animal_component != std::nullopt) {
        auto& animal_component = entity->animal_component.value();
        animal_component.cat[0][1] = 1.0;
        animal_component.dog[2][2] = 1.0;
        animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
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
  }

  Timer timer_4("EnTT");
  for (int i = 0; i < iteration_count; i++) {
    timer_4.Start();
    auto view = registry.view<Animals>();
    for (auto entity : view) {
      auto& animals = view.get<Animals>(entity);
      animals.cat[0][1] = 1.0;
      animals.dog[2][2] = 1.0;
      animals.fish[3][3] = animals.dog[2][2] + 2.0;
    }
    timer_4.CaptureTimePoint(false);
  }
  timer_4.PrintAverageTime();
  // !======= EnTT =======!

  // Timer timer_5("My ECS - iterate 2");
  // for (int i = 0; i < iteration_count; i++) {
  //   timer_5.Start();
  //   animal_system.Iterate2comps();
  //   timer_5.CaptureTimePoint(false);
  // }
  // timer_5.PrintAverageTime();

  // Timer timer_6("EnTT - Iterate2");
  // for (int i = 0; i < iteration_count; i++) {
  //   timer_6.Start();
  //   auto view = registry.view<Animals, AnimalFood>();
  //   for (auto entity : view) {
  //     auto& animal_component = view.get<Animals>(entity);
  //     auto& animal_food_component = view.get<AnimalFood>(entity);
  //     animal_component.cat[0][1] = 1.0;
  //     animal_component.dog[2][2] = 1.0;
  //     animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
  //     animal_food_component.cat_food = 1.0;
  //     animal_food_component.dog_food = 1.0;
  //     animal_food_component.fish_food = 1.0;
  //   }
  //   timer_6.CaptureTimePoint(false);
  // }
  // timer_6.PrintAverageTime();

  // Timer timer_7("Vec of pointers - 2 components");
  // for (int i = 0; i < iteration_count; i++) {
  //   timer_7.Start();
  //   for (auto& entity : animal_collectors) {
  //     if (entity->animal_component != std::nullopt) {
  //       auto& animal_component = entity->animal_component.value();
  //       animal_component.cat[0][1] = 1.0;
  //       animal_component.dog[2][2] = 1.0;
  //       animal_component.fish[3][3] = animal_component.dog[2][2] + 2.0;
  //     }
  //     if (entity->animal_food_component != std::nullopt) {
  //       auto& animal_food_component = entity->animal_food_component.value();
  //       animal_food_component.cat_food = 1.0;
  //       animal_food_component.dog_food = 1.0;
  //       animal_food_component.fish_food = 1.0;
  //     }
  //   }
  //   timer_7.CaptureTimePoint(false);
  // }
  // timer_7.PrintAverageTime();

  return 0;
}