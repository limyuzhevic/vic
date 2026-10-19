#include <iostream>
#include <cassert>
#include "core/Config/Config.hpp"
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"

void testValidationFrameworkBasic() {
    std::cout << "=== Testing Basic Validation Framework ===\n\n";
    
    // Create a configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.1);
    
    // Create a schema
    nlm::ConfigSchema schema;
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons", "Core", false));
    schema.addParameter(nlm::ConfigParameter("simulation_timestep", "double", 0.001,
                                            "Simulation timestep", "Core", false));
    
    // Initialize validation framework
    nlm::ValidationFramework framework;
    bool initResult = framework.initialize();
    assert(initResult == true);
    
    std::cout << "✓ Framework initialized successfully\n";
    
    // Validate configuration
    auto results = framework.validate(config, schema);
    std::cout << "✓ Framework validation completed\n";
    
    // Check results
    std::cout << "Validation results: " << results.size() << " items\n";
    for (const auto& result : results) {
        std::cout << "  [" << static_cast<int>(result.level) << "]";
        std::cout << " Key: " << result.key;
        std::cout << " - " << result.message << "\n";
    }
    
    // Test utility functions
    std::cout << "\nTesting utility functions...\n";
    
    // Test isValid
    bool isValid = nlm::ValidationUtilities::isValid(results);
    std::cout << "✓ isValid(" << isValid << ")\n";
    
    // Test filtering
    auto errorResults = nlm::ValidationUtilities::filterResultsByLevel(results, nlm::ValidationLevel::Error);
    std::cout << "✓ filterResultsByLevel: " << errorResults.size() << " errors\n";
    
    // Test grouping
    auto groupedResults = nlm::ValidationUtilities::groupResultsByKey(results);
    std::cout << "✓ groupResultsByKey: " << groupedResults.size() << " groups\n";
    
    // Test counting
    auto counts = nlm::ValidationUtilities::countByLevel(results);
    std::cout << "✓ countByLevel:" << std::endl;
    for (const auto& pair : counts) {
        std::cout << "    Level " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    // Test export
    std::string jsonOutput = nlm::ValidationUtilities::exportResultsToJSON(results);
    std::cout << "✓ exportResultsToJSON: " << jsonOutput.size() << " characters\n";
    
    std::string csvOutput = nlm::ValidationUtilities::exportResultsToCSV(results);
    std::cout << "✓ exportResultsToCSV: " << csvOutput.size() << " characters\n";
    
    std::string textOutput = nlm::ValidationUtilities::exportResultsToText(results);
    std::cout << "✓ exportResultsToText: " << textOutput.size() << " characters\n";
    
    std::cout << "\n=== Basic Validation Framework Test Complete ===\n";
}

void testValidationUtilities() {
    std::cout << "\n=== Testing Validation Utilities ===\n\n";
    
    // Create test results
    std::vector<nlm::ValidationResultDetail> results;
    
    results.push_back(nlm::ValidationResultDetail("key1", "", nlm::ValidationLevel::Error, "Test error"));
    results.push_back(nlm::ValidationResultDetail("key2", "", nlm::ValidationLevel::Warning, "Test warning"));
    results.push_back(nlm::ValidationResultDetail("key3", "", nlm::ValidationLevel::Info, "Test info"));
    
    // Test save/load
    bool saved = nlm::ValidationUtilities::saveValidationResults("test_results.txt", results, "text");
    assert(saved == true);
    std::cout << "✓ saveValidationResults\n";
    
    std::vector<nlm::ValidationResultDetail> loadedResults;
    bool loaded = nlm::ValidationUtilities::loadValidationResults("test_results.txt", loadedResults, "text");
    assert(loaded == true);
    std::cout << "✓ loadValidationResults: " << loadedResults.size() << " results loaded\n";
    
    // Test filtering
    auto errorResults = nlm::ValidationUtilities::filterByRuleId(results, "NONEXISTENT_RULE");
    assert(errorResults.empty());
    std::cout << "✓ filterByRuleId: " << errorResults.size() << " results for non-existent rule\n";
    
    // Test merging
    std::vector<nlm::ValidationResultDetail> results2;
    results2.push_back(nlm::ValidationResultDetail("key4", "", nlm::ValidationLevel::Info, "Additional info"));
    
    auto mergedResults = nlm::ValidationUtilities::mergeResults(results, results2);
    assert(mergedResults.size() == 4);
    std::cout << "✓ mergeResults: " << mergedResults.size() << " results\n";
    
    // Test sorting
    auto sortedResults = nlm::ValidationUtilities::sortByPriority(results);
    assert(sortedResults.size() == 3);
    std::cout << "✓ sortByPriority: " << sortedResults.size() << " results\n";
    
    // Clean up
    std::remove("test_results.txt");
    
    std::cout << "\n=== Validation Utilities Test Complete ===\n";
}

void testValidationConfigFile() {
    std::cout << "\n=== Testing Validation Configuration File ===\n\n";
    
    // Create validation configuration
    nlm::ValidationConfig config;
    config.enableCrossParameterValidation = true;
    config.enableConditionalValidation = true;
    config.enableRangeValidation = true;
    config.enableDependencyValidation = true;
    config.enableConflictDetection = true;
    config.maxMemoryUsage = 100.0;
    config.maxValidationTime = 10;
    
    // Save to file
    bool saved = nlm::ValidationUtilities::saveValidationConfig("test_validation.cfg", config);
    assert(saved == true);
    std::cout << "✓ saveValidationConfig\n";
    
    // Load from file
    nlm::ValidationConfig loaded = nlm::ValidationUtilities::loadValidationConfig("test_validation.cfg");
    assert(loaded.enableCrossParameterValidation == true);
    assert(loaded.enableConditionalValidation == true);
    assert(loaded.enableRangeValidation == true);
    assert(loaded.enableDependencyValidation == true);
    assert(loaded.enableConflictDetection == true);
    assert(loaded.maxMemoryUsage == 100.0);
    assert(loaded.maxValidationTime == 10);
    std::cout << "✓ loadValidationConfig: configuration loaded successfully\n";
    
    // Clean up
    std::remove("test_validation.cfg");
    
    std::cout << "\n=== Validation Configuration File Test Complete ===\n";
}

int main() {
    std::cout << "=== Configuration Validation Framework Test Suite ===\n\n";
    
    testValidationFrameworkBasic();
    testValidationUtilities();
    testValidationConfigFile();
    
    std::cout << "=== All Tests Complete ===\n";
    return 0;
}
