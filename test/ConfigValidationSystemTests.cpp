#include "ConfigValidation.hpp"
#include "ConfigError.hpp"
#include "ValidationRules.hpp"
#include "ConfigSchema.hpp"
#include "ParameterResolver.hpp"
#include "ConfigValidator.hpp"
#include "ComponentFactoryConfigValidator.hpp"
#include "../Config/Config.hpp"
#include <iostream>

// Test configuration validation system
void testConfigError() {
    std::cout << "=== Testing ConfigError ===" << std::endl;
    
    try {
        throw nlm::ConfigError("Test error message");
    } catch (const nlm::ConfigError& e) {
        std::cout << "ConfigError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::ValidationError("Test validation error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "ValidationError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::TypeConversionError("Test type conversion error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "TypeConversionError caught: " << e.what() << std::endl;
    }
    
    std::cout << "ConfigError test passed!" << std::endl << std::endl;
}

void testValidationRules() {
    std::cout << "=== Testing ValidationRules ===" << std::endl;
    
    // Create a simple configuration value
    std::string testKey = "test_int";
    nlm::ConfigValue testValue = 42;
    
    // Create a validation function (e.g., positive integer)
    auto positiveValidator = nlm::ValidationRules::createPositiveValidator(0.0, "integer");
    
    // Test validation
    nlm::ConfigEntry testEntry{testKey, testValue, nlm::ConfigSource::Runtime, "Test integer"};
    nlm::ValidationResult result = positiveValidator(testValue, &testEntry);
    
    if (result.isValid) {
        std::cout << "Validation passed for value: " << nlm::ValidationRules::getValueAsString(testValue) << std::endl;
    } else {
        std::cout << "Validation failed: " << result.errorMessage << std::endl;
    }
    
    // Test range validation
    auto rangeValidator = nlm::ValidationRules::createRangeValidator(0.0, 100.0, "percentage");
    nlm::ConfigValue rangeValue = 50.0;
    nlm::ValidationResult rangeResult = rangeValidator(rangeValue, &testEntry);
    
    if (rangeResult.isValid) {
        std::cout << "Range validation passed for value: " << nlm::ValidationRules::getValueAsString(rangeValue) << std::endl;
    } else {
        std::cout << "Range validation failed: " << rangeResult.errorMessage << std::endl;
    }
    
    // Test string validation
    auto stringLengthValidator = nlm::ValidationRules::createStringLengthValidator(1, 10);
    nlm::ConfigValue stringValue = std::string("hello");
    nlm::ValidationResult stringResult = stringLengthValidator(stringValue, &testEntry);
    
    if (stringResult.isValid) {
        std::cout << "String length validation passed for value: " << nlm::ValidationRules::getValueAsString(stringValue) << std::endl;
    } else {
        std::cout << "String length validation failed: " << stringResult.errorMessage << std::endl;
    }
    
    std::cout << "ValidationRules test passed!" << std::endl << std::endl;
}

void testConfigSchema() {
    std::cout << "=== Testing ConfigSchema ===" << std::endl;
    
    // Create a schema
    nlm::ConfigSchema schema("TestSchema");
    
    // Add fields with different validation rules
    schema.addField("int_field", 0, "Integer field with range validation",
                   {nlm::ValidationRules::createRangeValidator(0.0, 100.0, "integer")});
    
    schema.addField("string_field", std::string("default"), "String field",
                   {nlm::ValidationRules::createStringLengthValidator(1, 10)});
    
    schema.addField("required_field", std::string(""), "Required field",
                   {}, true, "string");
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("int_field", 50);
    config->set("string_field", std::string("test"));
    config->set("required_field", std::string("required_value"));
    
    // Validate configuration
    nlm::ValidationResult result = schema.validate(config);
    
    if (result.isValid) {
        std::cout << "Schema validation passed!" << std::endl;
    } else {
        std::cout << "Schema validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    // Test with missing required field
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("int_field", 150); // Out of range
    
    nlm::ValidationResult result2 = schema.validate(config2);
    
    if (!result2.isValid) {
        std::cout << "Schema correctly rejected invalid configuration:" << std::endl;
        std::cout << result2.errorMessage << std::endl;
    }
    
    std::cout << "ConfigSchema test passed!" << std::endl << std::endl;
}

void testParameterResolver() {
    std::cout << "=== Testing ParameterResolver ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("int_param", 42);
    config->set("double_param", 3.14);
    config->set("bool_param", true);
    config->set("string_param", std::string("test"));
    
    // Create parameter resolver
    nlm::ParameterResolver resolver(config);
    
    // Register parameters
    resolver.registerParameter("int_param", nlm::ParameterType::Integer, 0, "Integer parameter");
    resolver.registerParameter("double_param", nlm::ParameterType::Double, 0.0, "Double parameter");
    resolver.registerParameter("bool_param", nlm::ParameterType::Bool, false, "Boolean parameter");
    resolver.registerParameter("string_param", nlm::ParameterType::String, std::string(""), "String parameter");
    
    // Test type-safe access
    auto intResult = resolver.getInt("int_param", 0);
    if (intResult.success) {
        std::cout << "Int parameter value: " << std::get<int>(intResult.value) << std::endl;
    }
    
    auto doubleResult = resolver.getDouble("double_param", 0.0);
    if (doubleResult.success) {
        std::cout << "Double parameter value: " << std::get<double>(doubleResult.value) << std::endl;
    }
    
    auto boolResult = resolver.getBool("bool_param", false);
    if (boolResult.success) {
        std::cout << "Bool parameter value: " << std::get<bool>(boolResult.value) << std::endl;
    }
    
    auto stringResult = resolver.getString("string_param", "");
    if (stringResult.success) {
        std::cout << "String parameter value: " << std::get<std::string>(stringResult.value) << std::endl;
    }
    
    // Test validation
    bool isValid = resolver.isParameterValid("int_param");
    std::cout << "Int parameter valid: " << (isValid ? "Yes" : "No") << std::endl;
    
    // Test parameter summary
    std::cout << resolver.getParameterSummary() << std::endl;
    
    std::cout << "ParameterResolver test passed!" << std::endl << std::endl;
}

void testConfigValidator() {
    std::cout << "=== Testing ConfigValidator ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("working_memory_capacity", 100);
    
    // Create config validator
    nlm::ConfigValidator validator(config);
    
    // Create a schema
    nlm::ConfigSchema schema("ValidationSchema");
    schema.addField("neuron_count", 0, "Number of neurons", {nlm::ValidationRules::createRangeValidator(10.0, 10000.0, "neuron count")});
    schema.addField("region_count", 0, "Number of regions", {nlm::ValidationRules::createRangeValidator(1.0, 100.0, "region count")});
    schema.addField("connection_probability", 0.0, "Connection probability", {nlm::ValidationRules::createRangeValidator(0.0, 1.0, "probability")});
    schema.addField("working_memory_capacity", 0, "Working memory capacity", {nlm::ValidationRules::createRangeValidator(1.0, 10000.0, "memory capacity")});
    
    validator.setSchema(std::make_unique<nlm::ConfigSchema>(schema));
    
    // Validate configuration
    nlm::ValidationResult result = validator.validate(config);
    
    if (result.isValid) {
        std::cout << "ConfigValidator validation passed!" << std::endl;
    } else {
        std::cout << "ConfigValidator validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    // Test component factory validator
    nlm::ComponentFactoryConfigValidator factoryValidator(config);
    
    // Add default validator
    factoryValidator.addValidator(std::make_unique<nlm::DefaultComponentFactoryValidator>());
    
    // Validate factory configuration
    nlm::ValidationResult factoryResult = factoryValidator.validateFactoryConfiguration();
    
    if (factoryResult.isValid) {
        std::cout << "ComponentFactoryConfigValidator validation passed!" << std::endl;
    } else {
        std::cout << "ComponentFactoryConfigValidator validation failed:" << std::endl;
        std::cout << factoryResult.errorMessage << std::endl;
    }
    
    // Get validation statistics
    nlm::ConfigValidator::ValidationStats stats = validator.getValidationStats();
    std::cout << "Validation statistics:" << std::endl;
    std::cout << "  Total validations: " << stats.totalValidations << std::endl;
    std::cout << "  Successful validations: " << stats.successfulValidations << std::endl;
    std::cout << "  Failed validations: " << stats.failedValidations << std::endl;
    
    std::cout << "ConfigValidator test passed!" << std::endl << std::endl;
}

void testIntegration() {
    std::cout << "=== Testing Integration ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_max_episodes", 1000);
    config->set("associative_memory_capacity", 500);
    config->set("dopamine_baseline", 0.5);
    config->set("curiosity_rate", 0.1);
    config->set("novelty_threshold", 0.7);
    config->set("plasticity_modifier", 1.0);
    config->set("replay_interval", 100);
    config->set("consolidation_interval", 1000);
    config->set("checkpoint_dir", "./checkpoints");
    config->set("checkpoint_interval", 10000);
    
    // Create full configuration validation system
    nlm::ConfigValidator validator(config);
    
    // Create and add component factory validator
    auto factoryValidator = std::make_unique<nlm::DefaultComponentFactoryValidator>();
    validator.addFactoryValidator([factoryValidator](const std::shared_ptr<nlm::Config>& config) {
        return factoryValidator->validateConfiguration(config);
    });
    
    // Add validation callback
    validator.addValidationCallback([](const std::string& key, const nlm::ConfigValue& value, const nlm::ConfigEntry* entry) {
        if (nlm::ValidationRules::isDouble(value)) {
            double val = std::get<double>(value);
            if (val < 0.0) {
                return nlm::ValidationResult::failure("Value must be non-negative");
            }
        }
        return nlm::ValidationResult::success();
    });
    
    // Create and set schema
    nlm::ConfigSchema schema("IntegratedSchema");
    schema.addField("neuron_count", 0, "Neuron count", {nlm::ValidationRules::createRangeValidator(10.0, 10000.0, "neuron count")});
    schema.addField("region_count", 0, "Region count", {nlm::ValidationRules::createRangeValidator(1.0, 100.0, "region count")});
    schema.addField("connection_probability", 0.0, "Connection probability", {nlm::ValidationRules::createRangeValidator(0.0, 1.0, "probability")});
    
    validator.setSchema(std::make_unique<nlm::ConfigSchema>(schema));
    
    // Create and set parameter resolver
    nlm::ParameterResolver resolver(config);
    resolver.registerParameter("neuron_count", nlm::ParameterType::Integer, 0, "Neuron count");
    resolver.registerParameter("region_count", nlm::ParameterType::Integer, 0, "Region count");
    
    validator.setParameterResolver(std::make_unique<nlm::ParameterResolver>(resolver));
    
    // Validate configuration
    nlm::ValidationResult result = validator.validate(config);
    
    if (result.isValid) {
        std::cout << "Integrated validation passed!" << std::endl;
    } else {
        std::cout << "Integrated validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    // Test with invalid configuration
    auto invalidConfig = std::make_shared<nlm::Config>();
    invalidConfig->set("neuron_count", 50000); // Out of range
    invalidConfig->set("region_count", 0); // Out of range
    
    nlm::ValidationResult invalidResult = validator.validate(invalidConfig);
    
    if (!invalidResult.isValid) {
        std::cout << "Integrated validation correctly rejected invalid configuration:" << std::endl;
        std::cout << invalidResult.errorMessage << std::endl;
    }
    
    std::cout << "Integration test passed!" << std::endl << std::endl;
}

void testRuntimeMonitoring() {
    std::cout << "=== Testing Runtime Monitoring ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    // Create config validator with monitoring
    nlm::ConfigValidator validator(config);
    
    // Add change callback
    bool changeDetected = false;
    validator.addChangeCallback([&changeDetected](const nlm::ConfigChange& change) {
        std::cout << "Change detected: " << change.key << " = " << change.newValue << std::endl;
        changeDetected = true;
    });
    
    // Start monitoring
    validator.startMonitoring();
    
    // Make a configuration change
    config->set("neuron_count", 2000);
    
    // Wait a bit for monitoring
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check if change was detected
    if (changeDetected) {
        std::cout << "Change monitoring worked correctly!" << std::endl;
    } else {
        std::cout << "Change monitoring did not work as expected." << std::endl;
    }
    
    // Start runtime validation
    validator.startRuntimeValidation(std::chrono::milliseconds(200));
    
    // Make another configuration change
    config->set("region_count", 5);
    
    // Wait for validation
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // Check validation statistics
    nlm::ConfigValidator::ValidationStats stats = validator.getValidationStats();
    std::cout << "Validation statistics after monitoring:" << std::endl;
    std::cout << "  Total validations: " << stats.totalValidations << std::endl;
    std::cout << "  Successful validations: " << stats.successfulValidations << std::endl;
    std::cout << "  Failed validations: " << stats.failedValidations << std::endl;
    
    // Stop monitoring
    validator.stopMonitoring();
    validator.stopRuntimeValidation();
    
    std::cout << "Runtime monitoring test passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration Validation System Tests ===" << std::endl << std::endl;
    
    testConfigError();
    testValidationRules();
    testConfigSchema();
    testParameterResolver();
    testConfigValidator();
    testIntegration();
    testRuntimeMonitoring();
    
    std::cout << "=== All tests passed! ===" << std::endl;
    return 0;
}