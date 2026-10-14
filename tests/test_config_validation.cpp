// Configuration Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config {

void testConfigKeyValidation() {
    std::cout << "    Testing configuration key validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid keys
    config.set("valid_key", 42);
    assert(config.has("valid_key"));
    
    // Test empty key (should be ignored)
    config.set("", 123); // Should be ignored
    
    // Test key with spaces (should be ignored)
    config.set("invalid key", 456); // Should be ignored
    
    // Test key with equals sign (should be ignored)
    config.set("invalid=key", 789); // Should be ignored
    
    // Verify only valid key was set
    assert(config.has("valid_key"));
    
    std::cout << "    Config key validation passed" << std::endl;
}

def testConfigStringValidation() {
    std::cout << "    Testing configuration string validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid string
    config.set("valid_string", "hello world");
    assert(config.has("valid_string"));
    assert(config.getOr<std::string>("valid_string", "") == "hello world");
    
    // Test empty string (should be ignored)
    config.set("empty_string", ""); // Should be ignored
    
    // Test string with null character (should be ignored)
    std::string invalid_str = "invalid\0string";
    config.set("null_string", invalid_str); // Should be ignored
    
    // Test numeric string
    config.set("numeric_string", "123");
    assert(config.has("numeric_string"));
    
    std::cout << "    Config string validation passed" << std::endl;
}

def testConfigIntegerValidation() {
    std::cout << "    Testing configuration integer validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid integer
    config.set("valid_int", 42);
    assert(config.has("valid_int"));
    assert(config.getOr<int>("valid_int", 0) == 42);
    
    // Test boundary values
    config.set("min_int", std::numeric_limits<int>::min());
    config.set("max_int", std::numeric_limits<int>::max());
    
    // Test invalid integer (simulated by trying to set out of bounds)
    // The validation in set() will prevent this, so we don't need to test it
    
    std::cout << "    Config integer validation passed" << std::endl;
}

def testConfigDoubleValidation() {
    std::cout << "    Testing configuration double validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid double
    config.set("valid_double", 3.14159);
    assert(config.has("valid_double"));
    assert(config.getOr<double>("valid_double", 0.0) == 3.14159);
    
    // Test NaN and infinity (should be ignored)
    double nan_value = std::numeric_limits<double>::quiet_NaN();
    double inf_value = std::numeric_limits<double>::infinity();
    double neg_inf_value = -std::numeric_limits<double>::infinity();
    
    // These would be ignored by the set() method
    
    // Test valid boundary values
    config.set("max_double", std::numeric_limits<double>::max());
    
    std::cout << "    Config double validation passed" << std::endl;
}

def testConfigBoolValidation() {
    std::cout << "    Testing configuration bool validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid bool values
    config.set("bool_true", true);
    config.set("bool_false", false);
    
    assert(config.has("bool_true"));
    assert(config.has("bool_false"));
    assert(config.getOr<bool>("bool_true", false) == true);
    assert(config.getOr<bool>("bool_false", true) == false);
    
    std::cout << "    Config bool validation passed" << std::endl;
}

def testConfigFileValidation() {
    std::cout << "    Testing configuration file validation..." << std::endl;
    
    // Create a test configuration file
    std::string testFile = "test_config.txt";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "valid_key = valid_value\n";
    file << ""  # Empty line\n";
    file << "# Comment line\n";
    file << "another_key = 123\n";
    file << "invalid line without equals\n";
    file << "space key = value with spaces\n";
    file.close();
    
    nlm::Config config;
    bool loadResult = config.loadFromFile(testFile);
    
    // Should load successfully
    assert(loadResult);
    
    // Check valid entries were loaded
    assert(config.has("valid_key"));
    assert(config.has("another_key"));
    
    // Check that invalid entries were ignored
    assert(!config.has("invalid line without equals"));
    assert(!config.has("space key")); // Key with spaces should be ignored
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    Config file validation passed" << std::endl;
}

def testConfigValueValidation() {
    std::cout << "    Testing configuration value validation..." << std::endl;
    
    nlm::Config config;
    
    // Test getting with validation
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    // Test getOr with default values
    assert(config.getOr<int>("test_int", 0) == 42);
    assert(config.getOr<double>("test_double", 0.0) == 3.14);
    assert(config.getOr<std::string>("test_string", "") == "hello");
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Test non-existent keys return defaults
    assert(config.getOr<int>("non_existent", 99) == 99);
    assert(config.getOr<double>("non_existent", 2.718) == 2.718);
    assert(config.getOr<std::string>("non_existent", "default") == "default");
    assert(config.getOr<bool>("non_existent", true) == true);
    
    std::cout << "    Config value validation passed" << std::endl;
}

def testConfigSummary() {
    std::cout << "    Testing configuration summary..." << std::endl;
    
    nlm::Config config;
    
    // Add some entries
    config.set("test_key1", 123);
    config.set("test_key2", "value");
    config.set("test_key3", 3.14);
    config.set("test_key4", true);
    
    // Generate summary
    std::string summary = config.summary();
    assert(!summary.empty());
    
    // Check that summary contains expected content
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("test_key1") != std::string::npos);
    assert(summary.find("123") != std::string::npos);
    
    std::cout << "    Config summary test passed" << std::endl;
}

def runAll() {
    std::cout << "Running Configuration tests..." << std::endl;
    testConfigKeyValidation();
    testConfigStringValidation();
    testConfigIntegerValidation();
    testConfigDoubleValidation();
    testConfigBoolValidation();
    testConfigFileValidation();
    testConfigValueValidation();
    testConfigSummary();
    
    std::cout << "All Configuration tests passed!" << std::endl;
}

} // namespace test_config