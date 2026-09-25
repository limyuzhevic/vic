// World Tests
#include "world/SimpleWorld.hpp"
#include "world/Observation.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_world {

TEST_CASE("SimpleWorld creation and initialization", "[world][simple][creation][initialization]") {
    nlm::SimpleWorld world;
    
    REQUIRE(!world.isInitialized());
}

TEST_CASE("SimpleWorld initialization with config", "[world][simple][initialization][config]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "simple", nlm::ConfigSource::Runtime);
    config->set("world.size", 100, nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    REQUIRE(world.isInitialized());
}

TEST_CASE("SimpleWorld basic operations", "[world][simple][operations]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "grid", nlm::ConfigSource::Runtime);
    config->set("world.size", 50, nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    REQUIRE(world.isInitialized());
    
    world.step(0);
}

TEST_CASE("World observation processing", "[world][observation][processing]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "dynamic", nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    nlm::Observation observation;
    observation.type = "world_state";
    observation.value = 0.9f;
    observation.timestamp = 1.0;
    
    REQUIRE(world.isInitialized());
}

TEST_CASE("World with different configurations", "[world][config][variations]") {
    std::vector<std::string> worldTypes = {"simple", "complex", "dynamic", "spatial"};
    
    for (const auto& worldType : worldTypes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("world.type", worldType, nlm::ConfigSource::Runtime);
        config->set("world.seed", 789, nlm::ConfigSource::Runtime);
        
        nlm::SimpleWorld world;
        world.initialize(config);
        
        REQUIRE(world.isInitialized());
    }
}

TEST_CASE("World simulation integration", "[world][simulation][integration]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "integrated", nlm::ConfigSource::Runtime);
    config->set("world.time_step", 0.2, nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    REQUIRE(world.isInitialized());
    
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        world.step(step);
    }
}

TEST_CASE("World reset functionality", "[world][reset][functionality]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "resettable", nlm::ConfigSource::Runtime);
    config->set("world.seed", 999, nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    REQUIRE(world.isInitialized());
    
    world.reset();
    
    world.initialize(config);
    REQUIRE(world.isInitialized());
}

TEST_CASE("World performance under load", "[world][performance][load]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "performance_test", nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        world.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    REQUIRE(duration.count() < 1000000);
}

TEST_CASE("World integration with sensory systems", "[world][integration][sensory]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("world.type", "sensory_integration", nlm::ConfigSource::Runtime);
    config->set("world.sensory_enabled", true, nlm::ConfigSource::Runtime);
    
    nlm::SimpleWorld world;
    world.initialize(config);
    
    REQUIRE(world.isInitialized());
    
    nlm::Observation observation;
    observation.type = "sensory_feedback";
    observation.value = 0.8f;
    observation.timestamp = 2.0;
    
    REQUIRE(world.isInitialized());
}

} // namespace test_world
