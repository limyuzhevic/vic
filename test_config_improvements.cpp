#include <iostream>
#include <cassert>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== Testing Improved Config System ===" << std::endl;
    
    // Test 1: Basic configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 500);
    config->set("enabled", true);
    
    // Verify basic values
    assert(config->get<int>("random_seed").value() == 42);
    assert(config->get<double>("simulation_timestep").value() == 0.001);
    assert(config->get<int64_t>("neuron_count").value() == 500);
    assert(config->get<bool>("enabled").value() == true);
    
    std::cout << "✓ Basic configuration test passed" << std::endl;
    
    // Test 2: Configuration validation
    assert(config->has("random_seed"));
    assert(config->has("simulation_timestep"));
    assert(config->has("neuron_count"));
    assert(config->has("enabled"));
    
    // Test 3: Configuration summary
    std::string summary = config->summary();
    assert(!summary.empty());
    assert(summary.find("random_seed") != std::string::npos);
    
    std::cout << "✓ Configuration validation test passed" << std::endl;
    
    // Test 4: Configuration modification
    config->set("random_seed", 100);
    assert(config->get<int>("random_seed").value() == 100);
    
    config->remove("neuron_count");
    assert(!config->has("neuron_count"));
    
    std::cout << "✓ Configuration modification test passed" << std::endl;
    
    // Test 5: Configuration getOr method
    int seed = config->getOr<int>("random_seed", 0);
    assert(seed == 100);
    
    int defaultValue = config->getOr<int>("nonexistent_key", 999);
    assert(defaultValue == 999);
    
    std::cout << "✓ Configuration getOr method test passed" << std::endl;
    
    std::cout << "\n=== All Config System Tests Passed ===" << std::endl;
    std::cout << "Configuration system improvements verified successfully!" << std::endl;
    
    return 0;
}