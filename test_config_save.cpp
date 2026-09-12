#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cmath>

int main() {
    nlm::Config config;
    
    // Set various types of values
    config.set("string_key", std::string("Hello World"));
    config.set("int_key", 42);
    config.set("double_key", 3.14159);
    config.set("bool_key", true);
    
    // Set list values
    std::vector<int> intList = {1, 2, 3, 4, 5};
    config.set("int_list", intList);
    
    std::vector<double> doubleList = {0.1, 0.2, 0.3};
    config.set("double_list", doubleList);
    
    std::vector<std::string> stringList = {"a", "b", "c"};
    config.set("string_list", stringList);
    
    // Save to file
    if (config.saveToFile("test_config.cfg")) {
        std::cout << "Successfully saved config to test_config.cfg" << std::endl;
        
        // Load it back and verify
        nlm::Config config2;
        if (config2.loadFromFile("test_config.cfg")) {
            std::cout << "Successfully loaded config back" << std::endl;
            
            // Verify some values
            auto strVal = config2.get<std::string>("string_key");
            if (strVal && *strVal == "Hello World") {
                std::cout << "✓ String value preserved" << std::endl;
            } else {
                std::cout << "✗ String value mismatch" << std::endl;
            }
            
            auto intVal = config2.get<int>("int_key");
            if (intVal && *intVal == 42) {
                std::cout << "✓ Integer value preserved" << std::endl;
            } else {
                std::cout << "✗ Integer value mismatch" << std::endl;
            }
            
            auto doubleVal = config2.get<double>("double_key");
            if (doubleVal && std::abs(*doubleVal - 3.14159) < 0.0001) {
                std::cout << "✓ Double value preserved" << std::endl;
            } else {
                std::cout << "✗ Double value mismatch" << std::endl;
            }
            
            auto boolVal = config2.get<bool>("bool_key");
            if (boolVal && *boolVal == true) {
                std::cout << "✓ Boolean value preserved" << std::endl;
            } else {
                std::cout << "✗ Boolean value mismatch" << std::endl;
            }
            
        } else {
            std::cout << "Failed to load config back" << std::endl;
        }
    } else {
        std::cout << "Failed to save config" << std::endl;
    }
    
    return 0;
}
