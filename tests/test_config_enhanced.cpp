// Enhanced Config Error Handling Tests
// Tests for improved error handling in the NLM Config class

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config_enhanced {

void testConfigFileErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool result = config.loadFromFile("/nonexistent/path/config.cfg");
    assert(!result); // Should fail
    
    std::cout << "    testConfigFileErrorHandling passed" << std::endl;
}

void testConfigCommandLineErrorHandling() {
    nlm::Config config;
    
    // Create dummy argv
    const char* argv[] = {
        "test_program",
        "--invalid_key",  // Missing value (should fail)
        "--valid_key=123",  // Valid
        "--another_key",   // Missing value (should fail)
        "--test=456"       // Valid
    };
    
    bool success = config.loadFromArgs(5, const_cast<char**>(argv));
    assert(success); // Should have some successes
    
    // Check that valid keys were processed
    assert(config.has("valid_key"));
    assert(config.has("test"));
    
    std::cout << "    testConfigCommandLineErrorHandling passed" << std::endl;
}

void testConfigSaveErrorHandling() {
    nlm::Config config;
    
    // Add some test data
    config.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    
    // Try to save to a directory that doesn't exist
    bool result = config.saveToFile("/nonexistent_dir/test.cfg");
    assert(!result); // Should fail
    
    std::cout << "    testConfigSaveErrorHandling passed" << std::endl;
}

void testConfigKeyValidation() {
    nlm::Config config;
    
    // Test invalid key names
    config.set("", "value", nlm::ConfigSource::Runtime);  // Empty key should be ignored
    config.set("key with spaces", "value", nlm::ConfigSource::Runtime);  // Spaces should be rejected
    config.set("key=equals", "value", nlm::ConfigSource::Runtime);  // Equals should be rejected
    
    // Valid key should still work
    config.set("valid_key", 123, nlm::ConfigSource::Runtime);
    assert(config.has("valid_key"));
    
    auto val = config.get<int>("valid_key");
    assert(val.has_value());
    assert(*val == 123);
    
    std::cout << "    testConfigKeyValidation passed" << std::endl;
}

void testConfigTypeErrorHandling() {
    nlm::Config config;
    
    // Set an integer value
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    
    // Try to get it as a double (should work)
    auto doubleVal = config.get<double>("int_value");
    assert(doubleVal.has_value());
    assert(*doubleVal == 42.0);
    
    // Try to get it as a string (should work)
    auto stringVal = config.get<std::string>("int_value");
    assert(stringVal.has_value());
    assert(*stringVal == "42");
    
    std::cout << "    testConfigTypeErrorHandling passed" << std::endl;
}

void testConfigGetOrDefaultHandling() {
    nlm::Config config;
    
    // Missing key should return default with warning
    int defaultVal = config.getOr<int>("missing_key", 999);
    assert(defaultVal == 999);
    
    // Existing key should return actual value
    config.set("existing_key", 123, nlm::ConfigSource::Runtime);
    int actualVal = config.getOr<int>("existing_key", 999);
    assert(actualVal == 123);
    
    std::cout << "    testConfigGetOrDefaultHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Config Tests..." << std::endl;
    
    testConfigFileErrorHandling();
    testConfigCommandLineErrorHandling();
    testConfigSaveErrorHandling();
    testConfigKeyValidation();
    testConfigTypeErrorHandling();
    testConfigGetOrDefaultHandling();
    
    std::cout << "=== All Enhanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config_enhanced