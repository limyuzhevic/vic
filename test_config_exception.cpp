#include <iostream>
#include <cassert>
#include <string>
#include <fstream>
#include <filesystem>
#include "core/Config/Config.hpp"

void testConfigExceptionHandling() {
    std::cout << "Testing Config exception handling..." << std::endl;
    
    nlm::Config config;
    
    // Test 1: Empty key validation
    std::cout << "Test 1: Empty key validation..." << std::endl;
    try {
        config.set("", "value");
        std::cout << "ERROR: Should have thrown ConfigValidationException for empty key" << std::endl;
        assert(false);
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "✓ Caught ConfigValidationException: " << e.what() << std::endl;
    }
    
    // Test 2: Invalid key pattern
    std::cout << "\nTest 2: Invalid key pattern validation..." << std::endl;
    try {
        config.set("123invalid", "value");
        std::cout << "ERROR: Should have thrown ConfigValidationException for invalid key pattern" << std::endl;
        assert(false);
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "✓ Caught ConfigValidationException: " << e.what() << std::endl;
    }
    
    // Test 3: Valid key setting
    std::cout << "\nTest 3: Valid key setting..." << std::endl;
    try {
        config.set("valid.key", "123");
        std::cout << "✓ Successfully set valid key" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ERROR: Should not have thrown exception for valid key: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test 4: Key existence check
    std::cout << "\nTest 4: Key existence check..." << std::endl;
    if (config.has("valid.key")) {
        std::cout << "✓ Valid key exists" << std::endl;
    } else {
        std::cout << "ERROR: Valid key should exist" << std::endl;
        assert(false);
    }
    
    if (!config.has("nonexistent.key")) {
        std::cout << "✓ Non-existent key correctly identified" << std::endl;
    } else {
        std::cout << "ERROR: Non-existent key should not exist" << std::endl;
        assert(false);
    }
    
    // Test 5: Error message access
    std::cout << "\nTest 5: Error message access..." << std::endl;
    std::string errorMsg = nlm::Config::getLastError();
    if (errorMsg.empty()) {
        std::cout << "✓ No error message (as expected after successful operations)" << std::endl;
    } else {
        std::cout << "Note: Current error message: " << errorMsg << std::endl;
    }
    
    // Test 6: File loading error (non-existent file)
    std::cout << "\nTest 6: File loading error..." << std::endl;
    if (!config.loadFromFile("/nonexistent/file.txt")) {
        std::cout << "✓ loadFromFile correctly returned false for non-existent file" << std::endl;
        std::cout << "  Error message: " << nlm::Config::getLastError() << std::endl;
    } else {
        std::cout << "ERROR: Should have returned false for non-existent file" << std::endl;
        assert(false);
    }
    
    // Test 7: Save to file error (directory that doesn't exist)
    std::cout << "\nTest 7: Save to file error..." << std::endl;
    if (!config.saveToFile("/nonexistent_dir/config.txt")) {
        std::cout << "✓ saveToFile correctly returned false for non-existent directory" << std::endl;
        std::cout << "  Error message: " << nlm::Config::getLastError() << std::endl;
    } else {
        std::cout << "ERROR: Should have returned false for non-existent directory" << std::endl;
        assert(false);
    }
    
    // Test 8: Type validation
    std::cout << "\nTest 8: Type validation..." << std::endl;
    config.set("int_value", 42);
    auto intVal = config.get<int>("int_value");
    if (intVal.has_value() && *intVal == 42) {
        std::cout << "✓ Integer value correctly retrieved" << std::endl;
    } else {
        std::cout << "ERROR: Integer value not correctly retrieved" << std::endl;
        assert(false);
    }
    
    // Test 9: Default value retrieval
    std::cout << "\nTest 9: Default value retrieval..." << std::endl;
    auto missingVal = config.get<std::string>("missing.key");
    if (!missingVal.has_value()) {
        std::cout << "✓ Missing key correctly returns nullopt" << std::endl;
    } else {
        std::cout << "ERROR: Missing key should return nullopt" << std::endl;
        assert(false);
    }
    
    std::string defaultVal = config.getOr<std::string>("missing.key", "default_value");
    if (defaultVal == "default_value") {
        std::cout << "✓ Default value correctly retrieved for missing key" << std::endl;
    } else {
        std::cout << "ERROR: Default value not correctly retrieved" << std::endl;
        assert(false);
    }
    
    // Test 10: Configuration validation
    std::cout << "\nTest 10: Configuration validation..." << std::endl;
    nlm::Config config2;
    config2.set("brain.neuron_count", 100);
    config2.set("brain.synapse_density", 0.5);
    config2.set("brain.v_thresh", -60.0);
    
    try {
        config2.validateConfig();
        std::cout << "✓ Valid configuration passes validation" << std::endl;
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "ERROR: Valid configuration should not throw: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test 11: Invalid configuration validation
    std::cout << "\nTest 11: Invalid configuration validation..." << std::endl;
    nlm::Config config3;
    config3.set("brain.neuron_count", -50); // Invalid: must be positive
    
    try {
        config3.validateConfig();
        std::cout << "ERROR: Invalid configuration should have thrown exception" << std::endl;
        assert(false);
    } catch (const nlm::ConfigValidationException& e) {
        std::cout << "✓ Invalid configuration correctly throws ConfigValidationException: " << e.what() << std::endl;
    }
    
    // Test 12: Remove operation
    std::cout << "\nTest 12: Remove operation..." << std::endl;
    config.set("test_to_remove", "value");
    if (config.has("test_to_remove")) {
        config.remove("test_to_remove");
        if (!config.has("test_to_remove")) {
            std::cout << "✓ Key successfully removed" << std::endl;
        } else {
            std::cout << "ERROR: Key should have been removed" << std::endl;
            assert(false);
        }
    } else {
        std::cout << "ERROR: Key to remove should exist" << std::endl;
        assert(false);
    }
    
    // Test 13: Clear operation
    std::cout << "\nTest 13: Clear operation..." << std::endl;
    config.clear();
    if (config.getKeys().empty()) {
        std::cout << "✓ Configuration successfully cleared" << std::endl;
    } else {
        std::cout << "ERROR: Configuration should be empty after clear" << std::endl;
        assert(false);
    }
    
    std::cout << "\n✓ All exception handling tests passed!" << std::endl;
}

int main() {
    testConfigExceptionHandling();
    return 0;
}
