#include <iostream>
#include <fstream>
#include <filesystem>
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== Simple NLM Config JSON Parser Test ===" << std::endl;
    
    // Create a simple configuration
    nlm::Config config;
    config.set("brain.neuron_count", 1000);
    config.set("brain.synapse_density", 0.1f);
    config.set("plasticity.stdp.enable", true);
    
    std::cout << "Configuration created:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test 1: Save to JSON file
    std::cout << "\nTest 1: Saving to JSON file" << std::endl;
    if (config.saveToFile("test_config.json")) {
        std::cout << "✅ Saved to test_config.json" << std::endl;
    } else {
        std::cout << "❌ Failed to save to test_config.json" << std::endl;
        return 1;
    }
    
    // Test 2: Load from JSON file
    std::cout << "\nTest 2: Loading from JSON file" << std::endl;
    nlm::Config config2;
    if (config2.loadFromFile("test_config.json")) {
        std::cout << "✅ Loaded from test_config.json" << std::endl;
        
        // Verify values
        if (config2.get<int>("brain.neuron_count").value_or(0) == 1000) {
            std::cout << "✅ neuron_count verified: " << config2.get<int>("brain.neuron_count").value_or(0) << std::endl;
        } else {
            std::cout << "❌ neuron_count mismatch" << std::endl;
            return 1;
        }
    } else {
        std::cout << "❌ Failed to load from test_config.json" << std::endl;
        return 1;
    }
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "\n🎉 All tests passed! 🎉" << std::endl;
    return 0;
}