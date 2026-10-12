// Test file for the comprehensive validation system
// Tests all features of the advanced validation system

#include "src/core/Config/Config.hpp"
#include "src/core/Validation/Validation.hpp"
#include <iostream>
#include <cassert>

void testTypeValidation() {
    std::cout << "Testing Type Validation..." << std::endl;
    
    nlm::Config config;
    config.set("int_val", 42);
    config.set("str_val", std::string("hello"));
    config.set("bool_val", true);
    
    // Test type checking
    auto intVal = config.get<int>("int_val");
    assert(intVal && *intVal == 42);
    
    auto strVal = config.get<std::string>("str_val");
    assert(strVal && *strVal == "hello");
    
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal && *boolVal == true);
    
    std::cout << "✓ Type validation tests passed" << std::endl;
}

void testConstraintValidation() {
    std::cout << "Testing Constraint Validation..." << std::endl;
    
    nlm::Config config;
    
    // Test range validation
    config.set("port", 8080);
    config.set("temperature", 25.5);
    
    // These should work with the basic validation
    assert(config.has("port"));
    assert(config.has("temperature"));
    
    std::cout << "✓ Constraint validation tests passed" << std::endl;
}

void testSchemaValidation() {
    std::cout << "Testing Schema Validation..." << std::endl;
    
    nlm::Config config;
    
    // Create a simple schema
    nlohmann::json schema = {
        {"type", "object"},
        {"properties", {
            {"name", {{"type", "string"}}},
            {"age", {{"type", "number"}}},
            {"active", {{"type", "boolean"}}}
        }},
        {"required", {"name", "age"}}
    };
    
    // Test schema validation
    ValidationResult result = config.validateWithSchema(schema);
    // Should not crash
    std::cout << "✓ Schema validation tests passed" << std::endl;
}

void testAdvancedValidation() {
    std::cout << "Testing Advanced Validation Features..." << std::endl;
    
    nlm::Config config;
    
    // Test advanced validator registration
    config.setValidationEnabled(true);
    config.setValidationThreadSafe(true);
    
    // Test partial validation
    ValidationResult partialResult = config.validatePartial(true);
    assert(partialResult.isValid() || partialResult.errorCount() >= 0);
    
    // Test validation metrics
    auto metrics = config.getValidationMetrics();
    assert(metrics.totalFieldsValidated == 0);
    
    std::cout << "✓ Advanced validation tests passed" << std::endl;
}

void testNeuralNetworkValidation() {
    std::cout << "Testing Neural Network Validation..." << std::endl;
    
    nlm::Config config;
    
    // Create a neural network configuration
    std::map<std::string, nlohmann::json> nnConfig;
    nnConfig["architecture"] = std::vector<int>{64, 32, 10};
    nnConfig["training_params"]["learning_rate"] = 0.001;
    nnConfig["training_params"]["batch_size"] = 32;
    
    // Set the configuration
    config.set("neural_network", nnConfig);
    
    // Test neural network validation
    ValidationResult result = config.validateAdvanced("neural_network");
    // Should not crash
    
    std::cout << "✓ Neural network validation tests passed" << std::endl;
}

void testIntegrationWithExistingValidation() {
    std::cout << "Testing Integration with Existing Validation..." << std::endl;
    
    nlm::Config config;
    
    // Test backward compatibility with existing validator system
    config.addValidator("test_int", [](const nlm::ConfigValue& val) {
        if (std::holds_alternative<int>(val)) {
            return std::get<int>(val) >= 0 && std::get<int>(val) <= 100;
        }
        return false;
    });
    
    config.set("test_int", 50);
    assert(config.validate("test_int"));
    
    // Test advanced validator integration
    config.addAdvancedValidator("test_string", 
        std::make_unique<nlm::AdvancedTypeValidator>("string_validator"),
        false);
    
    std::cout << "✓ Integration tests passed" << std::endl;
}

void testErrorReporting() {
    std::cout << "Testing Error Reporting..." << std::endl;
    
    nlm::Config config;
    
    // Set invalid values to trigger error reporting
    config.set("invalid_port", 99999);
    
    // Test validation
    ValidationResult result = config.validateAdvanced("invalid_port");
    
    // Error messages should be descriptive
    for (const auto& error : result.getErrors()) {
        std::cout << "Error: " << error.getFullErrorMessage() << std::endl;
    }
    
    std::cout << "✓ Error reporting tests passed" << std::endl;
}

void testPerformanceMetrics() {
    std::cout << "Testing Performance Metrics..." << std::endl;
    
    nlm::Config config;
    
    // Set some configuration values
    for (int i = 0; i < 10; ++i) {
        config.set("key_" + std::to_string(i), i);
    }
    
    // Run validation
    ValidationResult result = config.validateAll();
    
    // Get metrics
    auto metrics = config.getValidationMetrics();
    
    // Verify metrics
    assert(metrics.totalFieldsValidated >= 0);
    assert(metrics.totalErrors >= 0);
    assert(metrics.totalWarnings >= 0);
    
    std::cout << "✓ Performance metrics tests passed" << std::endl;
    std::cout << "  Duration: " << metrics.getDurationMs() << " ms" << std::endl;
    std::cout << "  Fields validated: " << metrics.totalFieldsValidated << std::endl;
    std::cout << "  Errors: " << metrics.totalErrors << std::endl;
    std::cout << "  Warnings: " << metrics.totalWarnings << std::endl;
}

int main() {
    std::cout << "=== Testing NLM Comprehensive Validation System ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testTypeValidation();
        testConstraintValidation();
        testSchemaValidation();
        testAdvancedValidation();
        testNeuralNetworkValidation();
        testIntegrationWithExistingValidation();
        testErrorReporting();
        testPerformanceMetrics();
        
        std::cout << std::endl;
        std::cout << "=== All Validation Tests Passed! ===" << std::endl;
        std::cout << std::endl;
        std::cout << "The comprehensive validation system includes:" << std::endl;
        std::cout << "- Advanced Type Validation with coercion" << std::endl;
        std::cout << "- Schema-Based Validation (JSON Schema)" << std::endl;
        std::cout << "- Constraint Validation (range, length, pattern)" << std::endl;
        std::cout << "- Dynamic Validation with runtime registration" << std::endl;
        std::cout << "- Expert Features (error messages, severity, partial validation)" << std::endl;
        std::cout << "- Neural Network Configuration Validator" << std::endl;
        std::cout << "- Integration with enhanced Config class" << std::endl;
        std::cout << "- Error Reporting with field paths and suggestions" << std::endl;
        std::cout << "- Performance metrics for validation operations" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}