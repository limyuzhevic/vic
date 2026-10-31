// Simple test to verify JSON/YAML fallback implementations
// This tests the core Config class functionality with our fallbacks

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing NLM Config with fallback JSON/YAML implementations..." << std::endl;
    
    // Create a Config instance
    auto config = std::make_shared<nlm::Config>();
    
    // Test basic configuration operations
    std::cout << "Setting basic values..." << std::endl;
    config->set("test_int", 42);
    config->set("test_string", "Hello World");
    config->set("test_double", 3.14);
    config->set("test_bool", true);
    
    // Test JSON serialization
    std::cout << "Testing JSON serialization..." << std::endl;
    if (config->saveToJSON("/tmp/test_config.json")) {
        std::cout << "✓ Successfully saved to JSON" << std::endl;
        
        // Display JSON content
        std::ifstream json_file("/tmp/test_config.json");
        if (json_file.is_open()) {
            std::string line;
            while (std::getline(json_file, line)) {
                std::cout << "  " << line << std::endl;
            }
            json_file.close();
        }
    } else {
        std::cout << "✗ Failed to save to JSON" << std::endl;
        return 1;
    }
    
    // Test YAML serialization
    std::cout << "Testing YAML serialization..." << std::endl;
    if (config->saveToYAML("/tmp/test_config.yaml")) {
        std::cout << "✓ Successfully saved to YAML" << std::endl;
        
        // Display YAML content
        std::ifstream yaml_file("/tmp/test_config.yaml");
        if (yaml_file.is_open()) {
            std::string line;
            while (std::getline(yaml_file, line)) {
                std::cout << "  " << line << std::endl;
            }
            yaml_file.close();
        }
    } else {
        std::cout << "✗ Failed to save to YAML" << std::endl;
        return 1;
    }
    
    // Test loading from JSON
    std::cout << "Testing JSON loading..." << std::endl;
    auto config2 = std::make_shared<nlm::Config>();
    if (config2->loadFromJSON("/tmp/test_config.json")) {
        std::cout << "✓ Successfully loaded from JSON" << std::endl;
        std::cout << "  Keys: ";
        auto keys = config2->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << std::endl;
    } else {
        std::cout << "✗ Failed to load from JSON" << std::endl;
        return 1;
    }
    
    // Test loading from YAML
    std::cout << "Testing YAML loading..." << std::endl;
    auto config3 = std::make_shared<nlm::Config>();
    if (config3->loadFromYAML("/tmp/test_config.yaml")) {
        std::cout << "✓ Successfully loaded from YAML" << std::endl;
        std::cout << "  Keys: ";
        auto keys = config3->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << keys[i];
        }
        std::cout << std::endl;
    } else {
        std::cout << "✗ Failed to load from YAML" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== All Tests Passed ===" << std::endl;
    std::cout << "The fallback JSON/YAML implementations are working correctly!" << std::endl;
    
    // Clean up
    std::remove("/tmp/test_config.json");
    std::remove("/tmp/test_config.yaml");
    
    return 0;
}
