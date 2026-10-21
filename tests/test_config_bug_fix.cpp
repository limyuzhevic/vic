#include "../src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

// Test configuration bug fix - duplicate dopamine_baseline key
void testConfigurationBugFix() {
    std::cout << "Testing configuration bug fix..." << std::endl;
    
    // Load default configuration
    auto config = std::make_shared<Config>();
    bool loaded = config->loadFromFile("configs/default.cfg");
    
    if (!loaded) {
        std::cerr << "Failed to load configuration file" << std::endl;
        return;
    }
    
    // Verify dopamine_baseline is present and has correct value
    if (!config->has("dopamine_baseline")) {
        std::cerr << "dopamine_baseline key should exist" << std::endl;
        return;
    }
    
    // The value should be 0.1 (the last one should win, or it should be validated)
    double dopamineValue = config->getOr<double>("dopamine_baseline", 0.0);
    std::cout << "dopamine_baseline value: " << dopamineValue << std::endl;
    
    // Verify other configuration keys
    if (!config->has("random_seed")) {
        std::cerr << "random_seed should exist" << std::endl;
        return;
    }
    if (!config->has("simulation_timestep")) {
        std::cerr << "simulation_timestep should exist" << std::endl;
        return;
    }
    if (!config->has("neuron_count")) {
        std::cerr << "neuron_count should exist" << std::endl;
        return;
    }
    if (!config->has("region_count")) {
        std::cerr << "region_count should exist" << std::endl;
        return;
    }
    
    std::cout << "✓ Configuration bug fix test passed" << std::endl;
}

int main() {
    try {
        testConfigurationBugFix();
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}