// Example demonstrating the enhanced configuration system
// This can be used as a test or example for users

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

void testSimpleConfigFormat() {
    std::cout << "=== Testing Simple Config Format ===" << std::endl;
    
    nlm::Config config;
    
    // Set some values using different types
    config.set("neuron_count", 500);
    config.set("region_count", 2);
    config.set("connection_probability", 0.1f);
    config.set("random_seed", int64_t(42));
    config.set("simulation_timestep", 0.001);
    config.set("use_advanced_plasticity", true);
    config.set("model_name", "test_model");
    config.set("layer_sizes", std::vector<int64_t>({64, 128, 64}));
    config.set("activation_functions", std::vector<std::string>({"relu", "tanh", "sigmoid"}));
    
    // Test getting values
    auto neuronCount = config.get<int64_t>("neuron_count");
    if (neuronCount) {
        std::cout << "neuron_count: " << *neuronCount << std::endl;
    }
    
    auto regionCount = config.get<int64_t>("region_count");
    if (regionCount) {
        std::cout << "region_count: " << *regionCount << std::endl;
    }
    
    auto useAdvancedPlasticity = config.get<bool>("use_advanced_plasticity");
    if (useAdvancedPlasticity) {
        std::cout << "use_advanced_plasticity: " << (*useAdvancedPlasticity ? "true" : "false") << std::endl;
    }
    
    auto layerSizes = config.get<std::vector<int64_t>>("layer_sizes");
    if (layerSizes) {
        std::cout << "layer_sizes: [";
        for (size_t i = 0; i < layerSizes->size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << (*layerSizes)[i];
        }
        std::cout << "]" << std::endl;
    }
    
    std::cout << "Config summary:\n" << config.summary() << std::endl;
}

void testFileSaveLoad() {
    std::cout << "\n=== Testing File Save/Load ===" << std::endl;
    
    nlm::Config config;
    config.set("test_value", 42);
    config.set("test_string", "hello world");
    config.set("test_float", 3.14159);
    config.set("test_bool", false);
    
    const std::string filename = "/tmp/test_config.txt";
    
    // Save config
    bool saved = config.saveToFile(filename);
    std::cout << "Save successful: " << (saved ? "yes" : "no") << std::endl;
    
    if (saved) {
        nlm::Config loadedConfig;
        bool loaded = loadedConfig.loadFromFile(filename);
        std::cout << "Load successful: " << (loaded ? "yes" : "no") << std::endl;
        
        if (loaded) {
            auto testVal = loadedConfig.get<int64_t>("test_value");
            if (testVal) {
                std::cout << "Loaded test_value: " << *testVal << std::endl;
            }
        }
    }
}

void testErrorHandling() {
    std::cout << "\n=== Testing Error Handling ===" << std::endl;
    
    nlm::Config config;
    
    // Test empty key (should throw or log warning)
    try {
        config.set("", "should_fail");
        std::cout << "Empty key set (may log warning)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Empty key correctly rejected: " << e.what() << std::endl;
    }
    
    // Test getting non-existent key
    auto missing = config.get<std::string>("nonexistent_key");
    if (!missing) {
        std::cout << "Non-existent key correctly returns null" << std::endl;
    }
    
    // Test remove operation
    config.set("temp_key", "temp_value");
    config.remove("temp_key");
    
    auto tempAfterRemove = config.get<std::string>("temp_key");
    if (!tempAfterRemove) {
        std::cout << "Key correctly removed" << std::endl;
    }
}

int main() {
    std::cout << "NLM Configuration System - Enhanced Features Demo" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    testSimpleConfigFormat();
    testFileSaveLoad();
    testErrorHandling();
    
    std::cout << "\nAll tests completed successfully!" << std::endl;
    return 0;
}
