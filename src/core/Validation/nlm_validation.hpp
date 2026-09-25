// Enhanced Configuration Validation System
// Comprehensive validation for NLM configuration with detailed error reporting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

#include "nlm_error.hpp"

namespace nlm {

// Validation Result
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::vector<ValidationError> warnings;
    std::chrono::system_clock::time_point timestamp;
    std::string validationId;
    double validationScore; // 0.0 to 1.0, overall validity score
    
    ValidationResult() : valid(true), validationScore(1.0), timestamp(std::chrono::system_clock::now()) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    bool isComplete() const { return valid && !hasErrors(); }
    
    std::string formatReport() const;
    std::string formatSummary() const;
};

// Parameter Range Definition
struct ParameterRange {
    std::string parameterName;
    double minValue;
    double maxValue;
    std::string unit;
    bool inclusiveMin;
    bool inclusiveMax;
    std::string description;
    
    ParameterRange(const std::string& name, double min, double max,
                   const std::string& unit = "", bool incMin = true, bool incMax = true,
                   const std::string& desc = "")
        : parameterName(name), minValue(min), maxValue(max), unit(unit),
          inclusiveMin(incMin), inclusiveMax(incMax), description(desc) {}
    
    bool isInRange(double value) const {
        bool inRange = true;
        if (inclusiveMin) inRange = inRange && (value >= minValue);
        else inRange = inRange && (value > minValue);
        if (inclusiveMax) inRange = inRange && (value <= maxValue);
        else inRange = inRange && (value < maxValue);
        return inRange;
    }
    
    std::string formatRange() const {
        std::stringstream ss;
        ss << parameterName;
        if (!unit.empty()) ss << " (" << unit << ")";
        ss << ": ";
        ss << (inclusiveMin ? "[" : "(") << minValue << ", " << maxValue << ""
           << (inclusiveMax ? "]" : ")") << "";
        if (!description.empty()) ss << " - " << description;
        return ss.str();
    }
};

// File Validation Rules
struct FileValidationRule {
    std::string filePath;
    bool required;
    std::string fileType; // "json", "yaml", "txt", "csv", "bin"
    size_t minSize;
    size_t maxSize;
    std::string contentPattern; // regex pattern for content validation
    std::function<bool(const std::string&)> contentValidator;
    std::string errorMessage;
    
    FileValidationRule(const std::string& path, bool req = true,
                       const std::string& type = "", size_t min = 0, size_t max = 0,
                       const std::string& pattern = "",
                       const std::string& errorMsg = "File validation failed")
        : filePath(path), required(req), fileType(type), minSize(min), maxSize(max),
          contentPattern(pattern), contentValidator(nullptr), errorMessage(errorMsg) {}
};

// Resource Validation Rules
struct ResourceValidationRule {
    std::string resourceName;
    ResourceType resourceType;
    double requiredAmount;
    std::string unit;
    bool checkAvailability;
    std::chrono::milliseconds timeout;
    std::string errorMessage;
    
    ResourceValidationRule(const std::string& name, ResourceType type,
                           double amount, const std::string& unit = "",
                           bool check = true, std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(5000),
                           const std::string& errorMsg = "Resource validation failed")
        : resourceName(name), resourceType(type), requiredAmount(amount), unit(unit),
          checkAvailability(check), timeout(timeoutMs), errorMessage(errorMsg) {}
};

// Input Validation Rules
struct InputValidationRule {
    std::string parameterName;
    std::string parameterType; // "int", "double", "string", "bool", "enum"
    std::string enumValues; // comma-separated for enum type
    std::string pattern; // regex pattern for string validation
    double minValue;
    double maxValue;
    bool required;
    std::string errorMessage;
    std::function<bool(const ConfigValue&)> customValidator;
    
    InputValidationRule(const std::string& name, const std::string& type,
                       const std::string& enumVals = "", const std::string& pat = "",
                       double min = std::numeric_limits<double>::lowest(),
                       double max = std::numeric_limits<double>::max(),
                       bool req = true, const std::string& errorMsg = "Input validation failed")
        : parameterName(name), parameterType(type), enumValues(enumVals), pattern(pat),
          minValue(min), maxValue(max), required(req), errorMessage(errorMsg) {}
};

// Enhanced Configuration Validator
class EnhancedConfigValidator {
private:
    std::vector<ParameterRange> parameterRanges;
    std::vector<FileValidationRule> fileRules;
    std::vector<ResourceValidationRule> resourceRules;
    std::vector<InputValidationRule> inputRules;
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> customValidators;
    std::vector<std::string> requiredFiles;
    std::set<std::string> excludeParameters;
    bool strictMode;
    std::chrono::milliseconds validationTimeout;
    
    // Validation helpers
    bool validateParameterRange(const ConfigValue& value, double min, double max, bool inclusive) const;
    bool validateStringPattern(const std::string& value, const std::string& pattern) const;
    bool validateEnumValue(const std::string& value, const std::string& enumValues) const;
    bool validateFileExists(const std::string& path, bool required) const;
    bool validateFileSize(const std::string& path, size_t minSize, size_t maxSize) const;
    bool validateFileContent(const std::string& path, const std::string& pattern,
                            const std::function<bool(const std::string&)>& validator) const;
    bool validateResourceAvailability(const ResourceValidationRule& rule) const;
    
    // File format validation
    bool validateJSONFile(const std::string& path) const;
    bool validateYAMLFile(const std::string& path) const;
    bool validatePlainTextFile(const std::string& path) const;
    
    // Error message generation
    ValidationError createValidationError(const std::string& ruleName,
                                         const std::string& errorMsg,
                                         ErrorSeverity severity = ErrorSeverity::ERROR) const;
    
    // Value type conversion helpers
    bool valueToDouble(const ConfigValue& value, double& outValue) const;
    bool valueToString(const ConfigValue& value, std::string& outValue) const;
    bool valueToBool(const ConfigValue& value, bool& outValue) const;
    
public:
    EnhancedConfigValidator();
    
    // Configuration validation rules
    void addParameterRange(const ParameterRange& range);
    void addFileRule(const FileValidationRule& rule);
    void addResourceRule(const ResourceValidationRule& rule);
    void addInputRule(const InputValidationRule& rule);
    
    // Custom validators
    void addCustomValidator(const std::string& parameterName,
                           std::function<bool(const ConfigValue&)> validator,
                           const std::string& description = "");
    
    // Exclusion and strict mode
    void excludeParameter(const std::string& parameterName);
    void setStrictMode(bool strict) { strictMode = strict; }
    void setValidationTimeout(std::chrono::milliseconds timeout) { validationTimeout = timeout; }
    
    // Validation methods
    ValidationResult validate(const Config& config) const;
    ValidationResult validateFile(const std::string& filepath, ConfigFileFormat format = ConfigFileFormat::JSON) const;
    ValidationResult validateInput(const std::map<std::string, std::string>& inputMap) const;
    ValidationResult validateResource(const std::string& resourceName, double amount) const;
    
    // Parameter validation
    ValidationResult validateParameter(const std::string& parameterName, const ConfigValue& value) const;
    bool isParameterValid(const std::string& parameterName, const ConfigValue& value) const;
    
    // File validation
    ValidationResult validateFileSystem(const std::vector<std::string>& paths) const;
    
    // Configuration validation
    ValidationResult validateConfigurationStructure(const Config& config) const;
    ValidationResult validateConfigurationValues(const Config& config) const;
    
    // Compatibility and validation helpers
    bool isParameterExcluded(const std::string& parameterName) const { return excludeParameters.find(parameterName) != excludeParameters.end(); }
    bool isStrictMode() const { return strictMode; }
    
    // Get validation statistics
    std::map<std::string, int> getValidationStatistics() const;
    
    // Validation report generation
    std::string generateValidationReport(const ValidationResult& result) const;
    bool exportValidationReport(const ValidationResult& result, const std::string& filepath) const;
};

// Configuration File Format Validator
class ConfigFileValidator {
private:
    std::map<ConfigFileFormat, std::function<bool(const std::string&)>> formatValidators;
    std::map<ConfigFileFormat, std::function<ValidationResult(const std::string&, const Config&)>> formatParsers;
    std::map<std::string, std::function<bool(const std::string&)>> extensionValidators;
    
public:
    ConfigFileValidator();
    
    // Format registration
    void registerFormatValidator(ConfigFileFormat format, std::function<bool(const std::string&)> validator);
    void registerFormatParser(ConfigFileFormat format, std::function<ValidationResult(const std::string&, const Config&)> parser);
    void registerExtensionValidator(const std::string& extension, std::function<bool(const std::string&)> validator);
    
    // Format validation
    bool validateFileFormat(const std::string& filepath, ConfigFileFormat& detectedFormat) const;
    bool validateFileExtension(const std::string& filepath) const;
    ValidationResult parseAndValidate(const std::string& filepath, Config& config) const;
    
    // File format specific validation
    ValidationResult validateJSONFile(const std::string& content, Config& config) const;
    ValidationResult validateYAMLFile(const std::string& content, Config& config) const;
    ValidationResult validatePlainTextFile(const std::string& content, Config& config) const;
    
    // Schema validation
    ValidationResult validateAgainstSchema(const std::string& content, ConfigFileFormat format, const std::string& schemaPath) const;
};

// Parameter Value Validator
class ParameterValidator {
private:
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> typeValidators;
    std::map<std::string, std::vector<ParameterRange>> parameterRanges;
    std::map<std::string, std::string> parameterUnits;
    std::map<std::string, std::string> parameterDescriptions;
    std::map<std::string, std::vector<std::string>> validEnumValues;
    
    // Value conversion and validation
    bool validateValueType(const ConfigValue& value, const std::string& expectedType) const;
    bool validateEnum(const ConfigValue& value, const std::string& enumName) const;
    bool validateRange(const ConfigValue& value, const std::string& parameterName) const;
    
public:
    ParameterValidator();
    
    // Parameter registration
    void registerParameter(const std::string& name, const std::string& type,
                          const std::string& unit = "",
                          const std::string& description = "");
    void registerEnumValues(const std::string& parameterName, const std::vector<std::string>& values);
    void registerRangeValidator(const std::string& parameterName, const ParameterRange& range);
    void registerTypeValidator(const std::string& type, std::function<bool(const ConfigValue&)> validator);
    
    // Validation
    ValidationResult validateParameter(const std::string& parameterName, const ConfigValue& value) const;
    bool isValueValid(const std::string& parameterName, const ConfigValue& value) const;
    
    // Get parameter info
    std::string getParameterType(const std::string& parameterName) const;
    std::string getParameterUnit(const std::string& parameterName) const;
    std::string getParameterDescription(const std::string& parameterName) const;
    std::vector<std::string> getValidEnumValues(const std::string& parameterName) const;
    std::vector<ParameterRange> getParameterRanges(const std::string& parameterName) const;
    
    // Batch validation
    ValidationResult validateAllParameters(const Config& config) const;
    std::map<std::string, std::vector<ValidationError>> getAllValidationErrors(const Config& config) const;
};

// Configuration Loader with Enhanced Validation
class ValidatingConfigLoader {
private:
    EnhancedConfigValidator validator;
    ConfigFileValidator fileValidator;
    ParameterValidator parameterValidator;
    std::map<std::string, std::function<ValidationResult(const std::string&, Config&)>> customLoaders;
    
public:
    ValidatingConfigLoader();
    
    // Load and validate configuration
    ValidationResult loadConfig(const std::string& filepath, Config& config,
                               ConfigFileFormat format = ConfigFileFormat::JSON) const;
    
    ValidationResult loadConfigFromString(const std::string& content, Config& config,
                                          ConfigFileFormat format = ConfigFileFormat::JSON) const;
    
    ValidationResult loadConfigFromArgs(int argc, char** argv, Config& config) const;
    
    // Configuration validation
    ValidationResult validateConfig(const Config& config) const;
    
    // Parameter validation
    ValidationResult validateParameters(const Config& config) const;
    
    // File validation
    ValidationResult validateConfigFile(const std::string& filepath, ConfigFileFormat format = ConfigFileFormat::JSON) const;
    
    // Error recovery
    ValidationResult attemptRecovery(const ValidationResult& validationResult, Config& config) const;
    
    // Get validation statistics
    std::map<std::string, int> getValidationStats() const;
    
    // Configuration presets with validation
    ValidationResult loadPresetProfile(const std::string& profileName, Config& config) const;
    std::vector<std::string> getAvailableProfiles() const;
};

// Global validation instances
extern EnhancedConfigValidator globalValidator;
extern ConfigFileValidator globalFileValidator;
extern ParameterValidator globalParameterValidator;
extern ValidatingConfigLoader globalConfigLoader;

// Validation macros
#define NLM_VALIDATE_CONFIG(config) \
    do { \
        auto validationResult = nlm::globalValidator.validate(config); \
        if (validationResult.hasErrors()) { \
            for (const auto& error : validationResult.errors) { \
                throw nlm::ConfigError(error.what(), error.getSeverity(), error.getComponent(), error.getErrorCode()); \
            } \
        } \
    } while(0)

#define NLM_VALIDATE_FILE(filepath) \
    do { \
        auto validationResult = nlm::globalFileValidator.validateFile(filepath, ConfigFileFormat::JSON); \
        if (validationResult.hasErrors()) { \
            for (const auto& error : validationResult.errors) { \
                throw nlm::FileError(error.what(), error.getSeverity(), error.getComponent(), error.getErrorCode()); \
            } \
        } \
    } while(0)

#define NLM_VALIDATE_PARAMETER(param_name, value) \
    do { \
        auto validationResult = nlm::globalParameterValidator.validateParameter(param_name, value); \
        if (!validationResult.valid) { \
            for (const auto& error : validationResult.errors) { \
                throw nlm::ValidationError(error.what(), error.getSeverity(), error.getComponent(), error.getErrorCode()); \
            } \
        } \
    } while(0)