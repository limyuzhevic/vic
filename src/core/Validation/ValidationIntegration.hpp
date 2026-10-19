#pragma once

#include "ValidationFramework.hpp"
#include "ValidationUtilities.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <memory>

namespace nlm {

// Integration example showing how to use the validation framework with existing config system
class ValidationIntegration {
public:
    // Initialize validation framework with configuration
    static std::unique_ptr<ValidationFramework> initializeFramework(const std::string& configFile = "configs/validation.cfg") {
        auto framework = std::make_unique<ValidationFramework>();
        
        // Load validation configuration
        ValidationConfig config = ValidationUtilities::loadValidationConfig(configFile);
        framework->setConfig(config);
        
        // Load validation rules
        if (!config.validationFile.empty()) {
            framework->getEngine().loadRulesFromFile(config.validationFile);
        }
        
        // Initialize the framework
        framework->initialize();
        
        return framework;
    }
    
    // Validate configuration using the framework
    static std::vector<ValidationResultDetail> validateConfiguration(const Config& config,
                                                                     const ConfigSchema& schema,
                                                                     const ValidationConfig& validationConfig) {
        ValidationFramework framework;
        framework.setConfig(validationConfig);
        framework.initialize();
        
        return framework.validate(config, schema);
    }
    
    // Validate configuration and generate report
    static std::string validateAndGenerateReport(const Config& config,
                                                 const ConfigSchema& schema,
                                                 const ValidationConfig& validationConfig,
                                                 const std::string& reportFile = "") {
        auto results = validateConfiguration(config, schema, validationConfig);
        
        std::string report = ValidationUtilities::generateValidationReport(results, validationConfig);
        
        if (!reportFile.empty()) {
            ValidationUtilities::saveValidationResults(reportFile, results, "text");
        }
        
        return report;
    }
    
    // Validate configuration with command-line options
    static std::string validateWithCommandLineOptions(const Config& config,
                                                     const ConfigSchema& schema,
                                                     int argc, char** argv) {
        ValidationConfig validationConfig;
        std::string validationFile, reportFile;
        
        ValidationUtilities::parseValidationOptions(argc, argv, validationConfig, validationFile, reportFile);
        
        // Load validation rules from specified file
        if (!validationFile.empty()) {
            ValidationUtilities::loadRulesFromFile(validationConfig.validationFile, validationFile);
        }
        
        return validateAndGenerateReport(config, schema, validationConfig, reportFile);
    }
    
    // Create validation schema from configuration
    static ConfigSchema createValidationSchema(const std::vector<ConfigParameter>& parameters) {
        ConfigSchema schema;
        
        for (const auto& param : parameters) {
            schema.addParameter(param);
        }
        
        return schema;
    }
    
    // Generate validation rules from configuration schema
    static std::vector<ValidationRule> generateValidationRules(const ConfigSchema& schema) {
        return ValidationUtilities::generateRulesFromSchema(schema);
    }
    
    // Validate configuration with custom rules
    static std::vector<ValidationResultDetail> validateWithCustomRules(const Config& config,
                                                                      const std::vector<ValidationRule>& customRules,
                                                                      const ConfigSchema& schema) {
        // Create a validation framework with custom rules
        ValidationFramework framework;
        framework.initialize();
        
        // Add custom rules
        for (const auto& rule : customRules) {
            framework.getEngine().addRule(rule);
        }
        
        // Get schema validation results
        auto schemaResults = ValidationUtilities::validateConfigWithSchema(config, schema);
        
        // Get framework validation results
        auto frameworkResults = framework.validate(config, schema);
        
        // Merge results
        auto mergedResults = ValidationUtilities::mergeResults(schemaResults, frameworkResults);
        
        return ValidationUtilities::sortByPriority(mergedResults);
    }
    
    // Validate configuration and save results
    static bool validateAndSave(const Config& config,
                               const ConfigSchema& schema,
                               const std::string& reportFile,
                               const std::string& format = "text",
                               const ValidationConfig& validationConfig = ValidationConfig()) {
        auto results = validateConfiguration(config, schema, validationConfig);
        
        if (results.empty() || ValidationUtilities::isValid(results)) {
            return ValidationUtilities::saveValidationResults(reportFile, results, format);
        }
        
        return false;
    }
    
    // Get validation results summary
    static std::string getValidationSummary(const std::vector<ValidationResultDetail>& results) {
        return ValidationUtilities::getValidationSummary(results);
    }
    
    // Check if configuration is valid
    static bool isValidConfiguration(const std::vector<ValidationResultDetail>& results,
                                     ValidationLevel minLevel = ValidationLevel::Error) {
        return ValidationUtilities::isValid(results, minLevel);
    }
    
    // Get invalid configuration keys
    static std::set<std::string> getInvalidKeys(const std::vector<ValidationResultDetail>& results) {
        return ValidationUtilities::getInvalidKeys(results);
    }
    
    // Create test configuration for validation
    static Config createTestConfiguration() {
        Config config;
        
        // Add test parameters
        config.set("neuron_count", 1000);
        config.set("simulation_timestep", 0.001);
        config.set("connection_probability", 0.1);
        config.set("enable_plasticity", true);
        config.set("fast_mode", false);
        config.set("high_precision", true);
        
        return config;
    }
    
    // Create test schema for validation
    static ConfigSchema createTestSchema() {
        ConfigSchema schema;
        
        // Add test parameters
        schema.addParameter(ConfigParameter("neuron_count", "int64_t", 1000,
                                          "Number of neurons in the brain", "Core", false));
        schema.addParameter(ConfigParameter("simulation_timestep", "double", 0.001,
                                          "Simulation timestep in seconds", "Core", false));
        schema.addParameter(ConfigParameter("connection_probability", "double", 0.1,
                                          "Probability of connection between neurons", "Core", false));
        schema.addParameter(ConfigParameter("enable_plasticity", "bool", false,
                                          "Whether to enable plasticity", "Plasticity", false));
        schema.addParameter(ConfigParameter("fast_mode", "bool", false,
                                          "Whether to use fast mode", "Performance", false));
        schema.addParameter(ConfigParameter("high_precision", "bool", false,
                                          "Whether to use high precision", "Performance", false));
        
        return schema;
    }
};

} // namespace nlm
