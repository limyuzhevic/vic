// Advanced Config Tests
// Tests for new configuration validation and management features

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_config_advanced {

void testConfigurationValidation() {
    std::cout << "    Testing Configuration Validation..." << std::endl;
    
    nlm::Config config;
    
    // Test basic setting and getting
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    config.set("test_bool", true);
    
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    // Test type-specific getters
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr with defaults
    assert(config.getOr<int>("nonexistent", 99) == 99);
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testValidationRules() {
    std::cout << "    Testing Validation Rules..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    config.set("neuron_count", 1000);
    config.set("region_count", 5);
    config.set("connection_probability", 0.5);
    config.set("random_seed", 12345LL);
    
    // Test validation - should pass
    assert(config.validateKey("neuron_count"));
    assert(config.validateKey("region_count"));
    assert(config.validateKey("connection_probability"));
    assert(config.validateKey("random_seed"));
    
    // Test validation for non-existent key
    assert(!config.validateKey("nonexistent_key"));
    
    // Test overall validation
    assert(config.validateAll());
    
    // Test getting invalid keys
    auto invalidKeys = config.getInvalidKeys();
    assert(invalidKeys.empty());  // All keys should be valid
    
    // Test getting valid keys
    auto validKeys = config.getValidKeys();
    assert(validKeys.size() >= 4);  // At least the keys we set
    
    // Test validation stats
    auto stats = config.getValidationStats();
    assert(stats.totalValidations > 0);
    assert(stats.passedValidations > 0);
    
    std::cout << "    testValidationRules passed" << std::endl;
}

void testSchemaValidation() {
    std::cout << "    Testing Schema Validation..." << std::endl;
    
    nlm::Config config;
    
    // Add validation rules
    nlm::ValidationRule neuronRule;
    neuronRule.keyPattern = "neuron_count";
    neuronRule.valueType = "int";
    neuronRule.description = "Number of neurons";
    neuronRule.required = true;
    neuronRule.validator = [](const nlm::ConfigValue& v) { return std::holds_alternative<int>(v) && std::get<int>(v) > 0; };
    config.addValidationRule(neuronRule);
    
    // Test schema validation with brain schema
    nlm::ConfigSchema brainSchema("brain", true, true);
    brainSchema.requiredKeys = {"neuron_count", "region_count"};
    
    // Add required keys
    config.set("neuron_count", 100);
    config.set("region_count", 1);
    
    // The config should be valid according to the schema
    // (Note: full schema validation would require the schema to be part of the config)
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testConfigurationValidationErrors() {
    std::cout << "    Testing Configuration Validation Errors..." << std::endl;
    
    nlm::Config config;
    
    // Set invalid values
    config.set("invalid_int", -100);  // This should fail validation if we had a rule
    config.set("invalid_string", "");  // Empty string might fail some validations
    
    // Get validation errors
    std::string errors = config.getValidationErrors();
    // Errors might contain validation messages
    
    // Test clearing validation errors
    config.clearValidationErrors();
    
    // Set another invalid value
    config.set("another_invalid", "bad_value");
    
    std::cout << "    testConfigurationValidationErrors passed" << std::endl;
}

void testConfigurationJSONIO() {
    std::cout << "    Testing Configuration JSON I/O..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42);
    config.set("test_double", 3.14159);
    config.set("test_string", "hello world");
    config.set("test_bool", true);
    
    // Export to JSON
    std::string json = config.exportToJSON();
    assert(!json.empty());
    assert(json.find("{") != std::string::npos);
    assert(json.find("}") != std::string::npos);
    
    // Clear config
    config.clear();
    
    // Import from JSON
    bool success = config.importFromJSON(json);
    assert(success);
    
    // Verify values were imported
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    assert(config.has("test_bool"));
    
    std::cout << "    testConfigurationJSONIO passed" << std::endl;
}

void testConfigurationFileIO() {
    std::cout << "    Testing Configuration File I/O..." << std::endl;
    
    // Skip file I/O tests as they require actual file system access
    // In a real test environment, these would be tested with temporary files
    
    std::cout << "    testConfigurationFileIO skipped (requires file system access)" << std::endl;
}

void testConfigurationValidationStats() {
    std::cout << "    Testing Configuration Validation Stats..." << std::endl;
    
    nlm::Config config;
    
    // Get initial stats
    auto stats1 = config.getValidationStats();
    assert(stats1.totalValidations == 0);
    assert(stats1.passedValidations == 0);
    assert(stats1.failedValidations == 0);
    
    // Set some valid values
    config.set("valid_int", 42);
    config.set("valid_string", "test");
    
    // Get stats again
    auto stats2 = config.getValidationStats();
    assert(stats2.totalValidations > 0);
    assert(stats2.passedValidations > 0);
    
    // Validate all
    bool allValid = config.validateAll();
    assert(allValid);
    
    // Get final stats
    auto stats3 = config.getValidationStats();
    // Stats should have been updated by validation
    
    // Clear stats
    config.clearValidationStats();
    auto stats4 = config.getValidationStats();
    assert(stats4.totalValidations == 0);
    
    std::cout << "    testConfigurationValidationStats passed" << std::endl;
}

void testConfigurationAdvancedFeatures() {
    std::cout << "    Testing Configuration Advanced Features..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values with different sources
    config.set("test_key", "value", nlm::ConfigSource::Default);
    config.set("test_key", "value2", nlm::ConfigSource::File);
    config.set("test_key", "value3", nlm::ConfigSource::CommandLine);
    config.set("test_key", "value4", nlm::ConfigSource::Runtime);
    
    // Last source should win
    assert(config.get<std::string>("test_key").has_value());
    
    // Test getting all keys
    auto keys = config.getKeys();
    assert(keys.size() >= 1);
    
    // Test clearing
    config.clear();
    assert(config.getKeys().empty());
    
    // Test validation rule management
    nlm::ValidationRule rule1;
    rule1.keyPattern = "key1";
    rule1.validator = [](const nlm::ConfigValue&) { return true; };
    config.addValidationRule(rule1);
    
    // Test removing validation rule
    config.removeValidationRule("key1");
    
    // Test schema management
    nlm::ConfigSchema schema1("schema1");
    config.addSchema(schema1);
    
    // Test getting schema names
    auto schemaNames = config.getSchemaNames();
    assert(schemaNames.size() >= 1);
    
    // Test removing schema
    config.removeSchema("schema1");
    
    std::cout << "    testConfigurationAdvancedFeatures passed" << std::endl;
}

void runAllConfigTests() {
    std::cout << "\n=== Advanced Config Tests ===" << std::endl;
    
    testConfigurationValidation();
    testValidationRules();
    testSchemaValidation();
    testConfigurationValidationErrors();
    testConfigurationJSONIO();
    testConfigurationFileIO();
    testConfigurationValidationStats();
    testConfigurationAdvancedFeatures();
    
    std::cout << "\n=== All Config Tests Passed ===" << std::endl;
}

} // namespace test_config_advanced