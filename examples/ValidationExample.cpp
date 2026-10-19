#include <iostream>
#include <fstream>
#include <string>
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include "core/Validation/ValidationFramework.hpp"
#include "core/Validation/ValidationUtilities.hpp"

// Example: Simple validation of configuration
void example_validation() {
    std::cout << "=== Configuration Validation Example ===\n\n";
    
    // Create a configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.1);
    
    // Create a simple schema
    nlm::ConfigSchema schema;
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons", "Core", false));
    schema.addParameter(nlm::ConfigParameter("simulation_timestep", "double", 0.001,
                                            "Simulation timestep", "Core", false));
    schema.addParameter(nlm::ConfigParameter("connection_probability", "double", 0.1,
                                            "Connection probability", "Core", false));
    
    // Initialize validation framework
    nlm::ValidationFramework framework;
    framework.initialize();
    
    // Validate configuration
    auto results = framework.validate(config, schema);
    
    // Display results
    std::cout << "Validation Results:\n";
    for (const auto& result : results) {
        std::cout << "  [" << static_cast<int>(result.level) << "]";
        std::cout << " Key: " << result.key;
        if (!result.subKey.empty()) std::cout << " SubKey: " << result.subKey;
        std::cout << " - " << result.message << "\n";
    }
    
    // Check validity
    bool isValid = nlm::ValidationUtilities::isValid(results);
    std::cout << "\nConfiguration is " << (isValid ? "valid" : "invalid") << "\n";
}

// Example: Validation configuration file
void example_config_file() {
    std::cout << "=== Validation Configuration Example ===\n\n";
    
    // Create a validation configuration
    nlm::ValidationConfig config;
    config.enableCrossParameterValidation = true;
    config.enableConditionalValidation = true;
    config.enableRangeValidation = true;
    config.enableDependencyValidation = true;
    config.enableConflictDetection = true;
    config.maxMemoryUsage = 100.0;
    config.maxValidationTime = 10;
    config.reportAllWarnings = false;
    config.validateOnLoad = true;
    config.validateOnSet = true;
    config.validateOnChange = true;
    
    // Save to file
    bool saved = nlm::ValidationUtilities::saveValidationConfig("configs/validation.cfg", config);
    if (saved) {
        std::cout << "Validation configuration saved to 'configs/validation.cfg'\n";
    }
    
    // Load from file
    nlm::ValidationConfig loaded = nlm::ValidationUtilities::loadValidationConfig("configs/validation.cfg");
    
    std::cout << "Loaded validation configuration:\n";
    std::cout << "  Cross-parameter validation: " << (loaded.enableCrossParameterValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Conditional validation: " << (loaded.enableConditionalValidation ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Range validation: " << (loaded.enableRangeValidation ? "Enabled" : "Disabled") << "\n";
}

// Example: Validation result export
void example_export_results() {
    std::cout << "=== Validation Result Export Example ===\n\n";
    
    // Create sample validation results
    std::vector<nlm::ValidationResultDetail> results;
    
    results.push_back(nlm::ValidationResultDetail("neuron_count", "",
                                                 nlm::ValidationLevel::Error,
                                                 "Neuron count exceeds maximum limit"));
    
    results.push_back(nlm::ValidationResultDetail("simulation_timestep", "",
                                                 nlm::ValidationLevel::Warning,
                                                 "Small timestep may affect performance"));
    
    results.push_back(nlm::ValidationResultDetail("connection_probability", "",
                                                 nlm::ValidationLevel::Info,
                                                 "Connection probability is within range"));
    
    // Export to different formats
    std::string json = nlm::ValidationUtilities::exportResultsToJSON(results);
    std::string yaml = nlm::ValidationUtilities::exportResultsToYAML(results);
    std::string csv = nlm::ValidationUtilities::exportResultsToCSV(results);
    
    // Save to files
    std::ofstream jsonFile("validation_results.json");
    if (jsonFile.is_open()) {
        jsonFile << json;
        jsonFile.close();
        std::cout << "Results saved to 'validation_results.json' (JSON format)\n";
    }
    
    std::ofstream yamlFile("validation_results.yaml");
    if (yamlFile.is_open()) {
        yamlFile << yaml;
        yamlFile.close();
        std::cout << "Results saved to 'validation_results.yaml' (YAML format)\n";
    }
    
    std::ofstream csvFile("validation_results.csv");
    if (csvFile.is_open()) {
        csvFile << csv;
        csvFile.close();
        std::cout << "Results saved to 'validation_results.csv' (CSV format)\n";
    }
}

// Example: Integration with existing configuration system
void example_config_integration() {
    std::cout << "=== Configuration Integration Example ===\n\n";
    
    // Create configuration
    nlm::Config config;
    config.set("neuron_count", 1000);
    config.set("simulation_timestep", 0.001);
    
    // Create schema
    nlm::ConfigSchema schema;
    schema.addParameter(nlm::ConfigParameter("neuron_count", "int64_t", 1000,
                                            "Number of neurons", "Core", false));
    
    // Validate using schema
    std::vector<nlm::ValidationResultDetail> schemaResults = 
        nlm::ValidationUtilities::validateConfigWithSchema(config, schema);
    
    // Validate using framework
    nlm::ValidationFramework framework;
    framework.initialize();
    std::vector<nlm::ValidationResultDetail> frameworkResults = 
        framework.validate(config, schema);
    
    // Merge results
    std::vector<nlm::ValidationResultDetail> merged = 
        nlm::ValidationUtilities::mergeResults(schemaResults, frameworkResults);
    
    std::cout << "Schema validation: " << schemaResults.size() << " results\n";
    std::cout << "Framework validation: " << frameworkResults.size() << " results\n";
    std::cout << "Merged validation: " << merged.size() << " results\n";
    
    // Save merged results
    bool saved = nlm::ValidationUtilities::saveValidationResults("merged_validation.txt", merged, "text");
    if (saved) {
        std::cout << "Merged results saved to 'merged_validation.txt'\n";
    }
}

int main() {
    std::cout << "Configuration Validation Framework Examples\n";
    std::cout << "===============================================\n\n";
    
    example_validation();
    std::cout << "\n";
    
    example_config_file();
    std::cout << "\n";
    
    example_export_results();
    std::cout << "\n";
    
    example_config_integration();
    std::cout << "\n";
    
    std::cout << "=== All Examples Complete ===\n";
    
    return 0;
}
