// Enhanced Config Tests
// Includes tests for the new Config functionality (environment variables, validation, etc.)

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Test for new validation features
void testConfigValidation() {
    nlm::Config config;
    
    // Test setValidated with valid value
    bool validator = [](const int& val) { return val > 0 && val < 100; };
    assert(config.setValidated("valid_value", 50, validator, nlm::ConfigSource::Runtime, "Valid test value"));
    assert(config.hasValidated("valid_value", validator));
    
    // Test setValidated with invalid value
    assert(!config.setValidated("invalid_value", -5, validator, nlm::ConfigSource::Runtime, "Invalid test value"));
    assert(!config.hasValidated("invalid_value", validator));
    
    // Test getValidated
    auto validResult = config.getValidated<int>("valid_value", 0, validator);
    assert(validResult.first == 50);
    assert(validResult.second == true);
    
    auto invalidResult = config.getValidated<int>("invalid_value", 0, validator);
    assert(invalidResult.first == 0); // Default value when key doesn't exist
    assert(invalidResult.second == true); // Default passes validator
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// Test for new environment variable support
void testConfigLoadFromEnv() {
    nlm::Config config;
    
    // Set environment variable
    setenv("NLM_TEST_ENV_VAR", "env_value", 1);
    
    // Load from environment
    bool loaded = config.loadFromEnv("NLM_TEST_ENV_VAR");
    assert(loaded);
    
    // Check that value was loaded
    assert(config.getOr<std::string>("test_env_var", "") == "env_value");
    
    // Test that NLM_ prefix is automatically removed
    assert(config.has("test_env_var"));
    
    // Clean up
    unsetenv("NLM_TEST_ENV_VAR");
    
    // Test non-existent environment variable
    bool notLoaded = config.loadFromEnv("NLM_NONEXISTENT_VAR");
    assert(!notLoaded);
    
    std::cout << "    testConfigLoadFromEnv passed" << std::endl;
}

// Test for new string parsing (boolean, numeric)
void testConfigStringParsing() {
    nlm::Config config;
    
    // Test boolean parsing
    config.set("bool_true", std::string("true"), nlm::ConfigSource::Runtime);
    auto boolTrue = config.get<bool>("bool_true");
    assert(boolTrue.has_value());
    assert(*boolTrue == true);
    
    config.set("bool_false", std::string("false"), nlm::ConfigSource::Runtime);
    auto boolFalse = config.get<bool>("bool_false");
    assert(boolFalse.has_value());
    assert(*boolFalse == false);
    
    // Test numeric parsing
    config.set("int_val", std::string("42"), nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_val", std::string("3.14"), nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value());
    assert(std::abs(*doubleVal - 3.14) < 0.001);
    
    // Test string parsing with quotes
    config.set("quoted_string", std::string("\"hello world\""), nlm::ConfigSource::Runtime);
    auto quotedStr = config.get<std::string>("quoted_string");
    assert(quotedStr.has_value());
    assert(*quotedStr == "hello world");
    
    std::cout << "    testConfigStringParsing passed" << std::endl;
}

// Test for new config saving functionality
void testConfigSaveToFile() {
    nlm::Config config;
    
    // Add some test entries with descriptions
    config.set("test1", 42, nlm::ConfigSource::File, "Test integer value");
    config.set("test2", std::string("hello"), nlm::ConfigSource::CommandLine, "Test string value");
    config.set("test3", 3.14, nlm::ConfigSource::Runtime, "Test double value");
    config.set("test4", true, nlm::ConfigSource::Default, "Test boolean value");
    
    // Save to a temporary file
    std::string tempFile = "/tmp/test_config.cfg";
    bool saved = config.saveToFile(tempFile);
    assert(saved);
    
    // Read the file back
    std::ifstream file(tempFile);
    assert(file.is_open());
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check that the file contains expected content
    assert(content.find("test1") != std::string::npos);
    assert(content.find("test2") != std::string::npos);
    assert(content.find("test3") != std::string::npos);
    assert(content.find("test4") != std::string::npos);
    assert(content.find("Test integer value") != std::string::npos);
    assert(content.find("Test string value") != std::string::npos);
    assert(content.find("Test double value") != std::string::npos);
    assert(content.find("Test boolean value") != std::string::npos);
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

// Test for new file loading with descriptions
void testConfigLoadFromFileWithDescriptions() {
    nlm::Config config;
    
    // Create a test config file with descriptions
    std::string testContent = R"(
# Test configuration file with descriptions
key1 = 42           # Integer value
key2 = "hello"      # String value with quotes
key3 = 3.14         # Double value
key4 = true         # Boolean value
key5 = "world"      # Another string
)";
    
    std::string tempFile = "/tmp/test_config_with_descriptions.cfg";
    
    // Write test file
    std::ofstream file(tempFile);
    file << testContent;
    file.close();
    
    // Load from file
    bool loaded = config.loadFromFile(tempFile);
    assert(loaded);
    
    // Check that keys were loaded
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    assert(config.has("key5"));
    
    // Check that values are correct
    assert(config.getOr<int>("key1", 0) == 42);
    assert(config.getOr<std::string>("key2", "") == "hello");
    assert(std::abs(config.getOr<double>("key3", 0.0) - 3.14) < 0.001);
    assert(config.getOr<bool>("key4", false) == true);
    assert(config.getOr<std::string>("key5", "") == "world");
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigLoadFromFileWithDescriptions passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigLoadFromEnv();
    testConfigStringParsing();
    testConfigSaveToFile();
    testConfigLoadFromFileWithDescriptions();
}

} // namespace test_config
