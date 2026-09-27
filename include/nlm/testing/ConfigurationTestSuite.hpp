#pragma once

/**
 * Configuration Testing Framework for NLM
 * 
 * Comprehensive configuration validation testing framework providing parameter-aware
 * testing for configuration validation and verification.
 * 
 * Key Features:
 * - Configuration validation testing
 * - Parameter-aware testing for configuration validation
 * - Configuration schema validation
 * - Configuration value range validation
 * - Configuration dependency validation
 * - Configuration consistency validation
 * - Automated configuration test generation
 * 
 * Design Principles:
 * - Parameter-aware testing approach
 * - Comprehensive configuration validation
 * - Schema-driven validation
 * - Dependency-aware validation
 * - Cross-component configuration consistency
 * 
 * Biological Motivation:
 * Neural systems maintain precise internal parameter consistency
 * This framework provides similar validation for neural computation configurations
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <regex>
#include "TestFramework.hpp"

namespace nlm {
namespace testing {

/**
 * Configuration test case structure
 */
struct ConfigurationTestCase {
    std::string name;
    std::string description;
    std::string configFile;
    std::string configSection;
    std::unordered_map<std::string, std::string> expectedValues;
    std::unordered_map<std::string, std::string> expectedRanges;
    std::unordered_map<std::string, std::vector<std::string>> expectedDependencies;
    std::function<bool(const std::unordered_map<std::string, std::string>&)> validationFunction;
    std::string testType; // "value", "range", "dependency", "schema", "custom"
    double timeoutDuration;
    bool isRequired;
    
    ConfigurationTestCase(const std::string& name, const std::string& description,
                         const std::unordered_map<std::string, std::string>& expectedValues)
        : name(name), description(description), expectedValues(expectedValues),
          timeoutDuration(30.0), isRequired(true) {}
};

/**
 * Configuration schema definition
 */
struct ConfigurationSchema {
    std::string schemaName;
    std::string schemaVersion;
    std::string description;
    std::unordered_map<std::string, std::string> fieldTypes;
    std::unordered_map<std::string, std::pair<double, double>> fieldRanges;
    std::unordered_map<std::string, std::vector<std::string>> fieldDependencies;
    std::unordered_map<std::string, std::string> fieldFormats;
    std::unordered_map<std::string, std::string> fieldDescriptions;
    bool isRequired;
    
    ConfigurationSchema(const std::string& name, const std::string& version)
        : schemaName(name), schemaVersion(version), isRequired(true) {}
};

/**
 * Configuration validation result
 */
struct ConfigurationValidationResult {
    std::string testName;
    std::string configFile;
    std::string configSection;
    std::string fieldName;
    std::string fieldValue;
    std::string expectedValue;
    std::string expectedRange;
    std::string errorMessage;
    bool isValid;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string testType;
    
    ConfigurationValidationResult(const std::string& testName, const std::string& configFile,
                                 const std::string& configSection, const std::string& fieldName,
                                 const std::string& fieldValue, const std::string& expectedValue,
                                 const std::string& expectedRange, bool isValid)
        : testName(testName), configFile(configFile), configSection(configSection),
          fieldName(fieldName), fieldValue(fieldValue), expectedValue(expectedValue),
          expectedRange(expectedRange), isValid(isValid),
          timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Configuration testing framework
 */
class ConfigurationTestSuite {
public:
    ConfigurationTestSuite();
    ~ConfigurationTestSuite();
    
    // Schema management
    void registerSchema(const ConfigurationSchema& schema);
    bool validateSchema(const ConfigurationSchema& schema) const;
    void loadSchemaFromFile(const std::string& filename);
    
    // Test case registration
    void registerConfigurationTest(const ConfigurationTestCase& testCase);
    void registerConfigurationTest(const std::string& name,
                                   const std::string& description,
                                   const std::unordered_map<std::string, std::string>& expectedValues,
                                   const std::string& configFile = "",
                                   const std::string& configSection = "");
    
    // Configuration validation
    void runConfigurationTests();
    void runSingleConfigurationTest(const std::string& testName);
    
    // Configuration validation utilities
    bool validateConfigurationFile(const std::string& filename) const;
    bool validateConfigurationSection(const std::string& filename,
                                      const std::string& section) const;
    bool validateConfigurationValue(const std::string& fieldName,
                                    const std::string& fieldValue,
                                    const std::string& expectedValue,
                                    const std::string& expectedRange = "") const;
    bool validateConfigurationDependency(const std::string& filename,
                                         const std::string& section,
                                         const std::string& dependency) const;
    
    // Configuration parsing
    std::unordered_map<std::string, std::string> parseConfigurationFile(const std::string& filename) const;
    std::unordered_map<std::string, std::string> parseConfigurationSection(const std::string& filename,
                                                                            const std::string& section) const;
    
    // Configuration comparison
    bool compareConfigurations(const std::unordered_map<std::string, std::string>& config1,
                              const std::unordered_map<std::string, std::string>& config2) const;
    
    // Configuration test reporting
    void generateConfigurationReport() const;
    void exportConfigurationResults(const std::string& filename) const;
    
    // Configuration consistency validation
    bool validateConfigurationConsistency() const;
    bool validateCrossComponentConfiguration() const;
    
private:
    // Configuration validation utilities
    bool validateValueRange(const std::string& value,
                           const std::string& range) const;
    bool validateValueFormat(const std::string& value,
                           const std::string& format) const;
    bool validateDependency(const std::string& fieldName,
                          const std::string& value,
                          const std::vector<std::string>& dependencies) const;
    
    // Configuration parsing utilities
    void trim(std::string& str) const;
    bool isCommentLine(const std::string& line) const;
    std::string extractSection(const std::string& line) const;
    std::pair<std::string, std::string> parseKeyValue(const std::string& line) const;
    
    // Test execution utilities
    bool executeConfigurationTest(const ConfigurationTestCase& testCase);
    bool executeConfigurationValidation(const ConfigurationTestCase& testCase,
                                       const std::unordered_map<std::string, std::string>& config);
    
    // Reporting utilities
    void writeConfigurationTestResult(std::ofstream& file,
                                      const ConfigurationValidationResult& result) const;
    void generateConfigurationSummary(std::ofstream& file) const;
    
    // Schema validation
    bool validateFieldType(const std::string& value, const std::string& type) const;
    bool validateFieldRange(const std::string& value, const std::pair<double, double>& range) const;
    bool validateFieldDependencies(const std::string& value,
                                  const std::vector<std::string>& dependencies) const;
    
    // Internal state
    std::vector<ConfigurationSchema> schemas_;
    std::vector<ConfigurationTestCase> configurationTests_;
    mutable std::shared_mutex testsMutex_;
    
    std::vector<ConfigurationValidationResult> testResults_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> configData_;
};

/**
 * Configuration schema validator
 */
class ConfigurationSchemaValidator {
public:
    ConfigurationSchemaValidator() = default;
    ~ConfigurationSchemaValidator() = default;
    
    // Schema validation
    bool validateSchema(const ConfigurationSchema& schema) const;
    bool validateFieldTypes(const ConfigurationSchema& schema,
                           const std::unordered_map<std::string, std::string>& config) const;
    bool validateFieldRanges(const ConfigurationSchema& schema,
                           const std::unordered_map<std::string, std::string>& config) const;
    bool validateFieldDependencies(const ConfigurationSchema& schema,
                                  const std::unordered_map<std::string, std::string>& config) const;
    
    // Schema format validation
    bool validateFieldFormat(const std::string& value,
                           const std::string& format) const;
    
    // Schema conversion
    std::string convertToString(const std::string& type) const;
    std::string convertToRangeString(const std::pair<double, double>& range) const;
    
private:
    // Validation utilities
    bool validateInteger(const std::string& value) const;
    bool validateDouble(const std::string& value) const;
    bool validateString(const std::string& value) const;
    bool validateBoolean(const std::string& value) const;
    bool validateDate(const std::string& value) const;
    bool validateEmail(const std::string& value) const;
    bool validateURI(const std::string& value) const;
    
    // Format validation
    bool validateNumericFormat(const std::string& value,
                              const std::string& format) const;
    bool validateDateFormat(const std::string& value,
                          const std::string& format) const;
    bool validateRegexFormat(const std::string& value,
                           const std::string& format) const;
    
    // Range validation
    bool validateRange(const std::string& value,
                      const std::pair<double, double>& range) const;
};

} // namespace testing
} // namespace nlm