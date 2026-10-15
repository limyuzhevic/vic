// Test Configuration Validator
// Advanced configuration validation for tests

#ifndef TEST_CONFIGURATION_VALIDATOR_HPP
#define TEST_CONFIGURATION_VALIDATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <yaml-cpp/yaml.h>

namespace nlm {
namespace tests {

// Configuration validation result
struct ValidationResult {
    std::string validationName;
    bool passed;
    std::string errorMessage;
    std::map<std::string, std::string> details;
    std::chrono::milliseconds validationTime;
    
    ValidationResult(const std::string& name, bool success, const std::string& error = "")
        : validationName(name), passed(success), errorMessage(error), validationTime(0) {}
};

// Configuration schema
struct ConfigSchema {
    std::string name;
    std::map<std::string, std::string> fieldTypes;
    std::map<std::string, std::string> fieldDescriptions;
    std::map<std::string, std::vector<std::string>> fieldEnums;
    std::map<std::string, std::pair<double, double>> fieldRanges;
    bool required;
    
    ConfigSchema(const std::string& n = "", bool req = true)
        : name(n), required(req) {}
};

// Test configuration
struct TestConfiguration {
    std::string testName;
    std::string testSuite;
    std::map<std::string, std::string> parameters;
    std::map<std::string, std::string> expectedValues;
    std::vector<std::string> validationRules;
    bool valid;
    std::vector<ValidationResult> validationResults;
    
    TestConfiguration(const std::string& name, const std::string& suite)
        : testName(name), testSuite(suite), valid(true) {}
};

// Configuration validator
class TestConfigurationValidator {
public:
    TestConfigurationValidator();
    ~TestConfigurationValidator();
    
    // Schema management
    bool addSchema(const std::string& schemaName, const ConfigSchema& schema);
    bool removeSchema(const std::string& schemaName);
    ConfigSchema getSchema(const std::string& schemaName) const;
    std::vector<std::string> getAvailableSchemas() const;
    
    // Configuration validation
    ValidationResult validateConfiguration(const TestConfiguration& config, const std::string& schemaName = "");
    std::vector<ValidationResult> validateAllConfigurations(const std::vector<TestConfiguration>& configs, const std::string& schemaName = "");
    bool validateConfigurationAgainstSchemas(const TestConfiguration& config);
    
    // Schema validation
    bool validateSchema(const ConfigSchema& schema);
    bool validateField(const std::string& fieldName, const std::string& fieldValue, const std::string& fieldType);
    bool validateFieldRange(const std::string& fieldName, const std::string& fieldValue, const std::pair<double, double>& range);
    bool validateFieldEnum(const std::string& fieldName, const std::string& fieldValue, const std::vector<std::string>& enumValues);
    
    // Configuration validation rules
    bool addValidationRule(const std::string& ruleName, std::function<bool(const TestConfiguration&)> rule);
    bool removeValidationRule(const std::string& ruleName);
    bool applyValidationRules(const TestConfiguration& config, std::vector<ValidationResult>& results);
    
    // Reporting
    std::string generateValidationReport(const std::vector<TestConfiguration>& configs) const;
    std::string generateSchemaReport(const ConfigSchema& schema) const;
    std::string generateValidationRulesReport() const;
    
    // File operations
    bool exportSchema(const std::string& filename, const std::string& format = "yaml");
    bool exportValidationResults(const std::string& filename, const std::vector<ValidationResult>& results, const std::string& format = "csv");
    bool importSchema(const std::string& filename);
    bool importValidationRules(const std::string& filename);
    
    // Configuration templates
    bool addConfigurationTemplate(const std::string& templateName, const TestConfiguration& templateConfig);
    TestConfiguration getConfigurationTemplate(const std::string& templateName) const;
    std::vector<std::string> getAvailableTemplates() const;
    TestConfiguration applyTemplate(const std::string& templateName, const TestConfiguration& baseConfig) const;
    
    // Analysis and statistics
    double calculateValidationSuccessRate(const std::vector<TestConfiguration>& configs) const;
    std::map<std::string, size_t> getValidationResultCounts(const std::vector<ValidationResult>& results) const;
    std::vector<std::string> getValidationErrorMessages(const std::vector<ValidationResult>& results) const;
    
    // Advanced validation
    std::vector<ValidationResult> validateConfigurationDependencies(const TestConfiguration& config);
    bool validateConfigurationConsistency(const TestConfiguration& config);
    std::vector<std::string> identifyConfigurationConflicts(const TestConfiguration& config) const;
    
    // Real-world scenario validation
    std::vector<ValidationResult> validateResearchConfiguration(const TestConfiguration& config);
    std::vector<ValidationResult> validateProductionConfiguration(const TestConfiguration& config);
    std::vector<ValidationResult> validateDevelopmentConfiguration(const TestConfiguration& config);
    std::vector<ValidationResult> validateDeploymentConfiguration(const TestConfiguration& config);
    
    // Configuration optimization
    TestConfiguration optimizeConfiguration(const TestConfiguration& config) const;
    std::vector<std::string> getConfigurationOptimizationSuggestions(const TestConfiguration& config) const;
    
    // Singleton pattern
    static TestConfigurationValidator& getInstance();
    static void initialize();
    static void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor and destructor
    TestConfigurationValidator(const TestConfigurationValidator&) = delete;
    TestConfigurationValidator& operator=(const TestConfigurationValidator&) = delete;
    
    // Helper functions
    bool validateYAMLConfiguration(const std::string& yamlString, TestConfiguration& config);
    std::string configToYAML(const TestConfiguration& config) const;
    bool yamlToConfig(const std::string& yamlString, TestConfiguration& config);
    bool validateFieldType(const std::string& fieldType, const std::string& value);
    std::string getFieldTypeName(const std::string& typeName) const;
};

} // namespace tests
} // namespace nlm

#endif // TEST_CONFIGURATION_VALIDATOR_HPP
