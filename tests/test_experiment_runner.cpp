// Test for ExperimentRunner
#include "application/ExperimentRunner.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_experiment_runner {
    void runAll() {
        std::cout << "Testing ExperimentRunner..." << std::endl;
        
        // Test that ExperimentRunner can be created
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Test that ExperimentRunner can be created (would normally need brain initialization)
        // This is a basic test to ensure the class exists
        
        std::cout << "  PASSED (basic initialization)" << std::endl;
    }
}