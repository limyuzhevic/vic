// Test for ConfigurationLoader
#include "application/ConfigurationLoader.hpp"
#include <cassert>
#include <iostream>

namespace test_configuration_loader {
    void runAll() {
        std::cout << "Testing ConfigurationLoader..." << std::endl;
        
        // Test default values
        ConfigurationLoader loader;
        
        // Create a temporary config file for testing
        // For simplicity, we'll test with the default config file
        auto config = loader.load("configs/default.cfg");
        
        // Test that config has required values
        assert(config->has("random_seed"));
        assert(config->has("simulation_timestep"));
        assert(config->has("neuron_count"));
        assert(config->has("region_count"));
        assert(config->has("connection_probability"));
        
        // Test default values
        assert(config->getOr<int64_t>("random_seed", 42) == 42);
        assert(config->getOr<double>("simulation_timestep", 0.001) == 0.001);
        assert(config->getOr<int64_t>("neuron_count", 500) == 500);
        assert(config->getOr<int64_t>("region_count", 1) == 1);
        assert(config->getOr<float>("connection_probability", 0.15f) == 0.15f);
        
        std::cout << "  PASSED" << std::endl;
    }
}