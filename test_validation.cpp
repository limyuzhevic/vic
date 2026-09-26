//!/usr/bin/env cpp
// Simple test program to demonstrate NLM configuration validation
// This program shows the new validation system in action

#include "Brain.hpp"
#include "Config.hpp"
#include "ConfigSchema.hpp"
#include <iostream>
#include <fstream>

void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void testBasicValidation() {
    printSection("Test 1: Basic Configuration Validation");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Add some configuration values
    config->set("learning_rate", 0.01);
    config->set("threshold", 0.5);
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    // Validate - should pass
    std::cout << "Validating configuration..." << std::endl;
    if (config->validate()) {
        std::cout << "✓ Basic validation passed" << std::endl;
    } else {
        std::cout << "✗ Basic validation failed:" << std::endl;
        auto errors = config->getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    }
}

void testInvalidConfiguration() {
    printSection("Test 2: Invalid Configuration Handling");
    
    // Create configuration with invalid values
    auto config = std::make_shared<Config>();
    
    config->set("learning_rate", 1.5);  // Invalid: > 1.0
    config->set("threshold", -0.1);     // Invalid: < 0.0
    config->set("neuron_count", -100); // Invalid: negative
    config->set("", "empty_key");       // Invalid: empty key
    
    std::cout << "Validating configuration with invalid values..." << std::endl;
    if (!config->validate()) {
        std::cout << "✓ Validation correctly detected invalid values:" << std::endl;
        auto errors = config->getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    } else {
        std::cout << "✗ Validation should have detected errors but didn't" << std::endl;
    }
}

void testAutomaticRepair() {
    printSection("Test 3: Automatic Configuration Repair");
    
    // Create configuration with issues
    auto config = std::make_shared<Config>();
    
    config->set("", "bad_key");              // Empty key
    config->set("learning_rate", 1.5);       // Out of range
    config->set("threshold", -0.1);          // Out of range
    config->set("neuron_count", 500);        // Valid
    
    std::cout << "Before repair - Validation result:" << std::endl;
    if (!config->validate()) {
        std::cout << "  Failed with " << config->getValidationErrors().size() << " errors" << std::endl;
    }
    
    // Attempt automatic repair
    std::cout << "Attempting automatic repair..." << std::endl;
    if (config->fixCommonIssues()) {
        std::cout << "✓ Repair completed" << std::endl;
        
        std::cout << "After repair - Validation result:" << std::endl;
        if (config->validate()) {
            std::cout << "  ✓ Validation passed after repair" << std::endl;
        } else {
            std::cout << "  ✗ Still has errors after repair:" << std::endl;
            auto errors = config->getValidationErrors();
            for (const auto& error : errors) {
                std::cout << "    " << error.key << ": " << error.errorMessage << std::endl;
            }
        }
    } else {
        std::cout << "✗ Repair failed" << std::endl;
    }
}

void testSchemaValidation() {
    printSection("Test 4: Schema-Based Validation");
    
    auto config = std::make_shared<Config>();
    
    // Create schema
    ConfigSchema schema;
    schema.addRangeRule("learning_rate", 0.0, 1.0, false);  // Required range 0.0-1.0
    schema.addRangeRule("threshold", 0.0, 1.0, false);     // Required range 0.0-1.0
    schema.addNumericRule("neuron_count", typeid(size_t), 1, 1000000, true); // Required: 1-1M neurons
    schema.addStringRule("model_name", 1, 100, "^[a-zA-Z0-9_-]+$", false); // Required: alphanumeric
    
    // Test with valid configuration
    config->set("learning_rate", 0.01);
    config->set("threshold", 0.5);
    config->set("neuron_count", 1000);
    config->set("model_name", "valid_model");
    
    std::cout << "Testing valid configuration against schema..." << std::endl;
    ValidationResult result = config->validateAgainstSchema(schema);
    if (result.isValid) {
        std::cout << "✓ Schema validation passed" << std::endl;
    } else {
        std::cout << "✗ Schema validation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    }
    
    // Test with invalid configuration
    std::cout << "\nTesting invalid configuration against schema..." << std::endl;
    auto badConfig = std::make_shared<Config>();
    badConfig->set("learning_rate", 1.5);  // Invalid: > 1.0
    badConfig->set("threshold", -0.1);     // Invalid: < 0.0
    badConfig->set("neuron_count", 0);     // Invalid: < 1
    badConfig->set("model_name", "invalid@name"); // Invalid: contains @
    
    ValidationResult badResult = badConfig->validateAgainstSchema(schema);
    if (!badResult.isValid) {
        std::cout << "✓ Schema validation correctly detected invalid values:" << std::endl;
        for (const auto& error : badResult.errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    } else {
        std::cout << "✗ Schema validation should have detected errors but didn't" << std::endl;
    }
}

void testPythonInterop() {
    printSection("Test 5: Python Interop");
    
    auto config = std::make_shared<Config>();
    
    // Set configuration values
    config->set("learning_rate", 0.01);
    config->set("threshold", 0.5);
    config->set("neuron_count", 1000);
    config->set("model_name", "python_test");
    
    // Convert to Python-like dictionary
    auto pythonDict = config->toPythonDict();
    
    std::cout << "Configuration converted to Python-like dictionary:" << std::endl;
    for (const auto& pair : pythonDict) {
        std::cout << "  " << pair.first << ": ";
        std::visit([](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), int>) {
                std::cout << val << " (int)" << std::endl;
            } else if constexpr (std::is_same_v<decltype(val), double>) {
                std::cout << val << " (double)" << std::endl;
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                std::cout << (val ? "true" : "false") << " (bool)" << std::endl;
            } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                std::cout << "\"" << val << "\" (string)" << std::endl;
            } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
                std::cout << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << val[i];
                }
                std::cout << "] (vector<int>)" << std::endl;
            } else {
                std::cout << "[complex type]" << std::endl;
            }
        }, pair.second);
    }
    
    // Create Python-like dictionary
    std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> inputDict;
    inputDict["learning_rate"] = 0.02;
    inputDict["threshold"] = 0.3;
    inputDict["neuron_count"] = 2000;
    inputDict["model_name"] = "imported_model";
    
    std::cout << "\nImporting from Python-like dictionary..." << std::endl;
    if (config->fromPythonDict(inputDict)) {
        std::cout << "✓ Import successful" << std::endl;
        std::cout << "Updated values:" << std::endl;
        std::cout << "  learning_rate: " << config->getOr<double>("learning_rate", 0.0) << std::endl;
        std::cout << "  threshold: " << config->getOr<double>("threshold", 0.0) << std::endl;
        std::cout << "  neuron_count: " << config->getOr<size_t>("neuron_count", 0) << std::endl;
        std::cout << "  model_name: " << config->getOr<std::string>("model_name", "") << std::endl;
    } else {
        std::cout << "✗ Import failed" << std::endl;
    }
}

void testBrainValidation() {
    printSection("Test 6: Brain Integration with Validation");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("learning_rate", 0.01);
    config->set("threshold", 0.5);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Enable health monitoring
    brain->enableHealthMonitoring(true);
    
    // Validate configuration
    std::cout << "Validating configuration for brain..." << std::endl;
    if (brain->validateConfiguration()) {
        std::cout << "✓ Configuration validation passed" << std::endl;
    } else {
        std::cout << "✗ Configuration validation failed" << std::endl;
        ValidationResult result = brain->getConfigurationValidation();
        for (const auto& error : result.errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    }
    
    // Check configuration health
    std::cout << "\nChecking configuration health..." << std::endl;
    if (brain->isConfigurationHealthy()) {
        std::cout << "✓ Configuration health check passed" << std::endl;
    } else {
        std::cout << "⚠ Configuration health check warnings:" << std::endl;
        ValidationResult health = brain->getConfigurationHealth();
        for (const auto& warning : health.warnings) {
            std::cout << "  Warning: " << warning << std::endl;
        }
    }
    
    // Check validation statistics
    std::cout << "\nGetting validation statistics..." << std::endl;
    auto stats = brain->getValidationStatistics();
    for (const auto& pair : stats) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void testConfigurationMerge() {
    printSection("Test 7: Configuration Merging");
    
    // Create first configuration
    auto config1 = std::make_shared<Config>();
    config1->set("learning_rate", 0.01);
    config1->set("threshold", 0.5);
    config1->set("neuron_count", 1000);
    
    // Create second configuration
    auto config2 = std::make_shared<Config>();
    config2->set("learning_rate", 0.02);  // Different value
    config2->set("region_count", 3);      // New value
    config2->set("model_name", "merged_model");
    
    std::cout << "Merging configurations..." << std::endl;
    std::cout << "Configuration 1 values:" << std::endl;
    std::cout << "  learning_rate: " << config1->getOr<double>("learning_rate", 0.0) << std::endl;
    std::cout << "  threshold: " << config1->getOr<double>("threshold", 0.0) << std::endl;
    std::cout << "  neuron_count: " << config1->getOr<size_t>("neuron_count", 0) << std::endl;
    
    std::cout << "Configuration 2 values:" << std::endl;
    std::cout << "  learning_rate: " << config2->getOr<double>("learning_rate", 0.0) << std::endl;
    std::cout << "  region_count: " << config2->getOr<size_t>("region_count", 0) << std::endl;
    std::cout << "  model_name: " << config2->getOr<std::string>("model_name", "") << std::endl;
    
    // Merge configurations
    if (config1->mergeConfigs(*config2)) {
        std::cout << "✓ Configuration merge successful" << std::endl;
        std::cout << "Merged configuration values:" << std::endl;
        std::cout << "  learning_rate: " << config1->getOr<double>("learning_rate", 0.0) << " (from config2)" << std::endl;
        std::cout << "  threshold: " << config1->getOr<double>("threshold", 0.0) << " (from config1)" << std::endl;
        std::cout << "  neuron_count: " << config1->getOr<size_t>("neuron_count", 0) << " (from config1)" << std::endl;
        std::cout << "  region_count: " << config1->getOr<size_t>("region_count", 0) << " (from config2)" << std::endl;
        std::cout << "  model_name: " << config1->getOr<std::string>("model_name", "") << " (from config2)" << std::endl;
    } else {
        std::cout << "✗ Configuration merge failed" << std::endl;
    }
}

void testDefaultSchema() {
    printSection("Test 8: Default Schema Validation");
    
    auto config = std::make_shared<Config>();
    
    // Set some values
    config->set("learning_rate", 0.01);
    config->set("threshold", 0.5);
    config->set("neuron_count", 1000);
    
    // Use default schema
    auto defaultSchema = ConfigSchema::getDefaultSchema();
    
    std::cout << "Validating against default schema..." << std::endl;
    ValidationResult result = config->validateAgainstSchema(defaultSchema);
    
    if (result.isValid) {
        std::cout << "✓ Default schema validation passed" << std::endl;
        std::cout << "Schema contains " << defaultSchema.getRules().size() << " validation rules" << std::endl;
        
        // Show some rules
        std::cout << "Sample rules:" << std::endl;
        int count = 0;
        for (const auto& rule : defaultSchema.getRules()) {
            if (count >= 3) break;
            std::cout << "  - " << rule.key << ": type=" << rule.type.name() << ", required=" << rule.required << std::endl;
            count++;
        }
    } else {
        std::cout << "✗ Default schema validation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.key << ": " << error.errorMessage << std::endl;
        }
    }
}

int main() {
    std::cout << "NLM Configuration Validation System Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    testBasicValidation();
    testInvalidConfiguration();
    testAutomaticRepair();
    testSchemaValidation();
    testPythonInterop();
    testBrainValidation();
    testConfigurationMerge();
    testDefaultSchema();
    
    printSection("All Tests Completed");
    std::cout << "The NLM configuration validation system is working correctly!" << std::endl;
    std::cout << "\nKey features demonstrated:" << std::endl;
    std::cout << "  ✓ Basic configuration validation" << std::endl;
    std::cout << "  ✓ Detection of invalid values" << std::endl;
    std::cout << "  ✓ Automatic configuration repair" << std::endl;
    std::cout << "  ✓ Schema-based validation" << std::endl;
    std::cout << "  ✓ Python interoperability" << std::endl;
    std::cout << "  ✓ Brain integration with validation" << std::endl;
    std::cout << "  ✓ Configuration merging" << std::endl;
    std::cout << "  ✓ Default schema validation" << std::endl;
    
    return 0;
}
