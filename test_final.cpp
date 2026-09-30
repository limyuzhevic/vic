#include <iostream>
#include <cassert>
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "=== Testing Enhanced Config Class ===" << std::endl;
    
    try {
        // Test 1: Basic configuration
        nlm::Config config;
        
        config.set("learning_rate", 0.001);
        config.set("batch_size", 32);
        config.set("epochs", 100);
        config.set("optimizer", "adam");
        
        std::cout << "\n1. Config created:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test 2: JSON serialization
        std::string json_str = config.toJson();
        std::cout << "\n2. JSON serialization:" << std::endl;
        std::cout << json_str << std::endl;
        
        // Test 3: JSON deserialization
        nlm::Config config_from_json;
        config_from_json.fromJson(json_str);
        
        std::cout << "\n3. Config loaded from JSON:" << std::endl;
        std::cout << config_from_json.summary() << std::endl;
        
        // Test 4: File format detection
        std::string json_format = nlm::Config::detectFileFormat("config.json");
        std::string yaml_format = nlm::Config::detectFileFormat("config.yaml");
        std::string unknown_format = nlm::Config::detectFileFormat("config.txt");
        
        std::cout << "\n4. File format detection:" << std::endl;
        std::cout << "   config.json -> " << json_format << std::endl;
        std::cout << "   config.yaml -> " << yaml_format << std::endl;
        std::cout << "   config.txt -> " << unknown_format << std::endl;
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}