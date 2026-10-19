#include "core/Validation/ValidationIntegration.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::cout << "=== Configuration Validation Framework ===\n\n";
    
    // Create test configuration
    Config config = nlm::ValidationIntegration::createTestConfiguration();
    
    // Create test schema
    ConfigSchema schema = nlm::ValidationIntegration::createTestSchema();
    
    std::cout << "1. Configuration loaded successfully!\n";
    std::cout << "   Configuration: " << config.summary() << "\n\n";
    
    // Initialize validation framework
    std::cout << "2. Initializing validation framework...\n";
    auto framework = nlm::ValidationIntegration::initializeFramework("configs/validation.cfg");
    
    if (framework) {
        std::cout << "   Framework initialized successfully!\n";
        std::cout << "   Framework statistics:\n";
        std::cout << framework->getStatistics() << "\n\n";
    } else {
        std::cout << "   Warning: Could not initialize framework from file, using defaults\n\n";
    }
    
    // Validate configuration
    std::cout << "3. Validating configuration...\n";
    auto results = nlm::ValidationIntegration::validateConfiguration(config, schema, framework->getConfig());
    
    std::cout << "   Validation complete!\n";
    std::cout << "   " << nlm::ValidationIntegration::getValidationSummary(results) << "\n\n";
    
    // Generate detailed report
    std::cout << "4. Generating validation report...\n";
    std::string report = nlm::ValidationIntegration::validateAndGenerateReport(config, schema, framework->getConfig(), "validation_report.txt");
    
    std::cout << "   Report generated and saved to 'validation_report.txt'\n\n";
    
    // Check if configuration is valid
    std::cout << "5. Checking configuration validity...\n";
    bool isValid = nlm::ValidationIntegration::isValidConfiguration(results);
    
    std::cout << "   Configuration is " << (isValid ? "VALID" : "INVALID") << "\n\n";
    
    // Show validation results
    std::cout << "6. Detailed validation results:\n";
    for (const auto& result : results) {
        std::cout << "   " << result.toString() << "\n";
    }
    std::cout << "\n";
    
    // Save validation results to different formats
    std::cout << "7. Exporting validation results to different formats...\n";
    nlm::ValidationUtilities::saveValidationResults("validation_results.json", results, "json");
    std::cout << "   Saved to 'validation_results.json' (JSON format)\n";
    
    nlm::ValidationUtilities::saveValidationResults("validation_results.yaml", results, "yaml");
    std::cout << "   Saved to 'validation_results.yaml' (YAML format)\n";
    
    nlm::ValidationUtilities::saveValidationResults("validation_results.csv", results, "csv");
    std::cout << "   Saved to 'validation_results.csv' (CSV format)\n\n";
    
    // Demonstrate command-line validation
    std::cout << "8. Demonstrating command-line validation...\n";
    
    // Simulate command-line arguments
    int argc = 6;
    char* argv[] = {
        (char*)"validation_test",
        (char*)"--validation-file=configs/validation.cfg",
        (char*)"--validation-report=test_report.txt",
        (char*)"--validation-stats"
    };
    
    std::string cmdLineReport = nlm::ValidationIntegration::validateWithCommandLineOptions(config, schema, argc, argv);
    std::cout << "   Command-line validation report generated\n\n";
    
    // Show configuration keys with validation errors
    std::cout << "9. Invalid configuration keys...\n";
    auto invalidKeys = nlm::ValidationIntegration::getInvalidKeys(results);
    
    if (invalidKeys.empty()) {
        std::cout << "   No invalid keys found!\n";
    } else {
        std::cout << "   Invalid keys: ";
        for (const auto& key : invalidKeys) {
            std::cout << key << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    
    // Demonstrate custom validation rules
    std::cout << "10. Demonstrating custom validation rules...\n";
    
    // Create a custom validation rule
    nlm::ValidationRule customRule("CUSTOM_001", "Custom Validation Rule",
                                   "This is a custom validation rule", 50);
    customRule.setRangeValidator(
        [](const nlm::Config& cfg, const std::vector<nlm::ConfigEntry>& entries,
           const nlm::ConfigParameter& param, const nlm::ConfigEntry& entry) {
            std::vector<nlm::ValidationResultDetail> results;
            
            // Custom validation logic
            if (entry.key == "neuron_count" && entry.value.if_type<size_t>()) {
                size_t count = *entry.value.if_type<size_t>();
                if (count > 5000) {
                    results.emplace_back("neuron_count", "", nlm::ValidationLevel::Warning,
                                       "Custom rule: Neuron count is high",
                                       "CUSTOM_001", 50);
                }
            }
            
            return results;
        }
    );
    
    // Validate with custom rules
    std::vector<nlm::ValidationRule> customRules;
    customRules.push_back(customRule);
    
    auto customResults = nlm::ValidationIntegration::validateWithCustomRules(config, customRules, schema);
    
    std::cout << "   Custom validation results:\n";
    for (const auto& result : customResults) {
        if (result.ruleId == "CUSTOM_001") {
            std::cout << "   " << result.toString() << "\n";
        }
    }
    std::cout << "\n";
    
    std::cout << "=== Validation Framework Test Complete ===\n";
    
    return 0;
}
