#include "core/Config/Config.hpp"
#include <iostream>

void testConfigSystem() {
    std::cout << "=== Testing Enhanced Config System ===" << std::endl;
    
    // Test 1: Default config
    nlm::Config config;
    config.set("test.int", 42);
    config.set("test.string", "Hello World");
    config.set("test.double", 3.14159);
    config.set("test.bool", true);
    
    std::cout << "Test 1 - Set basic values:" << std::endl;
    std::cout << "  int: " << config.getOr<int>("test.int", 0) << std::endl;
    std::cout << "  string: " << config.getOr<std::string>("test.string", "") << std::endl;
    std::cout << "  double: " << config.getOr<double>("test.double", 0.0) << std::endl;
    std::cout << "  bool: " << (config.getOr<bool>("test.bool", false) ? "true" : "false") << std::endl;
    
    // Test 2: JSON serialization
    std::string jsonStr = config.toJson();
    std::cout << "Test 2 - JSON serialization:" << std::endl;
    std::cout << "  Generated JSON:" << std::endl;
    std::cout << "  " << jsonStr << std::endl;
    
    // Test 3: YAML serialization
    std::string yamlStr = config.toYaml();
    std::cout << "Test 3 - YAML serialization:" << std::endl;
    std::cout << "  Generated YAML:" << std::endl;
    std::cout << "  " << yamlStr << std::endl;
    
    // Test 4: File operations (temporary files)
    std::string jsonFile = "test_config.json";
    std::string yamlFile = "test_config.yaml";
    
    std::cout << "Test 4 - File operations:" << std::endl;
    
    // Save as JSON
    if (config.saveToFile(jsonFile)) {
        std::cout << "  ✓ Saved to JSON file: " << jsonFile << std::endl;
        
        // Load from JSON
        nlm::Config configFromJson;
        if (configFromJson.loadFromFile(jsonFile)) {
            std::cout << "  ✓ Loaded from JSON file" << std::endl;
            std::cout << "  int value: " << configFromJson.getOr<int>("test.int", 0) << std::endl;
        } else {
            std::cout << "  ✗ Failed to load from JSON file" << std::endl;
        }
    } else {
        std::cout << "  ✗ Failed to save to JSON file" << std::endl;
    }
    
    // Save as YAML
    if (config.saveToFile(yamlFile)) {
        std::cout << "  ✓ Saved to YAML file: " << yamlFile << std::endl;
        
        // Load from YAML
        nlm::Config configFromYaml;
        if (configFromYaml.loadFromFile(yamlFile)) {
            std::cout << "  ✓ Loaded from YAML file" << std::endl;
            std::cout << "  string value: " << configFromYaml.getOr<std::string>("test.string", "") << std::endl;
        } else {
            std::cout << "  ✗ Failed to load from YAML file" << std::endl;
        }
    } else {
        std::cout << "  ✗ Failed to save to YAML file" << std::endl;
    }
    
    // Test 5: Command line argument parsing
    std::cout << "Test 5 - Command line argument parsing:" << std::endl;
    int argc = 3;
    char* argv[] = {
        (char*)"test",
        (char*)"--config.path=/tmp/test",
        (char*)"-debug=true"
    };
    
    nlm::Config configFromArgs;
    configFromArgs.loadFromArgs(argc, argv);
    
    if (configFromArgs.has("config.path")) {
        std::cout << "  ✓ Parsed --config.path: " << configFromArgs.getOr<std::string>("config.path", "") << std::endl;
    }
    
    if (configFromArgs.has("debug")) {
        std::cout << "  ✓ Parsed -debug: " << (configFromArgs.getOr<bool>("debug", false) ? "true" : "false") << std::endl;
    }
    
    // Test 6: Validation
    std::cout << "Test 6 - Configuration validation:" << std::endl;
    bool valid = config.validate({"test.int", "test.string"});
    std::cout << "  Validation result: " << (valid ? "✓ Valid" : "✗ Invalid") << std::endl;
    
    // Test 7: Merging configurations
    std::cout << "Test 7 - Configuration merging:" << std::endl;
    nlm::Config config1;
    config1.set("a", 1);
    config1.set("b", 2);
    
    nlm::Config config2;
    config2.set("b", 20);  // This should override
    config2.set("c", 30);
    
    config1.merge(config2);
    std::cout << "  Merged config - a: " << config1.getOr<int>("a", 0) << std::endl;
    std::cout << "  Merged config - b: " << config1.getOr<int>("b", 0) << std::endl;  // Should be 20
    std::cout << "  Merged config - c: " << config1.getOr<int>("c", 0) << std::endl;  // Should be 30
    
    // Test 8: Supported formats
    std::cout << "Test 8 - Supported file formats:" << std::endl;
    auto formats = config.getSupportedFormats();
    std::cout << "  Supported formats: ";
    for (size_t i = 0; i < formats.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << formats[i];
    }
    std::cout << std::endl;
    
    std::cout << "=== All Config System Tests Completed ===" << std::endl;
}

int main() {
    testConfigSystem();
    return 0;
}
