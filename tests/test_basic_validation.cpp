// Simple test demonstrating the validation framework integration with Config.hpp/Config.cpp
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"
#include <iostream>
#include <cassert>

// Test basic validation framework functionality
void testBasicValidation() {
    std::cout << "=== Basic Validation Framework Test ===\n\n";
    
    // Step 1: Create a configuration
    std::cout << "1. Creating configuration...\n";
    nlm::Config config;
    
    // Set some configuration values
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.1);
    config.set("enable_plasticity", true);
    config.set("fast_mode", false);
    config.set("high_precision", true);
    
    std::cout << "   Configuration created with " << config.getKeys().size() << " parameters\n\n";
    
    // Step 2: Create a schema
    std::cout << "2. Creating configuration schema...\n";
    nlm::ConfigSchema schema;
    
    // Add some parameters to the schema
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons in the brain", "Core", false));
    schema.addParameter(nlm::ConfigParameter("simulation_timestep", "double", 0.001,
                                            "Simulation timestep in seconds", "Core", false));
    schema.addParameter(nlm::ConfigParameter("connection_probability", "double", 0.1,
                                            "Probability of connection between neurons", "Core", false));
    schema.addParameter(nlm::ConfigParameter("enable_plasticity", "bool", false,
                                            "Whether to enable plasticity", "Plasticity", false));
    schema.addParameter(nlm::ConfigParameter("fast_mode", "bool", false,
                                            "Whether to use fast mode", "Performance", false));
    schema.addParameter(nlm::ConfigParameter("high_precision", "bool", false,
                                            "Whether to use high precision", "Performance", false));
    
    std::cout << "   Schema created with " << schema.getAllParameters().size() << " parameters\n\n";
    
    // Step 3: Initialize validation framework
    std::cout << "3. Initializing validation framework...\n";
    nlm::ValidationFramework framework;
    bool initResult = framework.initialize();
    assert(initResult == true);
    
    std::cout << "   Framework initialized successfully!\n";
    std::cout << "   Framework statistics:\n";
    std::cout << framework.getStatistics() << "\n\n";
    
    // Step 4: Validate configuration with schema
    std::cout << "4. Validating configuration with schema...\n";
    auto schemaResults = schema.validateAll(config.getKeys());
    
    std::cout << "   Schema validation completed!\n";
    std::cout << "   Schema validation errors:\n";
    for (const auto& result : schemaResults) {
        std::cout << "     " << result.errorMessage << " (key: " << result.key << ")\n";
    }
    std::cout << "\n";
    
    // Step 5: Validate configuration with framework
    std::cout << "5. Validating configuration with framework...\n";
    auto frameworkResults = framework.validate(config, schema);
    
    std::cout << "   Framework validation completed!\n";
    std::cout << "   Framework validation results: " << frameworkResults.size() << " items\n";
    
    std::cout << "   Detailed results:\n";
    for (const auto& result : frameworkResults) {
        std::cout << "     [" << static_cast<int>(result.level) << "]";
        std::cout << " Key: " << result.key;
        if (!result.subKey.empty()) std::cout << " SubKey: " << result.subKey;
        std::cout << " - " << result.message << "\n";
    }
    std::cout << "\n";
    
    // Step 6: Filter and analyze validation results
    std::cout << "6. Filtering and analyzing validation results...\n";
    
    // Filter by severity
    auto errorResults = nlm::ValidationUtilities::filterResultsByLevel(frameworkResults, nlm::ValidationLevel::Error);
    auto warningResults = nlm::ValidationUtilities::filterResultsByLevel(frameworkResults, nlm::ValidationLevel::Warning);
    auto infoResults = nlm::ValidationUtilities::filterResultsByLevel(frameworkResults, nlm::ValidationLevel::Info);
    
    std::cout << "   Error results: " << errorResults.size() << " items\n";
    std::cout << "   Warning results: " << warningResults.size() << " items\n";
    std::cout << "   Info results: " << infoResults.size() << " items\n";
    
    // Group by key
    auto groupedResults = nlm::ValidationUtilities::groupResultsByKey(frameworkResults);
    
    std::cout << "   Results by key:\n";
    for (const auto& group : groupedResults) {
        std::cout << "     Key: " << group.first << " (" << group.second.size() << " results)\n";
    }
    std::cout << "\n";
    
    // Step 7: Export validation results
    std::cout << "7. Exporting validation results...\n";
    
    // Export to different formats
    std::string jsonOutput = nlm::ValidationUtilities::exportResultsToJSON(frameworkResults);
    std::ofstream jsonFile("validation_results.json");
    if (jsonFile.is_open()) {
        jsonFile << jsonOutput;
        jsonFile.close();
        std::cout << "   Results saved to 'validation_results.json' (JSON format)\n";
    }
    
    std::string csvOutput = nlm::ValidationUtilities::exportResultsToCSV(frameworkResults);
    std::ofstream csvFile("validation_results.csv");
    if (csvFile.is_open()) {
        csvFile << csvOutput;
        csvFile.close();
        std::cout << "   Results saved to 'validation_results.csv' (CSV format)\n";
    }
    
    std::string textOutput = nlm::ValidationUtilities::exportResultsToText(frameworkResults);
    std::ofstream textFile("validation_results.txt");
    if (textFile.is_open()) {
        textFile << textOutput;
        textFile.close();
        std::cout << "   Results saved to 'validation_results.txt' (text format)\n";
    }
    
    std::cout << "\n";
    
    // Step 8: Generate validation report
    std::cout << "8. Generating validation report...\n";
    
    std::string report = nlm::ValidationUtilities::generateValidationReport(frameworkResults);
    
    std::cout << "   Validation report generated!\n";
    std::cout << "   Report preview:\n";
    std::cout << "     " << report.substr(0, std::min(size_t(500), report.size())) << "...\n";
    
    // Save report to file
    std::ofstream reportFile("validation_report.txt");
    if (reportFile.is_open()) {
        reportFile << report;
        reportFile.close();
        std::cout << "   Report saved to 'validation_report.txt'\n";
    }
    
    std::cout << "\n";
    
    // Step 9: Test configuration validation with custom rules
    std::cout << "9. Testing configuration validation with custom rules...\n";
    
    // Create a custom validation rule
    nlm::ValidationRule customRule("CUSTOM_VALIDATION", "Custom Validation Rule",
                                   "This is a custom validation rule", 75);
    customRule.setRangeValidator(
        [](const nlm::Config& cfg, const std::vector<nlm::ConfigEntry>& entries,
           const nlm::ConfigParameter& param, const nlm::ConfigEntry& entry) {
            std::vector<nlm::ValidationResultDetail> results;
            
            // Custom validation logic
            if (entry.key == "neuron_count" && entry.value.if_type<size_t>()) {
                size_t count = *entry.value.if_type<size_t>();
                if (count > 5000) {
                    results.emplace_back("neuron_count", "", nlm::ValidationLevel::Warning,
                                       "Custom rule: Neuron count is high", "CUSTOM_VALIDATION", 60);
                }
            }
            
            return results;
        }
    );
    
    // Add custom rule to framework
    framework.getEngine().addRule(customRule);
    
    // Validate again with custom rules
    auto customResults = framework.validate(config, schema);
    
    std::cout << "   Custom validation results: " << customResults.size() << " items\n";
    
    for (const auto& result : customResults) {
        if (result.ruleId == "CUSTOM_VALIDATION") {
            std::cout << "     " << result.toString() << "\n";
        }
    }
    
    std::cout << "\n";
    
    // Step 10: Demonstrate configuration integration
    std::cout << "10. Demonstrating configuration integration...\n";
    
    // Create validation configuration
    nlm::ValidationConfig validationConfig;
    validationConfig.enableCrossParameterValidation = true;
    validationConfig.enableConditionalValidation = true;
    validationConfig.enableRangeValidation = true;
    validationConfig.enableDependencyValidation = true;
    validationConfig.enableConflictDetection = true;
    
    // Set configuration in framework
    framework.setConfig(validationConfig);
    
    std::cout << "   Validation configuration updated in framework\n";
    
    std::cout << "\n";
    
    // Final validation summary
    std::cout << "11. Final validation summary...\n";
    
    bool isValid = nlm::ValidationUtilities::isValid(frameworkResults);
    std::cout << "   Configuration is " << (isValid ? "valid" : "invalid") << "\n";
    
    std::cout << "\n";
    
    // Clean up
    std::remove("validation_results.json");
    std::remove("validation_results.csv");
    std::remove("validation_results.txt");
    std::remove("validation_report.txt");
    
    std::cout << "=== Basic Validation Framework Test Complete ===\n";
}

int main() {
    testBasicValidation();
    return 0;
}
