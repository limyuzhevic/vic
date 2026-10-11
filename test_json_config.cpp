#include <iostream>
#include <string>
#include <filesystem>
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing JSON Config System..." << std::endl;
    
    // Create a config
    nlm::Config config;
    
    // Set some values
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    config.set("int_vector", std::vector<int>({1, 2, 3}));
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}));
    
    std::cout << "Config summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test JSON save
    std::string json_file = "test_config.json";
    if (config.saveToFile(json_file)) {
        std::cout << "Successfully saved config to " << json_file << std::endl;
        
        // Create a new config and load from JSON
        nlm::Config config2;
        if (config2.loadFromFile(json_file)) {
            std::cout << "Successfully loaded config from " << json_file << std::endl;
            std::cout << "Config2 summary:" << std::endl;
            std::cout << config2.summary() << std::endl;
            
            // Compare values
            std::cout << "Comparing configs..." << std::endl;
            std::cout << "int_value from config2: " << config2.get<int>("int_value").value_or(-1) << std::endl;
            std::cout << "double_value from config2: " << config2.get<double>("double_value").value_or(-1.0) << std::endl;
            std::cout << "string_value from config2: " << config2.get<std::string>("string_value").value_or("unknown") << std::endl;
            std::cout << "bool_value from config2: " << config2.get<bool>("bool_value").value_or(false) << std::endl;
            
            // Clean up
            std::filesystem::remove(json_file);
            std::cout << "Test completed successfully!" << std::endl;
            return 0;
        } else {
            std::cout << "Failed to load config from " << json_file << std::endl;
        }
    } else {
        std::cout << "Failed to save config to " << json_file << std::endl;
    }
    
    return 1;
}
