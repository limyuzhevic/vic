// Configuration Validation Tests
#include "core/Config/Config.hpp"
#include "core/Config/ConfigurationSchema.hpp"
#include "core/Config/ConfigurationValidator.hpp"
#include "core/Config/NLMConfigSchema.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testSchemaCreation() {
    nlm::ConfigurationSchema schema;
    assert(!schema.hasParameter("nonexistent"));
    
    std::cout << "    testSchemaCreation passed" << std::endl;
}

void testNLMDefaultSchema() {
    auto schema = nlm::NLMConfigSchema::createDefaultSchema();
    assert(schema != nullptr);
    
    assert(schema->hasParameter("random_seed"));
    assert(schema->hasParameter("neuron_count"));
    assert(schema->hasParameter("log_level"));
    
    std::cout << "    testNLMDefaultSchema passed" << std::endl;
}

void testSchemaParameterValidation() {
    auto schema = nlm::NLMConfigSchema::createDefaultSchema();
    
    // Test valid parameters
    assert(schema->validateParameter("random_seed", nlm::ConfigValue(42)).empty());
    assert(schema->validateParameter("neuron_count", nlm::ConfigValue(1000)).empty());
    assert(schema->validateParameter("log_level", nlm::ConfigValue("INFO")).empty());
    
    // Test type validation - invalid type
    auto typeErrors = schema->validateParameter("random_seed", nlm::ConfigValue("invalid"));
    assert(!typeErrors.empty());
    
    // Test out of range validation for numeric parameters
    auto rangeErrors = schema->validateParameter("neuron_count", nlm::ConfigValue(-1));
    assert(!rangeErrors.empty());
    
    std::cout << "    testSchemaParameterValidation passed" << std::endl;
}

void testConfigurationValidator() {
    auto schema = nlm::NLMConfigSchema::createDefaultSchema();
    nlm::ConfigurationValidator validator;
    validator.setSchema(schema);
    
    std::unordered_map<std::string, nlm::ConfigEntry> config;
    config["random_seed"] = nlm::ConfigEntry("random_seed", nlm::ConfigValue(42), nlm::ConfigSource::File);
    config["neuron_count"] = nlm::ConfigEntry("neuron_count", nlm::ConfigValue(1000), nlm::ConfigSource::File);
    
    // Test complete validation
    auto errors = validator.validateComplete(config, false);
    assert(errors.empty());
    
    // Test with invalid parameter
    config["invalid_param"] = nlm::ConfigEntry("invalid_param", nlm::ConfigValue("test"), nlm::ConfigSource::File);
    auto unknownErrors = validator.validateNoUnknownParameters(config);
    assert(!unknownErrors.empty());
    
    std::cout << "    testConfigurationValidator passed" << std::endl;
}

void testConfigValidationIntegration() {
    nlm::Config config;
    
    // Set valid values
    config.set("random_seed", 42);
    config.set("neuron_count", 1000);
    config.set("log_level", std::string("INFO"));
    
    // Validate with default schema
    bool valid = config.validateWithDefaultSchema();
    assert(valid);
    
    // Check validation errors
    auto errors = config.getValidationErrors();
    assert(errors.empty());
    
    // Set invalid values
    config.clear();
    config.set("random_seed", -1);  // Invalid: should be non-negative
    config.set("neuron_count", 0);  // Invalid: should be positive
    
    // Validate should fail
    valid = config.validateWithDefaultSchema();
    assert(!valid);
    
    // Check that errors were collected
    errors = config.getValidationErrors();
    assert(!errors.empty());
    
    std::cout << "    testConfigValidationIntegration passed" << std::endl;
}

void testSchemaEvolution() {
    nlm::Config config;
    
    // Get default schema
    auto schema = nlm::NLMConfigSchema::createDefaultSchema();
    config.setSchema(schema);
    
    // Test that default schema has required parameters
    auto deprecated = config.getDeprecatedParameters();
    assert(deprecated.empty());  // Default schema has no deprecated parameters
    
    // Test template application
    auto defaultTemplate = nlm::Config::applyTemplate("default");
    assert(defaultTemplate.find("random_seed") != defaultTemplate.end());
    assert(defaultTemplate.find("neuron_count") != defaultTemplate.end());
    
    auto minimalTemplate = nlm::Config::applyTemplate("minimal");
    assert(minimalTemplate.find("neuron_count") != minimalTemplate.end());
    // Minimal schema might not have all parameters
    
    std::cout << "    testSchemaEvolution passed" << std::endl;
}

void testEnvironmentVariableSubstitution() {
    nlm::Config config;
    
    // Set environment variable
    setenv("TEST_VAR", "test_value", 1);
    
    // Test substitution
    std::string testValue = "${TEST_VAR}_suffix";
    std::string substituted = config.substituteEnvironmentVariables(testValue);
    assert(substituted == "test_value_suffix");
    
    // Clean up
    unsetenv("TEST_VAR");
    
    std::cout << "    testEnvironmentVariableSubstitution passed" << std::endl;
}

void testErrorReporting() {
    nlm::Config config;
    
    // Set invalid values
    config.set("random_seed", -1);
    
    // Get error report
    std::string report = config.getValidationErrorReport();
    assert(!report.empty());
    assert(report.find("random_seed") != std::string::npos);
    
    std::cout << "    testErrorReporting passed" << std::endl;
}

void runAllValidationTests() {
    testSchemaCreation();
    testNLMDefaultSchema();
    testSchemaParameterValidation();
    testConfigurationValidator();
    testConfigValidationIntegration();
    testSchemaEvolution();
    testEnvironmentVariableSubstitution();
    testErrorReporting();
}

} // namespace test_config
