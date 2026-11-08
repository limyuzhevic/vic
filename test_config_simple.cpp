#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== Testing Enhanced Config System ===" << std::endl;
    
    nlm::Config config;
    config.set("test.int", 42);
    config.set("test.string", "Hello World");
    config.set("test.double", 3.14159);
    config.set("test.bool", true);
    
    // Test basic get operations
    std::cout << "Test 1 - Set and get basic values:" << std::endl;
    std::cout << "  int: " << config.getOr<int>("test.int", 0) << std::endl;
    std::cout << "  string: " << config.getOr<std::string>("test.string", "") << std::endl;
    std::cout << "  double: " << config.getOr<double>("test.double", 0.0) << std::endl;
    std::cout << "  bool: " << (config.getOr<bool>("test.bool", false) ? "true" : "false") << std::endl;
    
    // Test JSON serialization
    std::string jsonStr = config.toJson();
    std::cout << "\nTest 2 - JSON serialization:" << std::endl;
    std::cout << "  Generated JSON:" << std::endl;
    std::cout << "  " << jsonStr << std::endl;
    
    // Test YAML serialization
    std::string yamlStr = config.toYaml();
    std::cout << "\nTest 3 - YAML serialization:" << std::endl;
    std::cout << "  Generated YAML:" << std::endl;
    std::cout << "  " << yamlStr << std::endl;
    
    // Test file operations
    std::cout << "\nTest 4 - File operations:" << std::endl;
    
    // Save as JSON
    if (config.saveToFile("test_config.json")) {
        std::cout << "  ✓ Saved to JSON file" << std::endl;
        
        // Load from JSON
        nlm::Config configFromJson;
        if (configFromJson.loadFromFile("test_config.json")) {
            std::cout << "  ✓ Loaded from JSON file" << std::endl;
            std::cout << "  int value: " << configFromJson.getOr<int>("test.int", 0) << std::endl;
        } else {
            std::cout << "  ✗ Failed to load from JSON file" << std::endl;
        }
    } else {
        std::cout << "  ✗ Failed to save to JSON file" << std::endl;
    }
    
    // Save as YAML
    if (config.saveToFile("test_config.yaml")) {
        std::cout << "  ✓ Saved to YAML file" << std::endl;
        
        // Load from YAML
        nlm::Config configFromYaml;
        if (configFromYaml.loadFromFile("test_config.yaml")) {
            std::cout << "  ✓ Loaded from YAML file" << std::endl;
            std::cout << "  string value: " << configFromYaml.getOr<std::string>("test.string", "") << std::endl;
        } else {
            std::cout << "  ✗ Failed to load from YAML file" << std::endl;
        }
    } else {
        std::cout << "  ✗ Failed to save to YAML file" << std::endl;
    }
    
    // Test supported formats
    std::cout << "\nTest 5 - Supported file formats:" << std::endl;
    auto formats = config.getSupportedFormats();
    std::cout << "  Supported formats: ";
    for (size_t i = 0; i < formats.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << formats[i];
    }
    std::cout << std::endl;
    
    // Clean up test files
    std::remove("test_config.json");
    std::remove("test_config.yaml");
    
    std::cout << "\n=== Config System Test Completed Successfully ===" << std::endl;
    return 0;
}
