// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

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

// Test new enhanced features
void testEnvironmentVariableInterpolation() {
    nlm::Config config;
    
    // Set environment variable
    setenv("TEST_VAR", "interpolated_value", 1);
    
    config.set("key1", "${TEST_VAR}", nlm::ConfigSource::Runtime);
    assert(config.has("key1"));
    
    auto val = config.get<std::string>("key1");
    assert(val.has_value());
    assert(*val == "interpolated_value");
    
    // Clean up
    unsetenv("TEST_VAR");
    
    std::cout << "    testEnvironmentVariableInterpolation passed" << std::endl;
}

void testSchemaValidation() {
    nlm::Config config;
    
    // Valid values should pass validation
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("valid_string", "hello", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    assert(config.validateSchema());
    
    // Invalid values should still pass basic validation
    config.set("invalid_string", "not_a_number", nlm::ConfigSource::Runtime);
    // The validation should handle this gracefully
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testConfigurationInheritance() {
    nlm::Config config;
    
    // Add inheritance sources
    config.addInheritanceSource("Default");
    config.addInheritanceSource("File");
    config.addInheritanceSource("Environment");
    
    assert(config.pImpl->inheritanceChain.size() == 3);
    
    // Remove one source
    config.removeInheritanceSource("File");
    assert(config.pImpl->inheritanceChain.size() == 2);
    
    // Clear all sources
    config.clearInheritanceChain();
    assert(config.pImpl->inheritanceChain.empty());
    
    std::cout << "    testConfigurationInheritance passed" << std::endl;
}

void testConfigurationMerging() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", "value1", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", "value2", nlm::ConfigSource::Runtime);
    config2.set("key3", 3.0, nlm::ConfigSource::Runtime);
    
    // Merge config2 into config1
    assert(config1.mergeFrom(config2));
    
    // Check that all keys are present
    assert(config1.has("key1"));
    assert(config1.has("key2"));
    assert(config1.has("key3"));
    
    // Check values (key2 should have been overwritten)
    auto val2 = config1.get<std::string>("key2");
    assert(val2.has_value());
    assert(*val2 == "value2");
    
    std::cout << "    testConfigurationMerging passed" << std::endl;
}

void testConvenienceMethods() {
    nlm::Config config;
    
    // Test convenience setters
    config.setString("str_key", "string_value", nlm::ConfigSource::Runtime);
    config.setInt("int_key", 42, nlm::ConfigSource::Runtime);
    config.setDouble("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.setBool("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test convenience getters
    assert(config.getString("str_key") == "string_value");
    assert(config.getInt("int_key") == 42);
    assert(config.getDouble("double_key") == 3.14);
    assert(config.getBool("bool_key") == true);
    
    std::cout << "    testConvenienceMethods passed" << std::endl;
}

void testFileIOWithJSON() {
    nlm::Config config;
    
    // Set some values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("str_key", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    assert(config.saveToFile("test_config.json"));
    
    // Create a new config and load from JSON
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    // Verify values
    assert(config2.has("int_key"));
    assert(config2.has("str_key"));
    assert(config2.has("bool_key"));
    assert(config2.has("double_key"));
    
    auto intVal = config2.get<int>("int_key");
    assert(intVal.has_value() && *intVal == 42);
    
    auto strVal = config2.get<std::string>("str_key");
    assert(strVal.has_value() && *strVal == "hello");
    
    // Clean up
    std::remove("test_config.json");
    
    std::cout << "    testFileIOWithJSON passed" << std::endl;
}

void testHealthChecking() {
    nlm::Config config;
    
    // Initially should be healthy
    assert(config.isHealthy());
    
    // Set some values
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::Runtime);
    
    // Should still be healthy
    assert(config.isHealthy());
    
    // Get health report
    std::string healthReport = config.getHealthReport();
    assert(!healthReport.empty());
    assert(healthReport.find("HEALTHY") != std::string::npos);
    
    std::cout << "    testHealthChecking passed" << std::endl;
}

void testStatistics() {
    nlm::Config config;
    
    // Add various values
    config.set("int1", 1, nlm::ConfigSource::Runtime);
    config.set("int2", 2, nlm::ConfigSource::File);
    config.set("str", "value", nlm::ConfigSource::CommandLine);
    config.set("bool", true, nlm::ConfigSource::Default);
    
    auto stats = config.getStats();
    assert(stats.totalEntries == 4);
    assert(stats.entriesBySource[0] == 1); // Default
    assert(stats.entriesBySource[1] == 1); // File
    assert(stats.entriesBySource[2] == 1); // CommandLine
    assert(stats.entriesBySource[3] == 1); // Runtime
    
    std::cout << "    testStatistics passed" << std::endl;
}

void testSerialization() {
    nlm::Config config;
    
    // Set some values
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("str_key", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Serialize to JSON
    std::string jsonStr = config.toJSON();
    assert(!jsonStr.empty());
    
    // Deserialize from JSON
    nlm::Config config2;
    assert(config2.fromJSON(jsonStr));
    
    // Verify values
    assert(config2.has("int_key"));
    assert(config2.has("str_key"));
    assert(config2.has("bool_key"));
    
    auto intVal = config2.get<int>("int_key");
    assert(intVal.has_value() && *intVal == 42);
    
    auto strVal = config2.get<std::string>("str_key");
    assert(strVal.has_value() && *strVal == "hello");
    
    std::cout << "    testSerialization passed" << std::endl;
}

void testBatchOperations() {
    nlm::Config config;
    
    // Set multiple values using batch operations
    std::map<std::string, nlm::Config::Impl::ConfigValue> values;
    values["key1"] = 1;
    values["key2"] = std::string("value1");
    values["key3"] = 3.14;
    
    config.setMultiple(values, nlm::ConfigSource::Runtime);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    
    // Remove multiple keys
    std::vector<std::string> keysToRemove = {"key1", "key2"};
    config.removeMultiple(keysToRemove);
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(config.has("key3"));
    
    std::cout << "    testBatchOperations passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    
    // New tests for enhanced features
    testEnvironmentVariableInterpolation();
    testSchemaValidation();
    testConfigurationInheritance();
    testConfigurationMerging();
    testConvenienceMethods();
    testFileIOWithJSON();
    testHealthChecking();
    testStatistics();
    testSerialization();
    testBatchOperations();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config
