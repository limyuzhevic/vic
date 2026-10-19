#pragma once

#include "ValidationFramework.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace nlm {

// Validation utilities for integration with existing configuration system
class ValidationUtilities {
public:
    // Convert validation configuration file to internal format
    static ValidationConfig parseValidationConfig(const std::string& filepath);
    
    // Convert validation rules from file to ValidationRulesEngine
    static bool loadRulesFromFile(ValidationRulesEngine& engine, const std::string& filepath);
    
    // Convert validation results to different formats
    static std::string exportResultsToJSON(const std::vector<ValidationResultDetail>& results);
    static std::string exportResultsToYAML(const std::vector<ValidationResultDetail>& results);
    static std::string exportResultsToCSV(const std::vector<ValidationResultDetail>& results);
    static std::string exportResultsToText(const std::vector<ValidationResultDetail>& results);
    
    // Parse configuration from different formats
    static bool parseConfigFromJSON(const std::string& content, Config& config);
    static bool parseConfigFromYAML(const std::string& content, Config& config);
    static bool parseConfigFromKeyValue(const std::string& content, Config& config);
    
    // Generate validation rules from configuration schema
    static std::vector<ValidationRule> generateRulesFromSchema(const ConfigSchema& schema);
    
    // Get validation statistics
    static std::string getValidationStatistics(const ValidationFramework& framework);
    
    // Check if configuration is valid based on severity level
    static bool isValid(const std::vector<ValidationResultDetail>& results, ValidationLevel minLevel = ValidationLevel::Error);
    
    // Get validation results summary
    static std::string getValidationSummary(const std::vector<ValidationResultDetail>& results);
    
    // Filter validation results by rule ID
    static std::vector<ValidationResultDetail> filterByRuleId(const std::vector<ValidationResultDetail>& results, const std::string& ruleId);
    
    // Group validation results by severity
    static std::map<ValidationLevel, std::vector<ValidationResultDetail>> groupBySeverity(const std::vector<ValidationResultDetail>& results);
    
    // Count validation results by level
    static std::map<ValidationLevel, int> countByLevel(const std::vector<ValidationResultDetail>& results);
    
    // Get configuration keys that have validation errors
    static std::set<std::string> getInvalidKeys(const std::vector<ValidationResultDetail>& results);
    
    // Merge validation results
    static std::vector<ValidationResultDetail> mergeResults(const std::vector<ValidationResultDetail>& results1,
                                                            const std::vector<ValidationResultDetail>& results2);
    
    // Sort validation results by priority
    static std::vector<ValidationResultDetail> sortByPriority(const std::vector<ValidationResultDetail>& results);
    
    // Add validation metadata to results
    static void addMetadata(std::vector<ValidationResultDetail>& results, const std::string& frameworkVersion,
                           const std::string& timestamp, const std::string& frameworkName);
    
    // Validate configuration using existing schema
    static std::vector<ValidationResultDetail> validateConfigWithSchema(const Config& config, const ConfigSchema& schema);
    
    // Validate configuration with validation framework
    static std::vector<ValidationResultDetail> validateConfigWithFramework(const Config& config, const ConfigSchema& schema,
                                                                          ValidationFramework& framework);
    
    // Save validation results to file
    static bool saveValidationResults(const std::string& filepath, const std::vector<ValidationResultDetail>& results,
                                     const std::string& format = "text");
    
    // Load validation results from file
    static bool loadValidationResults(const std::string& filepath, std::vector<ValidationResultDetail>& results,
                                     const std::string& format = "text");
    
    // Create validation configuration from command line options
    static ValidationConfig createConfigFromCommandLine(int argc, char** argv);
    
    // Parse command line validation options
    static bool parseValidationOptions(int argc, char** argv, ValidationConfig& config,
                                       std::string& validationFile, std::string& reportFile);
    
    // Generate validation report
    static std::string generateValidationReport(const std::vector<ValidationResultDetail>& results,
                                               const ValidationConfig& config);
    
    // Get validation configuration from file
    static ValidationConfig loadValidationConfig(const std::string& filepath);
    
    // Save validation configuration to file
    static bool saveValidationConfig(const std::string& filepath, const ValidationConfig& config);
}; 

} // namespace nlm
