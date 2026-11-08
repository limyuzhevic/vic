// Config Schema Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace test_config_schema {

void testConfigSchemaAddition() {
    nlm::Config config;
    
    // Add schema for neuron_count
    config.addSchema("neuron_count", 
                     "Total number of neurons in the brain", 
                     nlm::ConfigSource::Default, 
                     true, 
                     "int",
                     std::vector<std::pair<std::string, double>>{
                         {"min", 1}, {"max", 100000}
                     });
    
    // Add schema for connection_probability
    config.addSchema("connection_probability", 
                     "Probability of connection between neurons", 
                     nlm::ConfigSource::Default, 
                     false, 
                     "double",
                     std::vector<std::pair<std::string, double>>{
                         {"min", 0.0}, {"max", 1.0}
                     });
    
    // Add schema for debug mode
    config.addSchema("debug_mode", 
                     "Enable debug logging", 
                     nlm::ConfigSource::Runtime, 
                     false, 
                     "bool",
                     std::vector<std::string>{"true", "false"});
    
    std::cout << "    testConfigSchemaAddition passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add valid values
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    // Add schema
    config.addSchema("neuron_count", 
                     "Total neurons", 
                     nlm::ConfigSource::Default, 
                     true, 
                     "int",
                     std::vector<std::pair<std::string, double>>{
                         {"min", 10}, {"max", 5000}
                     });
    
    // Test validation
    auto validation = config.validate();
    assert(validation.valid);
    assert(validation.invalidKeys.empty());
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigSchemaValidationFailure() {
    nlm::Config config;
    
    // Add invalid values
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 1.5, nlm::ConfigSource::Runtime); // Out of range
    
    // Add schema with range
    config.addSchema("neuron_count", 
                     "Neuron count", 
                     nlm::ConfigSource::Default, 
                     true, 
                     "int",
                     std::vector<std::pair<std::string, double>>{
                         {"min", 100}, {"max", 1000}
                     });
    
    config.addSchema("connection_probability", 
                     "Connection probability", 
                     nlm::ConfigSource::Default, 
                     false, 
                     "double",
                     std::vector<std::pair<std::string, double>>{
                         {"min", 0.0}, {"max", 1.0}
                     });
    
    // Test validation - should fail
    auto validation = config.validate();
    assert(!validation.valid);
    assert(!validation.invalidKeys.empty());
    assert(std::find(validation.invalidKeys.begin(), validation.invalidKeys.end(), 
                      "connection_probability") != validation.invalidKeys.end());
    
    std::cout << "    testConfigSchemaValidationFailure passed" << std::endl;
}

void testConfigSchemaMissingRequired() {
    nlm::Config config;
    
    // Add required schema but no value
    config.addSchema("required_param", 
                     "A required parameter", 
                     nlm::ConfigSource::Default, 
                     true, 
                     "string");
    
    // Test validation - should fail due to missing required key
    auto validation = config.validate();
    assert(!validation.valid);
    assert(std::find(validation.invalidKeys.begin(), validation.invalidKeys.end(), 
                      "required_param") != validation.invalidKeys.end());
    
    std::cout << "    testConfigSchemaMissingRequired passed" << std::endl;
}

void testConfigSchemaAllowedValues() {
    nlm::Config config;
    
    config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
    
    // Add schema with allowed values
    config.addSchema("log_level", 
                     "Logging level", 
                     nlm::ConfigSource::Default, 
                     false, 
                     "string",
                     std::vector<std::string>{"DEBUG", "INFO", "WARN", "ERROR", "CRIT"});
    
    // Test validation - should pass
    auto validation = config.validate();
    assert(validation.valid);
    
    std::cout << "    testConfigSchemaAllowedValues passed" << std::endl;
}

void testConfigSchemaRemove() {
    nlm::Config config;
    
    // Add schema
    config.addSchema("temp_param", 
                     "Temporary parameter", 
                     nlm::ConfigSource::Default, 
                     false, 
                     "int");
    
    // Verify schema exists
    auto schema = config.getSchema("temp_param");
    assert(schema.has_value());
    
    // Remove schema
    config.removeSchema("temp_param");
    
    // Verify schema is gone
    schema = config.getSchema("temp_param");
    assert(!schema.has_value());
    
    std::cout << "    testConfigSchemaRemove passed" << std::endl;
}

void testConfigSchemaMerge() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Set different values
    config1.set("param1", 42, nlm::ConfigSource::Runtime);
    config1.set("param2", "hello", nlm::ConfigSource::Runtime);
    
    config2.set("param2", "world", nlm::ConfigSource::File);
    config2.set("param3", true, nlm::ConfigSource::CommandLine);
    
    // Merge config2 into config1 (override = true)
    config1.merge(config2, true);
    
    // Verify merged values
    assert(config1.has("param1"));
    assert(config1.get<int>("param1") == 42);
    assert(config1.get<std::string>("param2") == "world"); // Overridden
    assert(config1.get<bool>("param3") == true);
    
    std::cout << "    testConfigSchemaMerge passed" << std::endl;
}

void testConfigSchemaCopy() {
    nlm::Config config1;
    
    config1.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config1.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    
    // Add schema
    config1.addSchema("neuron_count", "Neuron count", 
                     nlm::ConfigSource::Default, true, "int");
    
    // Create copy
    nlm::Config config2 = config1.copy();
    
    // Verify copy is independent
    assert(config2.has("neuron_count"));
    assert(config2.get<int>("neuron_count") == 100);
    
    // Modify original
    config1.set("neuron_count", 200, nlm::ConfigSource::Runtime);
    
    // Verify copy is unchanged
    assert(config2.get<int>("neuron_count") == 100);
    
    std::cout << "    testConfigSchemaCopy passed" << std::endl;
}

void testConfigSchemaEnvironment() {
    nlm::Config config;
    
    // Set environment variable
    setenv("NLM_TEST_VALUE", "123", 1);
    
    // Add schema for environment variable
    config.addSchema("test_value", "Test value from environment", 
                     nlm::ConfigSource::Runtime, false, "int64_t");
    
    // Load from environment
    config.loadFromEnvironment("NLM_");
    
    // Verify value was loaded
    auto value = config.get<int64_t>("test_value");
    assert(value.has_value());
    assert(value.value() == 123);
    
    std kunna
    std::cout << "    testConfigSchemaEnvironment passed" << std::endl;
}

void testConfigSchemaSummary() {
    nlm::Config config;
    
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.05, nlm::ConfigSource::Runtime);
    
    // Add schema
    config.addSchema("neuron_count", "Number of neurons", 
                     nlm::ConfigSource::Default, true, "int");
    config.addSchema("connection_probability", "Connection probability", 
                     nlm::ConfigSource::Default, false, "double");
    
    // Get schema summary
    std::string summary = config.getSchemaSummary();
    assert(!summary.empty());
    
    std::cout << "    testConfigSchemaSummary passed" << std::endl;
}

void runAll() {
    testConfigSchemaAddition();
    testConfigSchemaValidation();
    testConfigSchemaValidationFailure();
    testConfigSchemaMissingRequired();
    testConfigSchemaAllowedValues();
    testConfigSchemaRemove();
    testConfigSchemaMerge();
    testConfigSchemaCopy();
    testConfigSchemaEnvironment();
    testConfigSchemaSummary();
}

} // namespace test_config_schema