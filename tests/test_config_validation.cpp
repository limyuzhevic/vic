// Configuration validation tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_config_validation {

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Create a schema with strict bounds
    nlm::ConfigSchema schema;
    schema.name = "strict_validation";
    schema.description = "Schema for strict parameter validation";
    
    nlm::ParameterBounds positiveBounds;
    positiveBounds.min = 1.0;
    positiveBounds.max = 100.0;
    positiveBounds.allowNegative = false;
    positiveBounds.allowZero = false;
    positiveBounds.unit = "percentage";
    
    nlm::ParameterBounds probabilityBounds;
    probabilityBounds.min = 0.0;
    probabilityBounds.max = 1.0;
    probabilityBounds.allowNegative = false;
    probabilityBounds.allowZero = true;
    probabilityBounds.unit = "probability";
    
    schema.parameterBounds["learning_rate"] = positiveBounds;
    schema.parameterBounds["connection_probability"] = probabilityBounds;
    
    // Add description for validation messages
    schema.defaultValues["learning_rate"] = "0.01";
    
    config.addSchema("validation_test", schema);
    
    // Test valid values
    config.set("learning_rate", 0.5, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.3, nlm::ConfigSource::Runtime);
    
    assert(config.validateParameter("learning_rate"));
    assert(config.validateParameter("connection_probability"));
    
    // Test invalid values - too small
    config.set("learning_rate", 0.0, nlm::ConfigSource::Runtime);  // Below min
    assert(!config.validateParameter("learning_rate"));
    
    // Test invalid values - too large
    config.set("learning_rate", 150.0, nlm::ConfigSource::Runtime);  // Above max
    assert(!config.validateParameter("learning_rate"));
    
    // Test invalid values - negative
    config.set("learning_rate", -0.5, nlm::ConfigSource::Runtime);  // Negative
    assert(!config.validateParameter("learning_rate"));
    
    // Test probability validation - negative
    config.set("connection_probability", -0.1, nlm::ConfigSource::Runtime);
    assert(!config.validateParameter("connection_probability"));
    
    // Test probability validation - valid zero
    config.set("connection_probability", 0.0, nlm::ConfigSource::Runtime);
    assert(config.validateParameter("connection_probability"));
    
    // Test probability validation - valid positive
    config.set("connection_probability", 1.0, nlm::ConfigSource::Runtime);
    assert(config.validateParameter("connection_probability"));
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigMultipleSchemaValidation() {
    nlm::Config config;
    
    // Create first schema
    nlm::ConfigSchema schema1;
    schema1.name = "brain_schema";
    
    nlm::ParameterBounds brainBounds;
    brainBounds.min = 1;
    brainBounds.max = 10000;
    brainBounds.allowNegative = false;
    brainBounds.allowZero = false;
    brainBounds.unit = "neurons";
    
    schema1.parameterBounds["neuron_count"] = brainBounds;
    config.addSchema("brain", schema1);
    
    // Create second schema
    nlm::ConfigSchema schema2;
    schema2.name = "plasticity_schema";
    
    nlm::ParameterBounds plasticityBounds;
    plasticityBounds.min = 0.0;
    plasticityBounds.max = 0.1;
    plasticityBounds.allowNegative = false;
    plasticityBounds.allowZero = true;
    plasticityBounds.unit = "rate";
    
    schema2.parameterBounds["learning_rate"] = plasticityBounds;
    config.addSchema("plasticity", schema2);
    
    // Set values in both schemas
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.05, nlm::ConfigSource::Runtime);
    
    // Both should be valid
    assert(config.validateParameter("neuron_count"));
    assert(config.validateParameter("learning_rate"));
    
    // Test invalid values for both schemas
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime);  // Invalid for brain schema
    config.set("learning_rate", 0.2, nlm::ConfigSource::Runtime);  // Invalid for plasticity schema
    
    assert(!config.validateParameter("neuron_count"));
    assert(!config.validateParameter("learning_rate"));
    
    // Fix values
    config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    
    assert(config.validateParameter("neuron_count"));
    assert(config.validateParameter("learning_rate"));
    assert(config.validateAll());  // All parameters should be valid
    
    std::cout << "    testConfigMultipleSchemaValidation passed" << std::endl;
}

void testConfigValidationErrorReporting() {
    nlm::Config config;
    
    // Create a schema with specific bounds
    nlm::ConfigSchema schema;
    schema.name = "error_reporting_schema";
    
    nlm::ParameterBounds temperatureBounds;
    temperatureBounds.min = 20.0;
    temperatureBounds.max = 40.0;
    temperatureBounds.allowNegative = false;
    temperatureBounds.allowZero = false;
    temperatureBounds.unit = "celsius";
    
    schema.parameterBounds["temperature"] = temperatureBounds;
    config.addSchema("temp", schema);
    
    // Set multiple invalid values
    config.set("temperature", 10.0, nlm::ConfigSource::Runtime);  // Too cold
    config.set("temperature", 50.0, nlm::ConfigSource::Runtime);  // Too hot
    
    // Validate all
    bool allValid = config.validateAll();
    assert(!allValid);
    
    // Get validation errors
    std::vector<std::string> errors = config.getValidationErrors();
    assert(!errors.empty());  // Should have errors
    
    // Each invalid parameter should produce an error
    // Error messages should contain the parameter name
    bool hasTemperatureError = false;
    for (const auto& error : errors) {
        if (error.find("temperature") != std::string::npos) {
            hasTemperatureError = true;
            break;
        }
    }
    assert(hasTemperatureError);
    
    // Fix one value
    config.set("temperature", 30.0, nlm::ConfigSource::Runtime);  // Valid
    
    // Only one error should remain
    allValid = config.validateAll();
    assert(!allValid);  // Still one invalid
    
    errors = config.getValidationErrors();
    assert(errors.size() >= 1);
    
    std::cout << "    testConfigValidationErrorReporting passed" << std::endl;
}

void testConfigSchemaWithCustomValidator() {
    nlm::Config config;
    
    // Create schema with custom validator function
    nlm::ConfigSchema schema;
    schema.name = "custom_validator_schema";
    
    // Custom validator that only accepts even numbers
    std::function<bool(const nlm::ConfigEntry&)> evenNumberValidator = 
        [](const nlm::ConfigEntry& entry) {
            if (std::holds_alternative<int>(entry.value)) {
                int val = std::get<int>(entry.value);
                return val % 2 == 0;  // Only even numbers
            }
            return true;  // Non-numeric values are valid
        };
    
    schema.validator = evenNumberValidator;
    config.addSchema("custom", schema);
    
    // Test with even number
    config.set("even_param", 42, nlm::ConfigSource::Runtime);
    assert(config.validateParameter("even_param"));
    
    // Test with odd number
    config.set("odd_param", 43, nlm::ConfigSource::Runtime);
    assert(!config.validateParameter("odd_param"));
    
    // Test with string
    config.set("string_param", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.validateParameter("string_param"));  // Strings always valid
    
    std::cout << "    testConfigSchemaWithCustomValidator passed" << std::endl;
}

void testConfigValidationEdgeCases() {
    nlm::Config config;
    
    // Create schema with edge case bounds
    nlm::ConfigSchema schema;
    schema.name = "edge_cases_schema";
    
    nlm::ParameterBounds edgeBounds;
    edgeBounds.min = std::numeric_limits<double>::lowest();  // Very small
    edgeBounds.max = std::numeric_limits<double>::max();      // Very large
    edgeBounds.allowNegative = true;
    edgeBounds.allowZero = true;
    edgeBounds.unit = "unbounded";
    
    schema.parameterBounds["unbounded_param"] = edgeBounds;
    config.addSchema("edge", schema);
    
    // Test with extreme values
    config.set("unbounded_param", std::numeric_limits<double>::min(), nlm::ConfigSource::Runtime);
    assert(config.validateParameter("unbounded_param"));
    
    config.set("unbounded_param", std::numeric_limits<double>::max(), nlm::ConfigSource::Runtime);
    assert(config.validateParameter("unbounded_param"));
    
    config.set("unbounded_param", -1e100, nlm::ConfigSource::Runtime);
    assert(config.validateParameter("unbounded_param"));  // Negative allowed
    
    // Test with zero (allowed)
    config.set("unbounded_param", 0.0, nlm::ConfigSource::Runtime);
    assert(config.validateParameter("unbounded_param"));
    
    std::cout << "    testConfigValidationEdgeCases passed" << std::endl;
}

void runAll() {
    std::cout << "Running Configuration Validation tests..." << std::endl;
    testConfigSchemaValidation();
    testConfigMultipleSchemaValidation();
    testConfigValidationErrorReporting();
    testConfigSchemaWithCustomValidator();
    testConfigValidationEdgeCases();
}

} // namespace test_config_validation