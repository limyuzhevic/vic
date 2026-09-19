// Example: How to use the NLM Configuration Validation System
// This demonstrates the full integration of all components

#include "core/ConfigValidation/ConfigValidator.hpp"
#include "core/ConfigValidation/ParameterResolver.hpp"
#include "core/ConfigValidation/ConfigSchema.hpp"
#include "core/ComponentFactoryConfigValidator.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

void demonstrateBasicConfigurationValidation() {
    std::cout << "=== Basic Configuration Validation ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set configuration values
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("working_memory_capacity", 100);
    
    std::cout << "Configuration set successfully:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Create a schema for validation
    nlm::ConfigSchema schema("BasicSchema");
    schema.addField("neuron_count", 0, "Number of neurons", 
                   {nlm::ValidationRules::createRangeValidator(10.0, 10000.0, "neuron count")});
    schema.addField("region_count", 0, "Number of regions",
                   {nlm::ValidationRules::createRangeValidator(1.0, 100.0, "region count")});
    schema.addField("connection_probability", 0.0, "Connection probability",
                   {nlm::ValidationRules::createRangeValidator(0.0, 1.0, "probability")});
    schema.addField("working_memory_capacity", 0, "Working memory capacity",
                   {nlm::ValidationRules::createRangeValidator(1.0, 10000.0, "memory capacity")});
    
    // Create and use config validator
    nlm::ConfigValidator validator(config);
    validator.setSchema(std::make_unique<nlm::ConfigSchema>(schema));
    
    // Validate configuration
    nlm::ValidationResult result = validator.validate(config);
    
    if (result.isValid) {
        std::cout << "✓ Basic validation passed!" << std::endl;
    } else {
        std::cout << "✗ Basic validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateTypeSafeParameterAccess() {
    std::cout << "=== Type-Safe Parameter Access ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("int_param", 42);
    config->set("double_param", 3.14);
    config->set("bool_param", true);
    config->set("string_param", std::string("test"));
    
    // Create parameter resolver
    nlm::ParameterResolver resolver(config);
    
    // Register parameters with type and default values
    resolver.registerParameter("int_param", nlm::ParameterType::Integer, 0, "Integer parameter");
    resolver.registerParameter("double_param", nlm::ParameterType::Double, 0.0, "Double parameter");
    resolver.registerParameter("bool_param", nlm::ParameterType::Bool, false, "Boolean parameter");
    resolver.registerParameter("string_param", nlm::ParameterType::String, std::string(""), "String parameter");
    
    // Type-safe access
    auto intResult = resolver.getInt("int_param", 0);
    if (intResult.success) {
        std::cout << "✓ Int parameter: " << std::get<int>(intResult.value) << std::endl;
    }
    
    auto doubleResult = resolver.getDouble("double_param", 0.0);
    if (doubleResult.success) {
        std::cout << "✓ Double parameter: " << std::get<double>(doubleResult.value) << std::endl;
    }
    
    auto boolResult = resolver.getBool("bool_param", false);
    if (boolResult.success) {
        std::cout << "✓ Bool parameter: " << std::get<bool>(boolResult.value) << std::endl;
    }
    
    auto stringResult = resolver.getString("string_param", "");
    if (stringResult.success) {
        std::cout << "✓ String parameter: " << std::get<std::string>(stringResult.value) << std::endl;
    }
    
    // Validate parameters
    bool isValid = resolver.isParameterValid("int_param");
    std::cout << "✓ Int parameter valid: " << (isValid ? "Yes" : "No") << std::endl;
    
    std::cout << std::endl;
}

void demonstrateComponentFactoryIntegration() {
    std::cout << "=== Component Factory Integration ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_max_episodes", 1000);
    config->set("associative_memory_capacity", 500);
    
    // Create component factory validator
    nlm::ComponentFactoryConfigValidator factoryValidator(config);
    
    // Add default validator
    factoryValidator.addValidator(std::make_unique<nlm::DefaultComponentFactoryValidator>());
    
    // Validate configuration
    nlm::ValidationResult result = factoryValidator.validateFactoryConfiguration();
    
    if (result.isValid) {
        std::cout << "✓ Component factory validation passed!" << std::endl;
    } else {
        std::cout << "✗ Component factory validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    // Show validator summary
    std::cout << factoryValidator.getValidationSummary() << std::endl;
    
    std::cout << std::endl;
}

void demonstrateFullIntegration() {
    std::cout << "=== Full Integration Example ===" << std::endl;
    
    // Create a comprehensive configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Add various configuration values
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1);
    config->set("stdp_ltp_weight", 0.01);
    config->set("stdp_ltd_weight", 0.012);
    config->set("stdp_tau", 20.0);
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_max_episodes", 1000);
    config->set("associative_memory_capacity", 500);
    config->set("dopamine_baseline", 0.5);
    config->set("curiosity_rate", 0.1);
    config->set("novelty_threshold", 0.7);
    
    std::cout << "✓ Configuration created with " << config->getKeys().size() << " values" << std::endl;
    
    // Create full validator
    nlm::ConfigValidator validator(config);
    
    // Create and set parameter resolver
    nlm::ParameterResolver resolver(config);
    resolver.registerParameter("neuron_count", nlm::ParameterType::Integer, 0, "Neuron count");
    resolver.registerParameter("region_count", nlm::ParameterType::Integer, 0, "Region count");
    resolver.registerParameter("connection_probability", nlm::ParameterType::Double, 0.0, "Connection probability");
    
    validator.setParameterResolver(std::make_unique<nlm::ParameterResolver>(resolver));
    
    // Create and set schema
    nlm::ConfigSchema schema("FullIntegrationSchema");
    schema.addField("neuron_count", 0, "Neuron count", {nlm::ValidationRules::createRangeValidator(10.0, 10000.0, "neuron count")});
    schema.addField("region_count", 0, "Region count", {nlm::ValidationRules::createRangeValidator(1.0, 100.0, "region count")});
    schema.addField("connection_probability", 0.0, "Connection probability", {nlm::ValidationRules::createRangeValidator(0.0, 1.0, "probability")});
    
    validator.setSchema(std::make_unique<nlm::ConfigSchema>(schema));
    
    // Add validation callbacks
    validator.addValidationCallback([](const std::string& key, const nlm::ConfigValue& value, const nlm::ConfigEntry* entry) {
        // Validate that all numeric values are positive
        if (nlm::ValidationRules::isDouble(value)) {
            double val = std::get<double>(value);
            if (val < 0.0) {
                return nlm::ValidationResult::failure("Value must be non-negative");
            }
        } else if (nlm::ValidationRules::isInt(value)) {
            int val = std::get<int>(value);
            if (val < 0) {
                return nlm::ValidationResult::failure("Value must be non-negative");
            }
        }
        return nlm::ValidationResult::success();
    });
    
    // Validate configuration
    nlm::ValidationResult result = validator.validate(config);
    
    if (result.isValid) {
        std::cout << "✓ Full integration validation passed!" << std::endl;
    } else {
        std::cout << "✗ Full integration validation failed:" << std::endl;
        std::cout << result.errorMessage << std::endl;
    }
    
    // Show validation statistics
    nlm::ConfigValidator::ValidationStats stats = validator.getValidationStats();
    std::cout << "Validation statistics:" << std::endl;
    std::cout << "  Total validations: " << stats.totalValidations << std::endl;
    std::cout << "  Successful: " << stats.successfulValidations << std::endl;
    std::cout << "  Failed: " << stats.failedValidations << std::endl;
    
    std::cout << std::endl;
}

void demonstrateRuntimeMonitoring() {
    std::cout << "=== Runtime Monitoring Example ===" << std::endl;
    
    // Create a configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    // Create config validator with monitoring
    nlm::ConfigValidator validator(config);
    
    // Add change callback
    bool changeDetected = false;
    validator.addChangeCallback([&changeDetected](const nlm::ConfigChange& change) {
        std::cout << "  Change detected: " << change.key << " = " << change.newValue << std::endl;
        changeDetected = true;
    });
    
    // Start monitoring
    std::cout << "Starting configuration monitoring..." << std::endl;
    validator.startMonitoring();
    
    // Make configuration changes
    std::cout << "Making configuration changes..." << std::endl;
    config->set("neuron_count", 2000);
    config->set("region_count", 5);
    
    // Wait a bit for monitoring
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check if changes were detected
    if (changeDetected) {
        std::cout << "✓ Change monitoring worked correctly!" << std::endl;
    } else {
        std::cout << "✗ Change monitoring did not work as expected." << std::endl;
    }
    
    // Start runtime validation
    std::cout << "Starting runtime validation..." << std::endl;
    validator.startRuntimeValidation(std::chrono::milliseconds(200));
    
    // Make another configuration change
    config->set("region_count", 10);
    
    // Wait for validation
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // Stop monitoring
    std::cout << "Stopping monitoring..." << std::endl;
    validator.stopMonitoring();
    validator.stopRuntimeValidation();
    
    std::cout << "✓ Runtime monitoring example completed!" << std::endl;
    
    std::cout << std::endl;
}

void demonstrateErrorHandling() {
    std::cout << "=== Error Handling Example ===" << std::endl;
    
    // Test different error types
    try {
        throw nlm::ConfigError("Test config error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "✓ ConfigError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::ValidationError("Test validation error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "✓ ValidationError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::TypeConversionError("Test type conversion error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "✓ TypeConversionError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::SchemaError("Test schema error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "✓ SchemaError caught: " << e.what() << std::endl;
    }
    
    try {
        throw nlm::PluginConfigError("Test plugin config error");
    } catch (const nlm::ConfigError& e) {
        std::cout << "✓ PluginConfigError caught: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration Validation System Examples ===" << std::endl << std::endl;
    
    demonstrateBasicConfigurationValidation();
    demonstrateTypeSafeParameterAccess();
    demonstrateComponentFactoryIntegration();
    demonstrateFullIntegration();
    demonstrateRuntimeMonitoring();
    demonstrateErrorHandling();
    
    std::cout << "=== All examples completed successfully! ===" << std::endl;
    return 0;
}