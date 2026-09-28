#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

int main() {
    std::cout << "Testing Config JSON/YAML parsing with nlohmann/json...\n";
    
    nlm::Config config;
    
    // Test 1: Set various values
    config.set("int_value", 42);
    config.set("double_value", 3.14159);
    config.set("string_value", "Hello World");
    config.set("bool_value", true);
    config.set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
    config.set("string_array", std::vector<std::string>({"apple", "banana", "cherry"}));
    
    // Test getting values
    assert(config.get<int>("int_value").value() == 42);
    assert(config.get<double>("double_value").value() > 3.14 && config.get<double>("double_value").value() < 3.15);
    assert(config.get<std::string>("string_value").value() == "Hello World");
    assert(config.get<bool>("bool_value").value() == true);
    
    auto intArray = config.get<std::vector<int>("int_array");
    assert(intArray.has_value());
    assert(intArray.value() == std::vector<int>({1, 2, 3, 4, 5}));
    
    auto stringArray = config.get<std::vector<std::string>("string_array");
    assert(stringArray.has_value());
    assert(stringArray.value() == std::vector<std::string>({"apple", "banana", "cherry"}));
    
    // Test saveToFile (JSON)
    std::string jsonFile = "/tmp/test_config.json";
    if (config.saveToFile(jsonFile)) {
        std::cout << "JSON saveToFile successful\n";
        
        // Load the JSON file back
        nlm::Config config2;
        if (config2.loadFromFile(jsonFile)) {
            std::cout << "JSON loadFromFile successful\n";
            
            // Verify all values are preserved
            assert(config2.get<int>("int_value").value() == 42);
            assert(config2.get<double>("double_value").value() > 3.14 && config2.get<double>("double_value").value() < 3.15);
            assert(config2.get<std::string>("string_value").value() == "Hello World");
            assert(config2.get<bool>("bool_value").value() == true);
            
            auto intArray2 = config2.get<std::vector<int>("int_array");
            assert(intArray2.has_value());
            assert(intArray2.value() == std::vector<int>({1, 2, 3, 4, 5}));
            
            auto stringArray2 = config2.get<std::vector<std::string>("string_array");
            assert(stringArray2.has_value());
            assert(stringArray2.value() == std::vector<std::string>({"apple", "banana", "cherry"}));
            
            std::cout << "All JSON tests passed!\n";
        } else {
            std::cout << "JSON loadFromFile failed\n";
            return 1;
        }
    } else {
        std::cout << "JSON saveToFile failed\n";
        return 1;
    }
    
    // Test saveToFile (key=value format)
    std::string kvFile = "/tmp/test_config.kv";
    if (config.saveToFile(kvFile)) {
        std::cout << "Key-value saveToFile successful\n";
        
        // Load the key-value file back
        nlm::Config config3;
        if (config3.loadFromFile(kvFile)) {
            std::cout << "Key-value loadFromFile successful\n";
            
            // Verify all values are preserved
            assert(config3.get<int>("int_value").value() == 42);
            assert(config3.get<std::string>("string_value").value() == "Hello World");
            
            std::cout << "All key-value tests passed!\n";
        } else {
            std::cout << "Key-value loadFromFile failed\n";
            return 1;
        }
    } else {
        std::cout << "Key-value saveToFile failed\n";
        return 1;
    }
    
    std::cout << "\nAll Config JSON/YAML parsing tests with nlohmann/json passed successfully!\n";
    return 0;
}