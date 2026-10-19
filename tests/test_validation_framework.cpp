// Test file for the Validation Framework
// This file tests the main functionality of the validation framework

#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

// Test validation framework initialization
void testFrameworkInitialization() {
    std::cout << "Testing framework initialization...\n";
    
    nlm::ValidationFramework framework;
    
    // Initialize framework
    bool initResult = framework.initialize();
    assert(initResult == true);
    
    // Check if engine is initialized
    assert(framework.getEngine().isInitialized() == true);
    
    std::cout << "✓ Framework initialization test passed\n";
}

// Test validation configuration
void testValidationConfig() {
    std::cout << "Testing validation configuration...\n";
    
    nlm::ValidationConfig config;
    
    // Set configuration options
    config.enableCrossParameterValidation = true;
    config.enableConditionalValidation = true;
    config.enableRangeValidation = true;
    config.enableDependencyValidation = true;
    config.enableConflictDetection = true;
    config.maxMemoryUsage = 100.0;
    config.maxValidationTime = 10;
    
    // Check configuration
    assert(config.enableCrossParameterValidation == true);
    assert(config.enableConditionalValidation == true);
    assert(config.enableRangeValidation == true);
    assert(config.enableDependencyValidation == true);
    assert(config.enableConflictDetection == true);
    assert(config.maxMemoryUsage == 100.0);
    assert(config.maxValidationTime == 10);
    
    std::cout << "✓ Validation configuration test passed\n";
}

// Test validation results
void testValidationResults() {
    std::cout << "Testing validation results...\n";
    
    nlm::ValidationResultDetail result;
    
    // Test constructor
    nlm::ValidationResultDetail result1("key1", "", nlm::ValidationLevel::Error, "Test error");
    assert(result1.key == "key1");
    assert(result1.level == nlm::ValidationLevel::Error);
    assert(result1.message == "Test error");
    
    // Test isError()
    assert(result1.isError() == true);
    
    // Test toString()
    std::string str = result1.toString();
    assert(str.find("key1") != std::string::npos);
    assert(str.find("Error") != std::string::npos);
    
    std::cout << "✓ Validation results test passed\n";
}

// Test validation rules
void testValidationRules() {
    std::cout << "Testing validation rules...\n";
    
    nlm::ValidationRule rule("TEST_001", "Test Rule", "Test validation rule", 100);
    
    // Check rule properties
    assert(rule.id == "TEST_001");
    assert(rule.name == "Test Rule");
    assert(rule.description == "Test validation rule");
    assert(rule.priority == 100);
    
    // Test adding validators
    rule.setCrossValidator(
        [](const nlm::Config& config, const std::vector<nlm::ConfigEntry>& entries) {
            return std::vector<nlm::ValidationResultDetail>{};
        }
    );
    
    assert(rule.crossValidator != nullptr);
    
    std::cout << "✓ Validation rules test passed\n";
}

// Test validation rules engine
void testValidationRulesEngine() {
    std::cout << "Testing validation rules engine...\n";
    
    nlm::ValidationRulesEngine engine;
    
    // Add a test rule
    nlm::ValidationRule rule("ENGINE_TEST_001", "Engine Test Rule", "Test for engine", 50);
    engine.addRule(rule);
    
    // Check if rule exists
    assert(engine.getRule("ENGINE_TEST_001") != nullptr);
    
    // Get all rules
    auto rules = engine.getRulesByPriority(0, 100);
    assert(rules.size() == 1);
    
    std::cout << "✓ Validation rules engine test passed\n";
}

// Test validation utilities
void testValidationUtilities() {
    std::cout << "Testing validation utilities...\n";
    
    // Create test results
    std::vector<nlm::ValidationResultDetail> results;
    
    nlm::ValidationResultDetail result1("key1", "", nlm::ValidationLevel::Error, "Test error");
    nlm::ValidationResultDetail result2("key2", "", nlm::ValidationLevel::Warning, "Test warning");
    nlm::ValidationResultDetail result3("key3", "", nlm::ValidationLevel::Info, "Test info");
    
    results.push_back(result1);
    results.push_back(result2);
    results.push_back(result3);
    
    // Test filtering by level
    auto errorResults = nlm::ValidationUtilities::filterResultsByLevel(results, nlm::ValidationLevel::Error);
    assert(errorResults.size() == 1);
    assert(errorResults[0].key == "key1");
    
    // Test grouping by severity
    auto grouped = nlm::ValidationUtilities::groupBySeverity(results);
    assert(grouped[nlm::ValidationLevel::Error].size() == 1);
    assert(grouped[nlm::ValidationLevel::Warning].size() == 1);
    assert(grouped[nlm::ValidationLevel::Info].size() == 1);
    
    // Test counting by level
    auto counts = nlm::ValidationUtilities::countByLevel(results);
    assert(counts[nlm::ValidationLevel::Error] == 1);
    assert(counts[nlm::ValidationLevel::Warning] == 1);
    assert(counts[nlm::ValidationLevel::Info] == 1);
    
    std::cout << "✓ Validation utilities test passed\n";
}

// Test configuration validation
void testConfigValidation() {
    std::cout << "Testing configuration validation...\n";
    
    // Create configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Create schema
    nlm::ConfigSchema schema;
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons", "Core", false));
    
    // Initialize framework
    nlm::ValidationFramework framework;
    framework.initialize();
    
    // Validate configuration
    auto results = framework.validate(config, schema);
    
    // Should have at least one result (from default rules)
    assert(!results.empty());
    
    // Check if configuration is valid
    bool isValid = nlm::ValidationUtilities::isValid(results);
    assert(isValid == true);  // Default config should be valid
    
    std::cout << "✓ Configuration validation test passed\n";
}

// Test validation result export
void testValidationExport() {
    std::cout << "Testing validation result export...\n";
    
    // Create test results
    std::vector<nlm::ValidationResultDetail> results;
    
    results.push_back(nlm::ValidationResultDetail("key1", "", nlm::ValidationLevel::Error, "Test error"));
    
    // Export to JSON
    std::string json = nlm::ValidationUtilities::exportResultsToJSON(results);
    assert(json.find("key1") != std::string::npos);
    
    // Export to CSV
    std::string csv = nlm::ValidationUtilities::exportResultsToCSV(results);
    assert(csv.find("key1") != std::string::npos);
    
    // Export to text
    std::string text = nlm::ValidationUtilities::exportResultsToText(results);
    assert(text.find("key1") != std::string::npos);
    
    std::cout << "✓ Validation export test passed\n";
}

// Test validation configuration file
void testValidationConfigFile() {
    std::cout << "Testing validation configuration file...\n";
    
    // Create validation configuration
    nlm::ValidationConfig config;
    config.enableCrossParameterValidation = true;
    config.enableConditionalValidation = true;
    config.enableRangeValidation = true;
    
    // Save to file
    bool saved = nlm::ValidationUtilities::saveValidationConfig("test_validation.cfg", config);
    assert(saved == true);
    
    // Load from file
    nlm::ValidationConfig loaded = nlm::ValidationUtilities::loadValidationConfig("test_validation.cfg");
    
    // Check loaded configuration
    assert(loaded.enableCrossParameterValidation == true);
    assert(loaded.enableConditionalValidation == true);
    assert(loaded.enableRangeValidation == true);
    
    // Clean up
    std::remove("test_validation.cfg");
    
    std::cout << "✓ Validation configuration file test passed\n";
}

// Test validation result save/load
void testValidationResultFile() {
    std::cout << "Testing validation result file operations...\n";
    
    // Create test results
    std::vector<nlm::ValidationResultDetail> results;
    
    results.push_back(nlm::ValidationResultDetail("key1", "", nlm::ValidationLevel::Error, "Test error"));
    
    // Save to file
    bool saved = nlm::ValidationUtilities::saveValidationResults("test_results.txt", results, "text");
    assert(saved == true);
    
    // Load from file
    std::vector<nlm::ValidationResultDetail> loadedResults;
    bool loaded = nlm::ValidationUtilities::loadValidationResults("test_results.txt", loadedResults, "text");
    assert(loaded == true);
    
    assert(loadedResults.size() == 1);
    assert(loadedResults[0].key == "key1");
    
    // Clean up
    std::remove("test_results.txt");
    
    std::cout << "✓ Validation result file operations test passed\n";
}

// Run all tests
void runAllTests() {
    std::cout << "=== Running Validation Framework Tests ===\n\n";
    
    testFrameworkInitialization();
    std::cout << "\n";
    
    testValidationConfig();
    std::cout << "\n";
    
    testValidationResults();
    std::cout << "\n";
    
    testValidationRules();
    std::cout << "\n";
    
    testValidationRulesEngine();
    std::cout << "\n";
    
    testValidationUtilities();
    std::cout << "\n";
    
    testConfigValidation();
    std::cout << "\n";
    
    testValidationExport();
    std::cout << "\n";
    
    testValidationConfigFile();
    std::cout << "\n";
    
    testValidationResultFile();
    std::cout << "\n";
    
    std::cout << "=== All Tests Passed ===\n";
}

int main() {
    runAllTests();
    return 0;
}
