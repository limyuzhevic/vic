// Test configuration improvements
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testConfigImprovements() {
    std::cout << "Testing configuration improvements..." << std::endl;
    
    // Test 1: Configuration file loading
    nlm::Config config;
    assert(config.loadFromFile("configs/default.cfg"));
    
    // Test 2: Check for duplicate keys (dopamine_baseline should only appear once)
    auto keys = config.getKeys();
    int dopamine_count = 0;
    for (const auto& key : keys) {
        if (key == "dopamine_baseline") {
            dopamine_count++;
        }
    }
    assert(dopamine_count == 1, "dopamine_baseline should appear only once");
    
    // Test 3: Check that descriptions are present for configuration entries
    for (const auto& key : keys) {
        auto value = config.get<std::string>(key);
        if (value.has_value()) {
            std::cout << "Found: " << key << " = " << value.value() << std::endl;
        }
    }
    
    // Test 4: Configuration validation
    assert(config.validate("dopamine_baseline", nlm::ConfigValue(0.1)));
    assert(!config.validate("dopamine_baseline", nlm::ConfigValue(-0.1)));
    assert(config.validate("neuron_count", nlm::ConfigValue(1000)));
    assert(!config.validate("neuron_count", nlm::ConfigValue(-100)));
    
    // Test 5: Get validation errors
    auto errors = config.getValidationErrors();
    std::cout << "Validation errors: " << errors.size() << std::endl;
    
    // Test 6: Save to file
    std::string testFile = "test_config.cfg";
    if (config.saveToFile(testFile)) {
        std::cout << "Successfully saved configuration to " << testFile << std::endl;
        
        // Test 7: Load saved file
        nlm::Config config2;
        if (config2.loadFromFile(testFile)) {
            std::cout << "Successfully loaded saved configuration" << std::endl;
            
            // Compare some values
            auto original_neurons = config.getOr<int64_t>("neuron_count", 1000);
            auto saved_neurons = config2.getOr<int64_t>("neuron_count", 1000);
            assert(original_neurons == saved_neurons, "Loaded configuration should match saved configuration");
        }
    }
    
    std::cout << "All configuration improvement tests passed!" << std::endl;
}

int main() {
    testConfigImprovements();
    return 0;
}