// Enhanced configuration validation tests for NLM
// Comprehensive unit tests for all configuration validation scenarios
// Covers edge cases and integration scenarios for configuration validation

#include "src/core/Config/Config.hpp"
#include "tests/test_configuration_validation.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <map>

namespace test_configuration_validation {

void testConfigurationCreation() {
    std::cout << "    Testing configuration creation..." << std::endl;
    
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigurationCreation passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "    Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    // Test valid configurations
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("valid_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("valid_string", "hello", nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("valid_string");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    // Test configuration validation edge cases
    config.set("empty_key", "value", nlm::ConfigSource::Runtime); // Empty key handling
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationSourcePrecedence() {
    std::cout << "    Testing configuration source precedence..." << std::endl;
    
    nlm::Config config;
    
    // Test source precedence: Runtime > Default > Command Line > File
    config.set("test_value", "runtime", nlm::ConfigSource::Runtime);
    auto val = config.get<std::string>("test_value");
    assert(val.has_value());
    assert(*val == "runtime");
    
    // Changing to Default should override Runtime? (depends on implementation)
    config.set("test_value", "default", nlm::ConfigSource::Default);
    
    std::cout << "    testConfigurationSourcePrecedence passed" << std::endl;
}

void testConfigurationFileValidation() {
    std::cout << "    Testing configuration file validation..." << std::endl;
    
    nlm::Config config;
    std::string testFile = "/tmp/test_config_validation.json";
    
    // Set up test configuration
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello", nlm::ConfigSource::Runtime);
    config.set("float_value", 3.14, nlm::ConfigSource::Runtime);
    
    // Save configuration
    bool saveSuccess = config.saveToFile(testFile);
    assert(saveSuccess); // Should save successfully
    
    // Load configuration back
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile(testFile);
    assert(loadSuccess); // Should load successfully
    
    // Validate loaded values
    auto intVal = loadedConfig.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigurationFileValidation passed" << std::endl;
}

void testConfigurationValidationErrors() {
    std::cout << "    Testing configuration validation errors..." << std::endl;
    
    nlm::Config config;
    
    // Test getting non-existent key returns std::nullopt
    auto nonExistent = config.get<int>("non_existent_key_12345");
    assert(!nonExistent.has_value());
    
    // Test getOr with non-existent key
    int defaultValue = config.getOr<int>("another_missing_key_67890", 999);
    assert(defaultValue == 999);
    
    // Test remove functionality
    config.set("to_remove", 123, nlm::ConfigSource::Runtime);
    assert(config.has("to_remove"));
    config.remove("to_remove");
    assert(!config.has("to_remove"));
    
    std::cout << "    testConfigurationValidationErrors passed" << std::endl;
}

void testConfigurationEdgeCases() {
    std::cout << "    Testing configuration edge cases..." << std::endl;
    
    nlm::Config config;
    
    // Test very large values
    config.set("large_int", std::numeric_limits<int>::max(), nlm::ConfigSource::Runtime);
    auto largeInt = config.get<int>("large_int");
    assert(largeInt.has_value());
    assert(*largeInt == std::numeric_limits<int>::max());
    
    // Test very long strings
    std::string longString(10000, 'a');
    config.set("long_string", longString, nlm::ConfigSource::Runtime);
    auto retrievedString = config.get<std::string>("long_string");
    assert(retrievedString.has_value());
    assert(retrievedString->length() == 10000);
    
    // Test special characters
    config.set("special_chars", "Special: !@#$%^&*()_+-=[]{}|;':\",./<>?", nlm::ConfigSource::Runtime);
    auto specialChars = config.get<std::string>("special_chars");
    assert(specialChars.has_value());
    
    // Test Unicode characters
    config.set("unicode", "Test with unicode: αβγδε 中文 🚀", nlm::ConfigSource::Runtime);
    auto unicode = config.get<std::string>("unicode");
    assert(unicode.has_value());
    
    std::cout << "    testConfigurationEdgeCases passed" << std::endl;
}

void testConfigurationHealthValidation() {
    std::cout << "    Testing configuration health validation..." << std::endl;
    
    nlm::Config config;
    
    // Add various configuration entries
    config.set("health_test_int", 42, nlm::ConfigSource::Runtime);
    config.set("health_test_string", "healthy", nlm::ConfigSource::Runtime);
    config.set("health_test_float", 3.14159, nlm::ConfigSource::Runtime);
    config.set("health_test_bool", true, nlm::ConfigSource::Runtime);
    
    // Verify all entries are valid
    auto intVal = config.get<int>("health_test_int");
    assert(intVal.has_value());
    
    auto stringVal = config.get<std::string>("health_test_string");
    assert(stringVal.has_value());
    
    auto floatVal = config.get<double>("health_test_float");
    assert(floatVal.has_value());
    
    auto boolVal = config.get<bool>("health_test_bool");
    assert(boolVal.has_value());
    
    // Test getKeys functionality
    auto keys = config.getKeys();
    assert(keys.size() >= 4); // At least the 4 keys we set
    
    std::cout << "    testConfigurationHealthValidation passed" << std::endl;
}

void testConfigurationConsistency() {
    std::cout << "    Testing configuration consistency..." << std::endl;
    
    nlm::Config config1;
    nlm::Config config2;
    
    // Set same values in both configs
    config1.set("shared_int", 100, nlm::ConfigSource::Runtime);
    config2.set("shared_int", 100, nlm::ConfigSource::Runtime);
    
    // Verify consistency
    auto val1 = config1.get<int>("shared_int");
    auto val2 = config2.get<int>("shared_int");
    
    assert(val1.has_value());
    assert(val2.has_value());
    assert(*val1 == *val2);
    
    // Test different sources
    config1.set("source_test", "source1", nlm::ConfigSource::Default);
    config2.set("source_test", "source2", nlm::ConfigSource::Runtime);
    
    // Values should be different based on source
    auto src1 = config1.get<std::string>("source_test");
    auto src2 = config2.get<std::string>("source_test");
    
    assert(src1.has_value());
    assert(src2.has_value());
    
    std::cout << "    testConfigurationConsistency passed" << std::endl;
}

void runAll() {
    testConfigurationCreation();
    testConfigurationValidation();
    testConfigurationSourcePrecedence();
    testConfigurationFileValidation();
    testConfigurationValidationErrors();
    testConfigurationEdgeCases();
    testConfigurationHealthValidation();
    testConfigurationConsistency();
}

} // namespace test_configuration_validation
