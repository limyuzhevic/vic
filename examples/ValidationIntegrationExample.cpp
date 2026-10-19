#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"
#include "core/Validation/ValidationIntegration.hpp"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Configuration Validation Framework Integration Example ===\n\n";
    
    // Step 1: Create and populate configuration
    std::cout << "1. Creating and populating configuration...\n";
    nlm::Config config;
    
    // Set various configuration parameters
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.1);
    config.set("enable_plasticity", true);
    config.set("stdp_ltp_weight", 0.01);
    config.set("stdp_ltd_weight", 0.012);
    config.set("stdp_tau", 20.0);
    config.set("fast_mode", false);
    config.set("high_precision", true);
    
    std::cout << "   Configuration loaded:\n";
    std::cout << "   - neuron_count: " << config.getOr<size_t>("neuron_count", 0) << "\n";
    std::cout << "   - simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.0) << "\n";
    std::cout << "   - connection_probability: " << config.getOr<double>("connection_probability", 0.0) << "\n";
    std::cout << "   - enable_plasticity: " << (config.getOr<bool>("enable_plasticity", false) ? "true" : "false") << "\n";
    std::cout << "   - fast_mode: " << (config.getOr<bool>("fast_mode", false) ? "true" : "false") << "\n";
    std::cout << "   - high_precision: " << (config.getOr<bool>("high_precision", false) ? "true" : "false") << "\n";
    std::cout << "\n";
    
    // Step 2: Create configuration schema
    std::cout << "2. Creating configuration schema...\n";
    nlm::ConfigSchema schema;
    
    // Add schema parameters
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons in the brain", "Core", false));
    schema.addParameter(nlm::ConfigParameter("simulation_timestep", "double", 0.001,
                                            "Simulation timestep in seconds", "Core", false));
    schema.addParameter(nlm::ConfigParameter("connection_probability", "double", 0.1,
                                            "Probability of connection between neurons", "Core", false));
    schema.addParameter(nlm::ConfigParameter("enable_plasticity", "bool", false,
                                            "Whether to enable plasticity", "Plasticity", false));
    schema.addParameter(nlm::ConfigParameter("stdp_ltp_weight", "double", 0.01,
                                            "STDP LTP weight", "Plasticity", false));
    schema.addParameter(nlm::ConfigParameter("stdp_ltd_weight", "double", 0.012,
                                            "STDP LTD weight", "Plasticity", false));
    schema.addParameter(nlm::ConfigParameter("stdp_tau", "double", 20.0,
                                            "STDP time constant", "Plasticity", false));
    schema.addParameter(nlm::ConfigParameter("fast_mode", "bool", false,
                                            "Whether to use fast mode", "Performance", false));
    schema.addParameter(nlm::ConfigParameter("high_precision", "bool", false,
                                            "Whether to use high precision", "Performance", false));
    
    std::cout << "   Schema created with " << schema.getAllParameters().size() << " parameters\n";
    std::cout << "\n";
    
    // Step 3: Initialize validation framework
    std::cout << "3. Initializing validation framework...\n";
    auto framework = nlm::ValidationIntegration::initializeFramework("configs/validation.cfg");
    
    if (!framework) {
        std::cout << "   Warning: Could not load validation framework from file, using defaults\n";
        framework = std::make_unique<nlm::ValidationFramework>();
        framework->initialize();
    }
    
    std::cout << "   Framework initialized successfully!\n";
    std::cout << "   Engine statistics:\n";
    std::cout << framework->getStatistics() << "\n\n";
    
    // Step 4: Validate configuration using framework
    std::cout << "4. Validating configuration using framework...\n";
    auto frameworkResults = framework->validate(config, schema);
    
    std::cout << "   Framework validation results: " << frameworkResults.size() << " items\n";
    std::cout << "   " << nlm::ValidationIntegration::getValidationSummary(frameworkResults) << "\n\n";
    
    // Step 5: Validate configuration using schema
    std::cout << "5. Validating configuration using schema...\n";
    auto schemaResults = nlm::ValidationUtilities::validateConfigWithSchema(config, schema);
    
    std::cout << "   Schema validation results: " << schemaResults.size() << " items\n";
    for (const auto& result : schemaResults) {
        std::cout << "   " << result.toString() << "\n";
    }
    std::cout << "\n";
    
    // Step 6: Validate configuration using validation utilities
    std::cout << "6. Validating configuration using validation utilities...\n";
    auto utilitiesResults = nlm::ValidationUtilities::validateConfigWithFramework(config, schema, *framework);
    
    std::cout << "   Utilities validation results: " << utilitiesResults.size() << " items\n\n";
    
    // Step 7: Merge and analyze validation results
    std::cout << "7. Merging and analyzing validation results...\n";
    auto mergedResults = nlm::ValidationUtilities::mergeResults(frameworkResults, utilitiesResults);
    auto schemaMerged = nlm::ValidationUtilities::mergeResults(mergedResults, schemaResults);
    
    std::cout << "   Merged results: " << schemaMerged.size() << " items\n";
    std::cout << "   " << nlm::ValidationIntegration::getValidationSummary(schemaMerged) << "\n\n";
    
    // Step 8: Filter and group validation results
    std::cout << "8. Filtering and grouping validation results...\n";
    auto errorResults = nlm::ValidationUtilities::filterResultsByLevel(schemaMerged, nlm::ValidationLevel::Error);
    auto warningResults = nlm::ValidationUtilities::filterResultsByLevel(schemaMerged, nlm::ValidationLevel::Warning);
    auto infoResults = nlm::ValidationUtilities::filterResultsByLevel(schemaMerged, nlm::ValidationLevel::Info);
    
    std::cout << "   Error results: " << errorResults.size() << " items\n";
    std::cout << "   Warning results: " << warningResults.size() << " items\n";
    std::cout << "   Info results: " << infoResults.size() << " items\n\n";
    
    // Step 9: Group by key
    std::cout << "9. Grouping validation results by key...\n";
    auto groupedResults = nlm::ValidationUtilities::groupResultsByKey(schemaMerged);
    
    for (const auto& group : groupedResults) {
        std::cout << "   Key: " << group.first << " (" << group.second.size() << " results)\n";
        for (const auto& result : group.second) {
            std::cout << "     " << result.toString() << "\n";
        }
    }
    std::cout << "\n";
    
    // Step 10: Export validation results
    std::cout << "10. Exporting validation results...\n";
    
    // Export to JSON
    std::string jsonOutput = nlm::ValidationUtilities::exportResultsToJSON(schemaMerged);
    std::ofstream jsonFile("validation_results.json");
    if (jsonFile.is_open()) {
        jsonFile << jsonOutput;
        jsonFile.close();
        std::cout << "   Results saved to 'validation_results.json' (JSON format)\n";
    }
    
    // Export to YAML
    std::string yamlOutput = nlm::ValidationUtilities::exportResultsToYAML(schemaMerged);
    std::ofstream yamlFile("validation_results.yaml");
    if (yamlFile.is_open()) {
        yamlFile << yamlOutput;
        yamlFile.close();
        std::cout << "   Results saved to 'validation_results.yaml' (YAML format)\n";
    }
    
    // Export to CSV
    std::string csvOutput = nlm::ValidationUtilities::exportResultsToCSV(schemaMerged);
    std::ofstream csvFile("validation_results.csv");
    if (csvFile.is_open()) {
        csvFile << csvOutput;
        csvFile.close();
        std::cout << "   Results saved to 'validation_results.csv' (CSV format)\n";
    }
    
    // Export to text
    std::string textOutput = nlm::ValidationUtilities::exportResultsToText(schemaMerged);
    std::ofstream textFile("validation_results.txt");
    if (textFile.is_open()) {
        textFile << textOutput;
        textFile.close();
        std::cout << "   Results saved to 'validation_results.txt' (text format)\n";
    }
    
    std::cout << "\n";
    
    // Step 11: Generate comprehensive validation report
    std::cout << "11. Generating comprehensive validation report...\n";
    
    // Generate report using integration
    std::string report = nlm::ValidationIntegration::validateAndGenerateReport(config, schema, framework->getConfig(), "comprehensive_validation_report.txt");
    
    std::cout << "   Comprehensive report saved to 'comprehensive_validation_report.txt'\n\n";
    
    // Step 12: Demonstrate command-line validation
    std::cout << "12. Demonstrating command-line validation...\n";
    
    // Simulate command-line arguments
    int argc = 6;
    char* argv[] = {
        (char*)"validation_demo",
        (char*)"--validation-file=configs/validation.cfg",
        (char*)"--validation-report=cmd_line_report.txt",
        (char*)"--validation-stats"
    };
    
    std::string cmdLineReport = nlm::ValidationIntegration::validateWithCommandLineOptions(config, schema, argc, argv);
    std::cout << "   Command-line validation report generated\n\n";
    
    // Step 13: Demonstrate custom validation rules
    std::cout << "13. Demonstrating custom validation rules...\n";
    
    // Create a custom validation rule
    nlm::ValidationRule customRule("CUSTOM_PERFORMANCE", "Custom Performance Validation",
                                   "Validates performance-related custom rules", 80);
    customRule.setRangeValidator(
        [](const nlm::Config& cfg, const std::vector<nlm::ConfigEntry>& entries,
           const nlm::ConfigParameter& param, const nlm::ConfigEntry& entry) {
            std::vector<nlm::ValidationResultDetail> results;
            
            // Custom performance validation
            if (entry.key == "neuron_count" && entry.value.if_type<size_t>()) {
                size_t count = *entry.value.if_type<size_t>();
                if (count > 10000) {
                    results.emplace_back("neuron_count", "", nlm::ValidationLevel::Warning,
                                       "Custom rule: Very high neuron count may affect performance",
                                       "CUSTOM_PERFORMANCE", 60);
                }
            }
            
            if (entry.key == "simulation_timestep" && entry.value.if_type<double>()) {
                double timestep = *entry.value.if_type<double>();
                if (timestep < 0.0001) {
                    results.emplace_back("simulation_timestep", "", nlm::ValidationLevel::Error,
                                       "Custom rule: Very small timestep will cause performance issues",
                                       "CUSTOM_PERFORMANCE", 80);
                }
            }
            
            return results;
        }
    );
    
    // Add custom rule to framework
    framework->getEngine().addRule(customRule);
    
    // Validate with custom rules
    auto customResults = nlm::ValidationIntegration::validateWithCustomRules(config, {customRule}, schema);
    
    std::cout << "   Custom validation results:\n";
    for (const auto& result : customResults) {
        if (result.ruleId == "CUSTOM_PERFORMANCE") {
            std::cout << "   " << result.toString() << "\n";
        }
    }
    std::cout << "\n";
    
    // Step 14: Demonstrate validation configuration
    std::cout << "14. Demonstrating validation configuration...\n";
    
    // Create validation configuration
    nlm::ValidationConfig validationConfig;
    validationConfig.enableCrossParameterValidation = true;
    validationConfig.enableConditionalValidation = true;
    validationConfig.enableRangeValidation = true;
    validationConfig.enableDependencyValidation = true;
    validationConfig.enableConflictDetection = true;
    validationConfig.maxMemoryUsage = 100.0;
    validationConfig.maxValidationTime = 10;
    validationConfig.reportAllWarnings = false;
    validationConfig.validateOnLoad = true;
    validationConfig.validateOnSet = true;
    validationConfig.validateOnChange = true;
    
    std::cout << "   Validation configuration:\n";
    std::cout << "   - Cross-parameter validation: " << (validationConfig.enableCrossParameterValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "   - Conditional validation: " << (validationConfig.enableConditionalValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "   - Range validation: " << (validationConfig.enableRangeValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "   - Dependency validation: " << (validationConfig.enableDependencyValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "   - Conflict detection: " << (validationConfig.enableConflictDetection ? "Enabled" : "Disabled") << "\n";
    std::cout << "   - Max memory usage: " << validationConfig.maxMemoryUsage << " MB\n";
    std::cout << "   - Max validation time: " << validationConfig.maxValidationTime << " seconds\n";
    
    // Save validation configuration
    bool configSaved = nlm::ValidationUtilities::saveValidationConfig("demo_validation.cfg", validationConfig);
    if (configSaved) {
        std::cout << "   Validation configuration saved to 'demo_validation.cfg'\n";
    }
    
    std::cout << "\n";
    
    // Step 15: Final validation summary
    std::cout << "15. Final validation summary...\n";
    
    bool isValid = nlm::ValidationIntegration::isValidConfiguration(schemaMerged);
    std::cout << "   Final configuration is " << (isValid ? "VALID" : "INVALID") << "\n";
    
    auto invalidKeys = nlm::ValidationIntegration::getInvalidKeys(schemaMerged);
    if (!invalidKeys.empty()) {
        std::cout << "   Invalid keys: ";
        for (const auto& key : invalidKeys) {
            std::cout << key << " ";
        }
        std::cout << "\n";
    }
    
    std::cout << "\n";
    std::cout << "=== Configuration Validation Framework Integration Complete ===\n";
    std::cout << "\n";
    std::cout << "Summary of files created:\n";
    std::cout << "- validation_report.txt (Basic validation report)\n";
    std::cout << "- validation_results.json (Results in JSON format)\n";
    std::cout << "- validation_results.yaml (Results in YAML format)\n";
    std::cout << "- validation_results.csv (Results in CSV format)\n";
    std::cout << "- validation_results.txt (Results in text format)\n";
    std::cout << "- comprehensive_validation_report.txt (Comprehensive report)\n";
    std::cout << "- cmd_line_report.txt (Command-line validation report)\n";
    std::cout << "- demo_validation.cfg (Demo validation configuration)\n";
    std::cout << "- validation_results.json (Exported results)\n";
    std::cout << "- validation_results.yaml (Exported results)\n";
    std::cout << "- validation_results.csv (Exported results)\n";
    
    return 0;
}
