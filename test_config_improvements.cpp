#include <iostream>
#include <fstream>
#include <string>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing improved Config class..." << std::endl;
    
    // Test 1: Basic config operations
    nlm::Config config;
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test get operations
    auto intVal = config.get<int>("int_value");
    if (intVal && *intVal == 42) {
        std::cout << "✓ Basic get operations work" << std::endl;
    } else {
        std::cout << "✗ Basic get operations failed" << std::endl;
        return 1;
    }
    
    // Test getOr with default
    int defaultVal = config.getOr<int>("missing", 100);
    if (defaultVal == 100) {
        std::cout << "✓ getOr with default works" << std::endl;
    } else {
        std::cout << "✗ getOr with default failed" << std::endl;
        return 1;
    }
    
    // Test JSON save/load
    std::string jsonFile = "test_config.json";
    if (config.saveToFile(jsonFile)) {
        std::cout << "✓ JSON save works" << std::endl;
        
        // Try to load the JSON file
        nlm::Config config2;
        if (config2.loadFromFile(jsonFile)) {
            std::cout << "✓ JSON load works" << std::endl;
            
            // Verify values were loaded correctly
            auto loadedInt = config2.get<int>("int_value");
            if (loadedInt && *loadedInt == 42) {
                std::cout << "✓ JSON loaded values correctly" << std::endl;
            } else {
                std::cout << "✗ JSON loaded values incorrectly" << std::endl;
                return 1;
            }
        } else {
            std::cout << "✗ JSON load failed" << std::endl;
            return 1;
        }
    } else {
        std::cout << "✗ JSON save failed" << std::endl;
        return 1;
    }
    
    // Clean up
    std::remove(jsonFile.c_str());
    
    // Test with complex types (arrays)
    nlm::Config config3;
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    config3.set("int_array", intArray);
    auto loadedArray = config3.get<std::vector<int>>("int_array");
    
    if (loadedArray && loadedArray->size() == 5) {
        std::cout << "✓ Complex types (arrays) work" << std::endl;
    } else {
        std::cout << "✗ Complex types failed" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== All Config improvements working correctly! ===" << std::endl;
    
    return 0;
}