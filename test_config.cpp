// NLM Config Test Program
// Tests JSON/YAML parser, saveToFile(), validation, and error handling

#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

void testJsonLoad() {
    std::cout << "=== Test JSON Load ===" << std::endl;
    nlm::Config config;
    
    try {
        // Load from JSON file
        bool success = config.loadFromFile("configs/config.json");
        std::cout << "JSON load result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            // Check loaded values
            std::cout << "app_name: " << config.getOr<std::string>("app_name", "NOT FOUND") << std::endl;
            std::cout << "version: " << config.getOr<std::string>("version", "NOT FOUND") << std::endl;
            std::cout << "max_connections: " << config.getOr<int>("max_connections", -1) << std::endl;
            std::cout << "timeout: " << config.getOr<double>("timeout", -1.0) << std::endl;
            std::cout << "enable_logging: " << config.getOr<bool>("enable_logging", false) << std::endl;
            
            std::cout << "Validation result: " << (config.validate() ? "PASSED" : "FAILED") << std::endl;
            auto errors = config.getValidationErrors();
            if (!errors.empty()) {
                std::cout << "Validation errors:" << std::endl;
                for (const auto& error : errors) {
                    std::cout << "  - " << error << std::endl;
                }
            }
        }
    } catch (const nlm::ConfigError& e) {
        std::cout << "ConfigError: " << e.what() << std::endl;
    }
}

void testJsonSave() {
    std::cout << "\n=== Test JSON Save ===" << std::endl;
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42);
    config.set("test_string", std::string("Hello World"));
    config.set("test_double", 3.14159);
    config.set("test_bool", true);
    config.set("test_vector", std::vector<int>({1, 2, 3, 4, 5}));
    
    // Save to file
    std::string outputFile = "configs/test_output.json";
    try {
        bool success = config.saveToFile(outputFile);
        std::cout << "JSON save result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            // Read back and verify
            nlm::Config config2;
            config2.loadFromFile(outputFile);
            std::cout << "test_int: " << config2.getOr<int>("test_int", 0) << std::endl;
            std::cout << "test_string: " << config2.getOr<std::string>("test_string", "") << std::endl;
            std::cout << "test_double: " << config2.getOr<double>("test_double", 0.0) << std::endl;
            std::cout << "test_bool: " << config2.getOr<bool>("test_bool", false) << std::endl;
        }
        
        // Clean up
        std::filesystem::remove(outputFile);
    } catch (const nlm::ConfigError& e) {
        std::cout << "ConfigError: " << e.what() << std::endl;
    }
}

void testJsonStringLoad() {
    std::cout << "\n=== Test JSON String Load ===" << std::endl;
    nlm::Config config;
    
    try {
        std::string jsonString = R"({"from_string": true, "value": 123, "name": "Test"})";
        bool success = config.loadFromJsonString(jsonString);
        std::cout << "JSON string load result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            std::cout << "from_string: " << config.getOr<bool>("from_string", false) << std::endl;
            std::cout << "value: " << config.getOr<int>("value", 0) << std::endl;
            std::cout << "name: " << config.getOr<std::string>("name", "") << std::endl;
        }
    } catch (const nlm::ConfigError& e) {
        std::cout << "ConfigError: " << e.what() << std::endl;
    }
}

void testErrorHandling() {
    std::cout << "\n=== Test Error Handling ===" << std::endl;
    
    // Test loading non-existent file
    nlm::Config config;
    try {
        config.loadFromFile("configs/nonexistent.json");
        std::cout << "ERROR: Should have thrown ConfigError for non-existent file" << std::endl;
    } catch (const nlm::ConfigError& e) {
        std::cout << "Expected ConfigError caught: " << e.what() << std::endl;
    }
    
    // Test saving to invalid directory (without creating parent)
    try {
        // Try to save to a path in a non-existent subdirectory without creating it
        std::string invalidPath = "configs/invalid/subdir/config.json";
        bool success = config.saveToFile(invalidPath);
        std::cout << "Save to invalid path result: " << (success ? "UNEXPECTED SUCCESS" : "Expected failure") << std::endl;
    } catch (const nlm::ConfigError& e) {
        std::cout << "Expected ConfigError caught: " << e.what() << std::endl;
    }
}

void testBackwardCompatibility() {
    std::cout << "\n=== Test Backward Compatibility (Simple Key=Value) ===" << std::endl;
    nlm::Config config;
    
    try {
        // Save simple key=value format file
        std::ofstream file("configs/simple_config.txt");
        file << "# Simple config file\n";
        file << "key1 = value1\n";
        file << "key2 = 42\n";
        file << "key3 = true\n";
        file << "# Another comment\n";
        file << "key4 = \"quoted value\"\n";
        file.close();
        
        // Load using simple parser
        bool success = config.loadFromFileSimple("configs/simple_config.txt");
        std::cout << "Simple load result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
        
        if (success) {
            std::cout << "key1: " << config.getOr<std::string>("key1", "") << std::endl;
            std::cout << "key2: " << config.getOr<int>("key2", 0) << std::endl;
            std::cout << "key3: " << config.getOr<bool>("key3", false) << std::endl;
            std::cout << "key4: " << config.getOr<std::string>("key4", "") << std::endl;
        }
        
        // Clean up
        std::filesystem::remove("configs/simple_config.txt");
    } catch (const nlm::ConfigError& e) {
        std::cout << "ConfigError: " << e.what() << std::endl;
    }
}

void testValidation() {
    std::cout << "\n=== Test Validation ===" << std::endl;
    nlm::Config config;
    
    // Add missing required keys
    config.set("app_name", std::string("NLM Test"));
    config.set("version", std::string("1.0.0"));
    config.set("max_connections", 50);
    config.set("timeout", 10.0);
    
    bool valid = config.validate();
    std::cout << "With required keys: " << (valid ? "VALID" : "INVALID") << std::endl;
    
    // Try invalid values
    config.set("max_connections", 2000); // Too high
    valid = config.validate();
    std::cout << "With invalid max_connections: " << (valid ? "VALID" : "INVALID") << std::endl;
    
    auto errors = config.getValidationErrors();
    std::cout << "Validation errors:" << std::endl;
    for (const auto& error : errors) {
        std::cout << "  - " << error << std::endl;
    }
}

int main() {
    std::cout << "=== NLM Configuration Tests ===" << std::endl;
    
    testJsonLoad();
    testJsonSave();
    testJsonStringLoad();
    testErrorHandling();
    testBackwardCompatibility();
    testValidation();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    return 0;
}
