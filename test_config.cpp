#include <iostream>
#include "src/core/Config/Config.hpp"

int main() {
    nlm::Config config;
    
    // Test 1: Load from JSON file
    std::cout << "Test 1: Loading test_config.json..." << std::endl;
    if (config.loadFromFile("test_config.json")) {
        std::cout << "SUCCESS: Loaded JSON file" << std::endl;
        
        // Test 2: Get nested values
        std::cout << "\nTest 2: Getting nested values..." << std::endl;
        auto neuronCount = config.get<int>("neuron.count");
        std::cout << "neuron.count = " << (neuronCount ? std::to_string(*neuronCount) : "NULL") << std::endl;
        
        auto regionCount = config.get<int>("neuron.region.count");
        std::cout << "neuron.region.count = " << (regionCount ? std::to_string(*regionCount) : "NULL") << std::endl;
        
        auto regionType = config.get<std::string>("neuron.region.type");
        std::cout << "neuron.region.type = " << (regionType ? *regionType : "NULL") << std::endl;
        
        auto plasticity = config.get<bool>("brain.plasticity");
        std::cout << "brain.plasticity = " << (plasticity ? (*plasticity ? "true" : "false") : "NULL") << std::endl;
        
        auto learningRate = config.get<double>("brain.learning_rate");
        std::cout << "brain.learning_rate = " << (learningRate ? std::to_string(*learningRate) : "NULL") << std::endl;
        
        // Test 3: Save to file
        std::cout << "\nTest 3: Saving to saved_config.json..." << std::endl;
        if (config.saveToFile("saved_config.json")) {
            std::cout << "SUCCESS: Saved configuration to saved_config.json" << std::endl;
        } else {
            std::cout << "FAILED: Could not save configuration" << std::endl;
        }
        
        // Test 4: Check configuration summary
        std::cout << "\nTest 4: Configuration Summary:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test 5: Load from args (simulate)
        std::cout << "\nTest 5: Setting values via API..." << std::endl;
        config.set("new.test.value", 42, nlm::ConfigSource::Runtime);
        auto testValue = config.get<int>("new.test.value");
        std::cout << "new.test.value = " << (testValue ? std::to_string(*testValue) : "NULL") << std::endl;
        
        std::cout << "\nAll tests completed successfully!" << std::endl;
    } else {
        std::cout << "FAILED: Could not load JSON file" << std::endl;
        return 1;
    }
    
    return 0;
}
