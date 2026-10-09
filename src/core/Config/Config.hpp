#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <stdexcept>
#include <functional>
#include <any>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <ctime>
#include <limits>
#include <tuple>

namespace nlm {

// Configuration error types
enum class ValidationErrorType {
    None,
    TypeMismatch,
    RangeViolation,
    FormatViolation,
    DependencyViolation,
    EnumViolation,
    MissingRequired,
    Conflict,
    RuntimeError
};

// Validation error severity
enum class ValidationSeverity {
    Warning,
    Error,
    Fatal
};

// Configuration validation error with context
class ConfigurationError : public std::runtime_error {
public:
    ConfigurationError(const std::string& message,
                      ValidationErrorType type = ValidationErrorType::None,
                      ValidationSeverity severity = ValidationSeverity::Error,
                      const std::string& key = "",
                      const std::string& suggestion = "")
        : std::runtime_error(message), type_(type), severity_(severity),
          key_(key), suggestion_(suggestion) {}
    
    ValidationErrorType type() const { return type_; }
    ValidationSeverity severity() const { return severity_; }
    const std::string& key() const { return key_; }
    const std::string& suggestion() const { return suggestion_; }
    
    std::string formatted() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity_) << "]";
        oss << " [" << static_cast<int>(type_) << "]";
        if (!key_.empty()) oss << " key='" << key_ << "'";
        oss << " " << what();
        if (!suggestion_.empty()) {
            oss << "\n  Suggestion: " << suggestion_;
        }
        return oss.str();
    }
    
private:
    ValidationErrorType type_;
    ValidationSeverity severity_;
    std::string key_;
    std::string suggestion_;
};

// Validation result with error reporting
class ValidationResult {
public:
    ValidationResult() : valid_(true), errors_() {}
    
    ValidationResult& addError(const ConfigurationError& error) {
        errors_.push_back(error);
        if (error.severity() == ValidationSeverity::Fatal || 
            error.severity() == ValidationSeverity::Error) {
            valid_ = false;
        }
        return *this;
    }
    
    ValidationResult& addWarning(const ConfigurationError& error) {
        if (error.severity() == ValidationSeverity::Warning) {
            errors_.push_back(error);
        }
        return *this;
    }
    
    bool isValid() const { return valid_; }
    bool hasWarnings() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ConfigurationError& e) { return e.severity() == ValidationSeverity::Warning; });
    }
    
    bool hasErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
            [](const ConfigurationError& e) { return e.severity() != ValidationSeverity::Warning; });
    }
    
    const std::vector<ConfigurationError>& errors() const { return errors_; }
    size_t errorCount() const { return errors_.size(); }
    
    std::string toString() const {
        if (errors_.empty()) return "Validation passed.";
        
        std::ostringstream oss;
        oss << "Validation failed with " << errors_.size() << " error(s):\n";
        for (const auto& error : errors_) {
            oss << "  " << error.formatted() << "\n";
        }
        return oss.str();
    }
    
private:
    bool valid_;
    std::vector<ConfigurationError> errors_;
};

// Enumeration for enum validation
class EnumValidator {
public:
    template<typename Enum>
    static ValidationResult validate(const std::string& key, 
                                    const std::any& value,
                                    const std::vector<Enum>& allowedValues,
                                    const std::string& description = "") {
        ValidationResult result;
        
        try {
            Enum val = std::any_cast<Enum>(value);
            if (std::find(allowedValues.begin(), allowedValues.end(), val) == allowedValues.end()) {
                result.addError(ConfigurationError(
                    "Value not in allowed enum values",
                    ValidationErrorType::EnumViolation,
                    ValidationSeverity::Error,
                    key,
                    "Choose from: " + enumToString(allowedValues)
                ));
            }
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match enum type",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                key,
                "Expected enum type " + getTypeName<Enum>()
            ));
        }
        
        return result;
    }
    
    template<typename Enum>
    static std::string enumToString(const std::vector<Enum>& values) {
        std::ostringstream oss;
        bool first = true;
        for (const auto& val : values) {
            if (!first) oss << ", ";
            oss << static_cast<int>(val);
            first = false;
        }
        return oss.str();
    }
    
private:
    template<typename T>
    static std::string getTypeName() {
        return typeid(T).name();
    }
};

// Numeric validation utilities
class NumericValidator {
public:
    template<typename T>
    static ValidationResult validate(const std::string& key,
                                    const std::any& value,
                                    T minValue = std::numeric_limits<T>::lowest(),
                                    T maxValue = std::numeric_limits<T>::max(),
                                    const std::string& description = "") {
        ValidationResult result;
        
        try {
            T val = std::any_cast<T>(value);
            
            if (val < minValue) {
                result.addError(ConfigurationError(
                    "Value " + std::to_string(val) + " is below minimum " + std::to_string(minValue),
                    ValidationErrorType::RangeViolation,
                    ValidationSeverity::Error,
                    key,
                    "Set to at least " + std::to_string(minValue)
                ));
            }
            
            if (val > maxValue) {
                result.addError(ConfigurationError(
                    "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(maxValue),
                    ValidationErrorType::RangeViolation,
                    ValidationSeverity::Error,
                    key,
                    "Set to at most " + std::to_string(maxValue)
                ));
            }
            
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match numeric type",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                key,
                "Expected " + getTypeName<T>()
            ));
        }
        
        return result;
    }
    
    template<typename T>
    static std::string getTypeName() {
        return typeid(T).name();
    }
};

// String validation utilities
class StringValidator {
public:
    static ValidationResult validate(const std::string& key,
                                    const std::any& value,
                                    size_t minLength = 0,
                                    size_t maxLength = std::numeric_limits<size_t>::max(),
                                    const std::string& pattern = "",
                                    const std::string& description = "") {
        ValidationResult result;
        
        try {
            std::string val = std::any_cast<std::string>(value);
            
            if (val.length() < minLength) {
                result.addError(ConfigurationError(
                    "String length " + std::to_string(val.length()) + " is below minimum " + std::to_string(minLength),
                    ValidationErrorType::RangeViolation,
                    ValidationSeverity::Error,
                    key,
                    "String must have at least " + std::to_string(minLength) + " characters"
                ));
            }
            
            if (val.length() > maxLength) {
                result.addError(ConfigurationError(
                    "String length " + std::to_string(val.length()) + " exceeds maximum " + std::to_string(maxLength),
                    ValidationErrorType::RangeViolation,
                    ValidationSeverity::Error,
                    key,
                    "String must have at most " + std::to_string(maxLength) + " characters"
                ));
            }
            
            if (!pattern.empty()) {
                std::regex regex(pattern);
                if (!std::regex_match(val, regex)) {
                    result.addError(ConfigurationError(
                        "String does not match required pattern",
                        ValidationErrorType::FormatViolation,
                        ValidationSeverity::Error,
                        key,
                        "String must match pattern: " + pattern
                    ));
                }
            }
            
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match string type",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                key,
                "Expected std::string type"
            ));
        }
        
        return result;
    }
};

// Date/time validation utilities
class DateTimeValidator {
public:
    static ValidationResult validate(const std::string& key,
                                    const std::any& value,
                                    bool isDate = false,
                                    bool isTime = false,
                                    const std::string& minDate = "",
                                    const std::string& maxDate = "",
                                    const std::string& format = "") {
        ValidationResult result;
        
        try {
            std::string val = std::any_cast<std::string>(value);
            
            // Simple format validation
            if (!format.empty()) {
                // Basic ISO format validation
                if (format == "iso-date") {
                    if (!isISODate(val)) {
                        result.addError(ConfigurationError(
                            "Date does not match ISO format (YYYY-MM-DD)",
                            ValidationErrorType::FormatViolation,
                            ValidationSeverity::Error,
                            key,
                            "Use format: YYYY-MM-DD"
                        ));
                    } else if (!minDate.empty() && val < minDate) {
                        result.addError(ConfigurationError(
                            "Date " + val + " is before minimum " + minDate,
                            ValidationErrorType::RangeViolation,
                            ValidationSeverity::Error,
                            key,
                            "Date must be on or after " + minDate
                        ));
                    } else if (!maxDate.empty() && val > maxDate) {
                        result.addError(ConfigurationError(
                            "Date " + val + " is after maximum " + maxDate,
                            ValidationErrorType::RangeViolation,
                            ValidationSeverity::Error,
                            key,
                            "Date must be on or before " + maxDate
                        ));
                    }
                }
                
                else if (format == "iso-time") {
                    if (!isIOSTime(val)) {
                        result.addError(ConfigurationError(
                            "Time does not match ISO format (HH:MM:SS)",
                            ValidationErrorType::FormatViolation,
                            ValidationSeverity::Error,
                            key,
                            "Use format: HH:MM:SS"
                        ));
                    }
                }
            }
            
        } catch (const std::bad_any_cast&) {
            result.addError(ConfigurationError(
                "Value type does not match string type",
                ValidationErrorType::TypeMismatch,
                ValidationSeverity::Error,
                key,
                "Expected std::string type for date/time"
            ));
        }
        
        return result;
    }
    
private:
    static bool isISODate(const std::string& date) {
        // Simple ISO date format validation (YYYY-MM-DD)
        if (date.length() != 10) return false;
        
        for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) {
            if (!std::isdigit(date[i])) return false;
        }
        
        if (date[4] != '-' || date[7] != '-') return false;
        
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));
        
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        
        // Simple leap year check
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (month == 2 && day > (isLeap ? 29 : 28)) return false;
        
        return true;
    }
    
    static bool isIOSTime(const std::string& time) {
        // Simple ISO time format validation (HH:MM:SS)
        if (time.length() != 8) return false;
        
        for (int i : {0, 1, 3, 4, 6, 7}) {
            if (!std::isdigit(time[i])) return false;
        }
        
        if (time[2] != ':' || time[5] != ':') return false;
        
        int hour = std::stoi(time.substr(0, 2));
        int minute = std::stoi(time.substr(3, 2));
        int second = std::stoi(time.substr(6, 2));
        
        if (hour < 0 || hour > 23) return false;
        if (minute < 0 || minute > 59) return false;
        if (second < 0 || second > 59) return false;
        
        return true;
    }
};

// Parameter dependency validator
class DependencyValidator {
public:
    struct Dependency {
        std::string parameter;
        std::string operation; // "exists", "equals", "greater", "less"
        std::any value;
        std::string description;
    };
    
    static ValidationResult validate(const std::string& key,
                                    const std::any& value,
                                    const std::vector<Dependency>& dependencies) {
        ValidationResult result;
        
        // This would require access to the full Config object to check dependencies
        // For simplicity, we'll assume this is called with proper context
        // In a real implementation, this would need to be passed the Config instance
        
        return result;
    }
};

// Parameter grouping for organization
class ParameterGroup {
public:
    ParameterGroup(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    
    void addParameter(const std::string& key) { parameters_.insert(key); }
    
    bool hasParameter(const std::string& key) const {
        return parameters_.find(key) != parameters_.end();
    }
    
    const std::set<std::string>& parameters() const { return parameters_; }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << name_;
        if (!description_.empty()) {
            oss << " (" << description_ << ")";
        }
        oss << ": " << parameters_.size() << " parameters";
        return oss.str();
    }
    
private:
    std::string name_;
    std::string description_;
    std::set<std::string> parameters_;
};

// Configuration schema definition
class ConfigSchema {
public:
    struct ParameterInfo {
        std::string key;
        std::string type;
        std::any defaultValue;
        std::string description;
        bool required = false;
        
        // Validation constraints
        std::optional<std::any> minValue;
        std::optional<std::any> maxValue;
        std::vector<std::any> allowedValues;
        std::string pattern;
        std::string format;
        
        // Dependencies
        std::vector<DependencyValidator::Dependency> dependencies;
        
        // Python integration
        std::string pythonType;
        bool pythonOptional = false;
    };
    
    ConfigSchema(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}
    
    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    
    void addParameter(const ParameterInfo& param) {
        parameters_[param.key] = param;
    }
    
    bool hasParameter(const std::string& key) const {
        return parameters_.find(key) != parameters_.end();
    }
    
    const ParameterInfo& getParameter(const std::string& key) const {
        return parameters_.at(key);
    }
    
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(parameters_.size());
        for (const auto& pair : parameters_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Schema: " << name_;
        if (!description_.empty()) {
            oss << " (" << description_ << ")";
        }
        oss << "\n  Parameters: " << parameters_.size() << "\n";
        for (const auto& pair : parameters_) {
            oss << "    " << pair.first << " [" << pair.second.type << "]";
            if (!pair.second.description.empty()) {
                oss << ": " << pair.second.description;
            }
            oss << "\n";
        }
        return oss.str();
    }
    
private:
    std::string name_;
    std::string description_;
    std::map<std::string, ParameterInfo> parameters_;
};

// Main configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    ValidationError
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Validation state
    mutable ValidationResult validationResult;
    mutable bool validationDirty = true;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    void markValidationDirty() const { validationDirty = true; }
    bool isValidationDirty() const { return validationDirty; }
    void invalidateValidation() const { validationResult = ValidationResult(); }
};

// Main configuration class with validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema() const;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from Python dict (for Python integration)
    bool loadFromPythonDict(const std::map<std::string, std::any>& pythonDict);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set raw value without validation
    void setRaw(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate parameter
    ValidationResult validateParameter(const std::string& key) const;
    
    // Validate all parameters
    ValidationResult validateAll() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all parameter groups
    std::vector<ParameterGroup> getParameterGroups() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation results
    ValidationResult getValidationResults() const { return validationResults_; }
    
    // Check if configuration is valid
    bool isValid() const { return validationResults_.isValid() && validationResults_.errorCount() == 0; }
    
    // Get parameter type name
    std::string getParameterType(const std::string& key) const;
    
    // Get parameter constraints
    std::string getParameterConstraints(const std::string& key) const;
    
    // Python integration helpers
    bool hasPythonType(const std::string& key) const;
    std::string getPythonType(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation state
    mutable ValidationResult validationResults_;
    mutable bool validationDirty_ = true;
    
    // Schema
    std::unique_ptr<ConfigSchema> schema_;
    
    // Parameter groups
    std::vector<ParameterGroup> parameterGroups_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    ValidationResult validateConfigValue(const ConfigEntry& entry) const;
    ValidationResult validateAgainstSchema(const ConfigEntry& entry) const;
    ValidationResult validateDependencies(const ConfigEntry& entry) const;
    ValidationResult validateConditional(const ConfigEntry& entry) const;
    
    // Python integration helpers
    bool isPythonValueCompatible(const std::string& key, const std::any& value) const;
    std::any convertToPythonType(const std::string& key, const ConfigValue& value) const;
};

// Utility functions for validation and conversion
namespace ConfigUtils {
    // Type checking utilities
    template<typename T>
    bool isSameType(const ConfigValue& value);
    
    // Safe conversion utilities
    template<typename T>
    T safeGetValue(const ConfigValue& value);
    
    // Validation macros
    #define VALIDATE_RANGE(value, min, max) \
        do { \
            if (value < min || value > max) { \
                throw ConfigurationError("Value " + std::to_string(value) + " is out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]"); \
            } \
        } while(0)
    
    #define VALIDATE_NOT_EMPTY(str) \
        do { \
            if (str.empty()) { \
                throw ConfigurationError("String cannot be empty"); \
            } \
        } while(0)
    
    #define VALIDATE_ENUM(value, ...) \
        do { \
            bool valid = false; \
            int allowed[] = {__VA_ARGS__}; \
            for (int i = 0; i < sizeof(allowed)/sizeof(int); ++i) { \
                if (static_cast<int>(value) == allowed[i]) { \
                    valid = true; break; \
                } \
            } \
            if (!valid) { \
                throw ConfigurationError("Enum value " + std::to_string(static_cast<int>(value)) + " not in allowed values"); \
            } \
        } while(0)
    
    // String formatting for error messages
    std::string formatError(const std::string& template, 
                           const std::map<std::string, std::string>& placeholders);
    
    // Parameter value formatting for display
    std::string formatValue(const ConfigValue& value);
    
    // Configuration value comparison
    bool valuesEqual(const ConfigValue& a, const ConfigValue& b);
    
    // Hash for ConfigValue
    size_t hashValue(const ConfigValue& value);
}

// Python integration namespace
namespace PythonConfig {
    // Convert C++ Config to Python dict
    std::map<std::string, std::any> toPythonDict(const Config& config);
    
    // Load from Python dict
    bool loadFromPythonDict(Config& config, const std::map<std::string, std::any>& pythonDict);
    
    // Validation for Python types
    ValidationResult validatePythonType(const std::string& key, const std::any& value, const std::string& pythonType);
    
    // Get Python type for key
    std::string getPythonTypeForKey(const Config& config, const std::string& key);
    
    // Check if key exists in Python dict
    bool hasPythonKey(const std::map<std::string, std::any>& pythonDict, const std::string& key);
    
    // Get value from Python dict with type conversion
    template<typename T>
    T getFromPythonDict(const std::map<std::string, std::any>& pythonDict, const std::string& key);
}

// Forward declarations
class ConfigSchemaBuilder;

// Builder pattern for ConfigSchema
class ConfigSchemaBuilder {
public:
    ConfigSchemaBuilder(const std::string& name, const std::string& description = "")
        : schema_(std::make_unique<ConfigSchema>(name, description)) {}
    
    ConfigSchemaBuilder& addParameter(const std::string& key,
                                      const std::string& type,
                                      const std::any& defaultValue,
                                      const std::string& description,
                                      bool required = false) {
        ConfigSchema::ParameterInfo param;
        param.key = key;
        param.type = type;
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        schema_->addParameter(param);
        return *this;
    }
    
    ConfigSchemaBuilder& addIntParameter(const std::string& key,
                                         int defaultValue,
                                         int min = std::numeric_limits<int>::lowest(),
                                         int max = std::numeric_limits<int>::max(),
                                         const std::string& description = "",
                                         bool required = false) {
        ConfigSchema::ParameterInfo param;
        param.key = key;
        param.type = "int";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.minValue = min;
        param.maxValue = max;
        schema_->addParameter(param);
        return *this;
    }
    
    ConfigSchemaBuilder& addDoubleParameter(const std::string& key,
                                            double defaultValue,
                                            double min = -std::numeric_limits<double>::max(),
                                            double max = std::numeric_limits<double>::max(),
                                            const std::string& description = "",
                                            bool required = false) {
        ConfigSchema::ParameterInfo param;
        param.key = key;
        param.type = "double";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.minValue = min;
        param.maxValue = max;
        schema_->addParameter(param);
        return *this;
    }
    
    ConfigSchemaBuilder& addStringParameter(const std::string& key,
                                           const std::string& defaultValue,
                                           size_t minLength = 0,
                                           size_t maxLength = std::numeric_limits<size_t>::max(),
                                           const std::string& pattern = "",
                                           const std::string& description = "",
                                           bool required = false) {
        ConfigSchema::ParameterInfo param;
        param.key = key;
        param.type = "string";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        // Store pattern as string (implementation detail for min/max length)
        if (minLength > 0 || maxLength < std::numeric_limits<size_t>::max()) {
            param.pattern = "length:" + std::to_string(minLength) + "," + std::to_string(maxLength);
        }
        if (!pattern.empty()) {
            if (!param.pattern.empty()) param.pattern += ";";
            param.pattern += pattern;
        }
        schema_->addParameter(param);
        return *this;
    }
    
    ConfigSchemaBuilder& addEnumParameter(const std::string& key,
                                          const std::any& defaultValue,
                                          const std::vector<std::any>& allowedValues,
                                          const std::string& description = "",
                                          bool required = false) {
        ConfigSchema::ParameterInfo param;
        param.key = key;
        param.type = "enum";
        param.defaultValue = defaultValue;
        param.description = description;
        param.required = required;
        param.allowedValues = allowedValues;
        schema_->addParameter(param);
        return *this;
    }
    
    ConfigSchemaBuilder& addGroup(const std::string& name, const std::string& description = "") {
        parameterGroups_.emplace_back(name, description);
        return *this;
    }
    
    ConfigSchemaBuilder& addDependency(const std::string& key,
                                       const std::string& operation,
                                       const std::any& value,
                                       const std::string& description) {
        if (!parameterGroups_.empty()) {
            parameterGroups_.back().addParameter(key);
        }
        
        if (!schema_->hasParameter(key)) {
            ConfigSchema::ParameterInfo param;
            param.key = key;
            param.type = "unknown";
            param.required = false;
            schema_->addParameter(param);
        }
        
        DependencyValidator::Dependency dep;
        dep.parameter = key;
        dep.operation = operation;
        dep.value = value;
        dep.description = description;
        
        auto& param = const_cast<ConfigSchema::ParameterInfo&>(schema_->getParameter(key));
        param.dependencies.push_back(dep);
        
        return *this;
    }
    
    ConfigSchema build() {
        return *schema_;
    }
    
private:
    std::unique_ptr<ConfigSchema> schema_;
    std::vector<ParameterGroup> parameterGroups_;
};

// Global schema builder function
inline ConfigSchemaBuilder makeSchema(const std::string& name, const std::string& description = "") {
    return ConfigSchemaBuilder(name, description);
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    
    template bool ConfigUtils::isSameType<int>(const ConfigValue& value);
    template bool ConfigUtils::isSameType<int64_t>(const ConfigValue& value);
    template bool ConfigUtils::isSameType<double>(const ConfigValue& value);
    template bool ConfigUtils::isSameType<bool>(const ConfigValue& value);
    template bool ConfigUtils::isSameType<std::string>(const ConfigValue& value);
    
    template int ConfigUtils::safeGetValue<int>(const ConfigValue& value);
    template int64_t ConfigUtils::safeGetValue<int64_t>(const ConfigValue& value);
    template double ConfigUtils::safeGetValue<double>(const ConfigValue& value);
    template bool ConfigUtils::safeGetValue<bool>(const ConfigValue& value);
    template std::string ConfigUtils::safeGetValue<std::string>(const ConfigValue& value);
    
    template T PythonConfig::getFromPythonDict<int>(const std::map<std::string, std::any>& pythonDict, const std::string& key);
    template int64_t PythonConfig::getFromPythonDict<int64_t>(const std::map<std::string, std::any>& pythonDict, const std::string& key);
    template double PythonConfig::getFromPythonDict<double>(const std::map<std::string, std::any>& pythonDict, const std::string& key);
    template bool PythonConfig::getFromPythonDict<bool>(const std::map<std::string, std::any>& pythonDict, const std::string& key);
    template std::string PythonConfig::getFromPythonDict<std::string>(const std::map<std::string, std::any>& pythonDict, const std::string& key);
}