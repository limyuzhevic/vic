// Configuration Validation Test Program
// Demonstrates the configuration validation and schema checking functionality

#include "ConfigValidation.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

void testBasicValidation() {
    std::cout << "=== Basic Configuration Validation Test ===" << std::endl;
    
    try {
        // Test 1: Valid configuration
        std::cout << "\nTest 1: Creating valid configuration...";
        nlm::ConfigBuilder builder;
        builder.set("neuron_count", 500)
               .set("region_count", 2)
               .set("connection_probability", 0.1f)
               .set("random_seed", 42)
               .set("simulation_timestep", 0.001f)
               .set("stdp_ltp_weight", 0.01f)
               .set("stdp_ltd_weight", 0.012f)
               .set("environment_name", "GridWorld")
               .set("log_level", "INFO")
               .set("max_simulation_steps", 10000);
        
        auto config = builder.buildAndValidate();
        std::cout << " PASSED" << std::endl;
        
        // Test 2: Invalid configuration (missing required field)
        std::cout << "\nTest 2: Testing missing required field...";
        nlm::ConfigBuilder invalidBuilder;
        invalidBuilder.set("region_count", 2) // Missing neuron_count (required)
                   .set("connection_probability", 0.1f);
        
        try {
            auto invalidConfig = invalidBuilder.buildAndValidate();
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::ConfigValidationError& e) {
            std::cout << " PASSED (correctly caught exception: " << e.what() << ")" << std::endl;
        }
        
        // Test 3: Invalid configuration (invalid type)
        std::cout << "\nTest 3: Testing invalid type...";
        nlm::ConfigBuilder typeBuilder;
        typeBuilder.set("neuron_count", "not_a_number") // Should be int64_t
                 .set("region_count", 2);
        
        try {
            auto typeConfig = typeBuilder.buildAndValidate();
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::ConfigValidationError& e) {
            std::cout << " PASSED (correctly caught exception: " << e.what() << ")" << std::endl;
        }
        
        // Test 4: Invalid configuration (value out of range)
        std::cout << "\nTest 4: Testing value out of range...";
        nlm::ConfigBuilder rangeBuilder;
        rangeBuilder.set("connection_probability", 1.5f) // Should be <= 1.0
                 .set("region_count", 2);
        
        try {
            auto rangeConfig = rangeBuilder.buildAndValidate();
            std::cout << " FAILED (should have thrown exception)" << std::endl;
        } catch (const nlm::ConfigValidationError& e) {
            std::cout << " PASSED (correctly caught exception: " << e.what() << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testSchemaValidation() {
    std::cout << "\n=== Schema Validation Test ===" << std::endl;
    
    try {
        // Test schema validation
        std::cout << "\nTest 1: Custom schema validation...";
        nlm::ConfigSchema customSchema("2.0", "Custom NLM schema for testing");
        
        // Add custom fields
        customSchema.addField(nlm::ConfigField("custom_int", "int64_t", "100", true,
                                              "Custom integer field"));
        
        customSchema.addField(nlm::ConfigField("custom_float", "float", "3.14", false,
                                              "Custom float field"));
        
        customSchema.addField(nlm::ConfigField("custom_string", "string", "test", false,
                                              "Custom string field"));
        
        // Create config manager with custom schema
        nlm::ConfigManager manager(customSchema);
        
        // Valid configuration
        if (manager.createConfig({
            {"custom_int", "200"},
            {"custom_float", "2.718"},
            {"custom_string", "hello"}
        })) {
            std::cout << " PASSED (custom schema validation succeeded)" << std::endl;
            manager.printConfig();
        } else {
            std::cout << " FAILED (custom schema validation failed)" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testFileParsing() {
    std::cout << "\n=== Configuration File Parsing Test ===" << std::endl;
    
    try {
        // Test file parsing (this would normally read from a file)
        std::cout << "\nTest 1: Testing file-based configuration...";
        
        // Create a temporary config file content
        std::string tempConfig = 
            "# Test configuration file\n"
            "# This is a comment\n"
            "\n"
            "neuron_count = 1000\n"
            "region_count = 3\n"
            "connection_probability = 0.15\n"
            "random_seed = 12345\n"
            "simulation_timestep = 0.0005\n"
            "log_level = INFO\n"
            "log_to_file = true\n"
            "max_simulation_steps = 50000\n";
        
        std::cout << "  Parsing configuration from string...";
        // In a real implementation, this would parse the tempConfig string
        std::cout << " (skipped - would require file I/O in real implementation)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testConfigurationBuilder() {
    std::cout << "\n=== Configuration Builder Test ===" << std::endl;
    
    try {
        // Test builder pattern
        std::cout << "\nTest 1: Testing builder pattern...";
        
        auto config = nlm::ConfigBuilder()
            .set("neuron_count", 2000)
            .set("region_count", 5)
            .set("connection_probability", 0.05f)
            .set("random_seed", 99999)
            .set("log_level", "DEBUG")
            .set("log_to_file", true)
            .set("max_simulation_steps", 100000)
            .build();
        
        std::cout << " PASSED (builder pattern test succeeded)" << std::endl;
        
        // Print configuration
        std::cout << "\nBuilt configuration:" << std::endl;
        config->print();
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

void testErrorHandling() {
    std::cout << "\n=== Error Handling Test ===" << std::endl;
    
    try {
        // Test various error conditions
        std::cout << "\nTest 1: Testing duplicate field error...";
        
        try {
            nlm::ConfigSchema schema("1.0", "Test schema");
            schema.addField(nlm::ConfigField("test_field", "string", "default", true));
            schema.addField(nlm::ConfigField("test_field", "string", "default2", true)); // Duplicate!
            std::cout << " FAILED (should have thrown duplicate field error)" << std::endl;
        } catch (const nlm::ConfigValidationError& e) {
            if (e.getErrorType() == nlm::ConfigValidationError::ErrorType::DUPLICATE_FIELD) {
                std::cout << " PASSED (correctly caught duplicate field error: " << e.what() << ")" << std::endl;
            } else {
                std::cout << " FAILED (wrong error type: " << static_cast<int>(e.getErrorType()) << ")" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "NLM Configuration Validation Test Program" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Run all tests
    testBasicValidation();
    testSchemaValidation();
    testFileParsing();
    testConfigurationBuilder();
    testErrorHandling();
    
    std::cout << "\n=== All Tests Completed ===" << std::endl;
    return 0;
}
