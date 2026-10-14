// Test program to verify JSON configuration loading
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing NLM JSON Configuration Loading" << std::endl;
    std::cout << "======================================" << std::endl;
    
    // Test 1: Load JSON configuration
    nlm::Config config;
    std::cout << "\n1. Loading JSON configuration..." << std::endl;
    
    bool loadResult = config.loadFromFile("configs/test_config.json");
    if (loadResult) {
        std::cout << "   ✓ Successfully loaded JSON configuration" << std::endl;
        
        // Test 2: Verify values were loaded correctly
        std::cout << "\n2. Verifying loaded values..." << std::endl;
        
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount.has_value()) {
            std::cout << "   ✓ neuron_count = " << *neuronCount << std::endl;
        }
        
        auto regionCount = config.get<int64_t>("region_count");
        if (regionCount.has_value()) {
            std::cout << "   ✓ region_count = " << *regionCount << std::endl;
        }
        
        auto dopamineBaseline = config.get<double>("dopamine_baseline");
        if (dopamineBaseline.has_value()) {
            std::cout << "   ✓ dopamine_baseline = " << *dopamineBaseline << std::endl;
        }
        
        auto testBool = config.get<bool>("test_bool_true");
        if (testBool.has_value()) {
            std::cout << "   ✓ test_bool_true = " << (*testBool ? "true" : "false") << std::endl;
        }
        
        auto testArrayInts = config.get<std::vector<int64_t>>("test_array_integers");
        if (testArrayInts.has_value()) {
            std::cout << "   ✓ test_array_integers = [";
            for (size_t i = 0; i < testArrayInts->size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << (*testArrayInts)[i];
            }
            std::cout << "]" << std::endl;
        }
        
        // Test 3: Save configuration (should be in key=value format)
        std::cout << "\n3. Testing configuration save..." << std::endl;
        bool saveResult = config.saveToFile("configs/test_saved.cfg");
        if (saveResult) {
            std::cout << "   ✓ Successfully saved configuration" << std::endl;
            
            // Test 4: Load saved configuration
            std::cout << "\n4. Testing saved configuration load..." << std::endl;
            nlm::Config config2;
            bool loadSavedResult = config2.loadFromFile("configs/test_saved.cfg");
            if (loadSavedResult) {
                std::cout << "   ✓ Successfully loaded saved configuration" << std::endl;
                
                auto savedNeuronCount = config2.get<int64_t>("neuron_count");
                if (savedNeuronCount.has_value()) {
                    std::cout << "   ✓ Saved configuration contains neuron_count = " << *savedNeuronCount << std::endl;
                }
            } else {
                std::cout << "   ✗ Failed to load saved configuration" << std::endl;
            }
        } else {
            std::cout << "   ✗ Failed to save configuration" << std::endl;
        }
        
    } else {
        std::cout << "   ✗ Failed to load JSON configuration" << std::endl;
    }
    
    // Test 5: Load original config file to show backward compatibility
    std::cout << "\n5. Testing backward compatibility with original config file..." << std::endl;
    nlm::Config config3;
    bool loadOriginalResult = config3.loadFromFile("configs/default.cfg");
    if (loadOriginalResult) {
        std::cout << "   ✓ Successfully loaded original config file (key=value format)" << std::endl;
        
        auto originalNeuronCount = config3.get<int64_t>("neuron_count");
        if (originalNeuronCount.has_value()) {
            std::cout << "   ✓ Original config contains neuron_count = " << *originalNeuronCount << std::endl;
        }
        
        auto originalDopamineBaseline = config3.get<double>("dopamine_baseline");
        if (originalDopamineBaseline.has_value()) {
            std::cout << "   ✓ Original config contains dopamine_baseline = " << *originalDopamineBaseline << std::endl;
        }
    } else {
        std::cout << "   ✗ Failed to load original config file" << std::endl;
    }
    
    std::cout << "\n" << std::endl;
    std::cout << "Configuration Loading Tests Complete!" << std::endl;
    
    return 0;
}