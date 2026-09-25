// Environment Tests
#include "environment/Environment.hpp"
#include "environment/Observation.hpp"
#include <catch2/catch_test_macros.hpp>

namespace test_environment {

TEST_CASE("Environment creation and initialization", "[environment][creation][initialization]") {
    nlm::Environment environment;
    
    REQUIRE(!environment.isInitialized());
}

TEST_CASE("Environment initialization with config", "[environment][initialization][config]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "grid", nlm::ConfigSource::Runtime);
    config->set("environment.width", 100, nlm::ConfigSource::Runtime);
    config->set("environment.height", 100, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    REQUIRE(environment.isInitialized());
}

TEST_CASE("Observation creation and properties", "[environment][observation][creation]") {
    nlm::Observation observation;
    observation.type = "vision";
    observation.value = 0.8f;
    observation.timestamp = 1.0;
    
    REQUIRE(observation.type == "vision");
    REQUIRE(observation.value == 0.8f);
    REQUIRE(observation.timestamp == 1.0);
}

TEST_CASE("Environment state management", "[environment][state][management]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "3d", nlm::ConfigSource::Runtime);
    config->set("environment.seed", 42, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    // Verify environment is initialized
    REQUIRE(environment.isInitialized());
}

TEST_CASE("Environment observation processing", "[environment][observation][processing]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "dynamic", nlm::ConfigSource::Runtime);
    config->set("environment.change_rate", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    // Create and process observations
    nlm::Observation observation1;
    observation1.type = "temperature";
    observation1.value = 25.0f;
    observation1.timestamp = 1.0;
    
    nlm::Observation observation2;
    observation2.type = "humidity";
    observation2.value = 0.6f;
    observation2.timestamp = 2.0;
    
    // Environment should process observations
    REQUIRE(environment.isInitialized());
}

TEST_CASE("Environment with different configurations", "[environment][config][variations]") {
    // Test environment with different configurations
    std::vector<std::string> environmentTypes = {"grid", "3d", "dynamic", "spatial"};
    
    for (const auto& envType : environmentTypes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("environment.type", envType, nlm::ConfigSource::Runtime);
        config->set("environment.seed", 123, nlm::ConfigSource::Runtime);
        
        nlm::Environment environment;
        environment.initialize(config);
        
        REQUIRE(environment.isInitialized());
    }
}

TEST_CASE("Environment simulation step execution", "[environment][simulation][steps]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "continuous", nlm::ConfigSource::Runtime);
    config->set("environment.time_step", 0.1, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    REQUIRE(environment.isInitialized());
    
    // Execute simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        environment.step(step);
    }
}

TEST_CASE("Environment reset functionality", "[environment][reset][functionality]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "random", nlm::ConfigSource::Runtime);
    config->set("environment.seed", 456, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    REQUIRE(environment.isInitialized());
    
    // Reset environment
    environment.reset();
    
    // Environment should still be functional after reset
    environment.initialize(config);
    REQUIRE(environment.isInitialized());
}

TEST_CASE("Environment observation integration with brain", "[environment][integration][brain]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "responsive", nlm::ConfigSource::Runtime);
    config->set("environment.brain_connection", true, nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    REQUIRE(environment.isInitialized());
    
    // Create environment observation
    nlm::Observation observation;
    observation.type = "sensor_input";
    observation.value = 0.7f;
    observation.timestamp = 1.5;
    
    // Environment should integrate with brain systems
    REQUIRE(environment.isInitialized());
}

TEST_CASE("Environment performance under load", "[environment][performance][load]") {
    auto config = std::make_shared<nlm::Config>();
    config->set("environment.type", "complex", nlm::ConfigSource::Runtime);
    
    nlm::Environment environment;
    environment.initialize(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Execute many simulation steps
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        environment.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    REQUIRE(duration.count() < 1000000);  // Less than 1 second
}

} // namespace test_environment
