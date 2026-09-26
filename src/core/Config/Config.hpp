#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <unordered_map>
#include <stdexcept>
#include <chrono>
#include <typeinfo>
#include <sstream>

namespace nlm {

// ============= EXCEPTION HIERARCHY =============

// Base NLM exception
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMException() = default;
    virtual const char* type() const noexcept { return "NLMException"; }
};

// Configuration-related exceptions
class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("ConfigError: " + message) {}
    virtual const char* type() const noexcept { return "ConfigError"; }
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& key, const std::string& message)
        : ConfigError("ConfigValidationError for key '" + key + "': " + message) {}
    virtual const char* type() const noexcept { return "ConfigValidationError"; }
};

class ConfigSchemaError : public ConfigError {
public:
    explicit ConfigSchemaError(const std::string& message) : ConfigError("ConfigSchemaError: " + message) {}
    virtual const char* type() const noexcept { return "ConfigSchemaError"; }
};

class ConfigMergeError : public ConfigError {
public:
    explicit ConfigMergeError(const std::string& message) : ConfigError("ConfigMergeError: " + message) {}
    virtual const char* type() const noexcept { return "ConfigMergeError"; }
};

// Integration-related exceptions
class IntegrationError : public NLMException {
public:
    explicit IntegrationError(const std::string& message) : NLMException("IntegrationError: " + message) {}
    virtual const char* type() const noexcept { return "IntegrationError"; }
};

class BrainInitializationError : public IntegrationError {
public:
    explicit BrainInitializationError(const std::string& message) : IntegrationError("BrainInitializationError: " + message) {}
    virtual const char* type() const noexcept { return "BrainInitializationError"; }
};

class MemorySystemError : public IntegrationError {
public:
    explicit MemorySystemError(const std::string& message) : IntegrationError("MemorySystemError: " + message) {}
    virtual const char* type() const noexcept { return "MemorySystemError"; }
};

// Memory-related exceptions
class MemoryError : public NLMException {
public:
    explicit MemoryError(const std::string& message) : NLMException("MemoryError: " + message) {}
    virtual const char* type() const noexcept { return "MemoryError"; }
};

class PlasticityError : public MemoryError {
public:
    explicit PlasticityError(const std::string& message) : MemoryError("PlasticityError: " + message) {}
    virtual const char* type() const noexcept { return "PlasticityError"; }
};

class SynapticError : public MemoryError {
public:
    explicit SynapticError(const std::string& message) : MemoryError("SynapticError: " + message) {}
    virtual const char* type() const noexcept { return "SynapticError"; }
};

// ============= VALIDATION INFRASTRUCTURE =============

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::vector<ConfigValidationError> errors;
    std::vector<std::string> warnings;
    std::map<std::string, double> performanceMetrics;
    std::chrono::milliseconds validationTime;
    
    ValidationResult() : isValid(true), validationTime(0) {}
    
    void addError(const std::string& key, const std::string& message, ConfigSource source = ConfigSource::Runtime) {
        errors.emplace_back(key, message, source);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Validation " << (isValid ? "PASSED" : "FAILED");
        if (errors.size() > 0) ss << ", " << errors.size() << " errors";
        if (warnings.size() > 0) ss << ", " << warnings.size() << " warnings";
        ss << " in " << validationTime.count() << " ms";
        return ss.str();
    }
    
    void clear() {
        isValid = true;
        errors.clear();
        warnings.clear();
        performanceMetrics.clear();
        validationTime = std::chrono::milliseconds(0);
    }
};

// Configuration value types
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
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// ============= CONFIGURATION SCHEMA SYSTEM =============

class ConfigSchema {
public:
    struct ValidationRule {
        std::string key;
        std::type_index type;
        std::string validationPattern;
        std::variant<int, double> minValue;
        std::variant<int, double> maxValue;
        bool required;
        std::string defaultValue;
        std::function<bool(const ConfigValue&)> customValidator;
        
        ValidationRule() : type(typeid(void)), required(false) {}
        ValidationRule(const std::string& k, const std::type_index& t) : key(k), type(t), required(false) {}
    };
    
    ConfigSchema();
    ~ConfigSchema();
    
    void addRule(const ValidationRule& rule);
    void addRule(const std::string& key, const std::type_index& type, bool required = false,
                 const std::variant<int, double>& minValue = nullptr, 
                 const std::variant<int, double>& maxValue = nullptr,
                 const std::string& defaultValue = "");
    void addStringRule(const std::string& key, size_t minLength = 0, size_t maxLength = 0,
                       const std::string& pattern = "", bool required = false);
    void addNumericRule(const std::string& key, const std::type_index& type, 
                        const std::variant<int, double>& minValue = nullptr,
                        const std::variant<int, double>& maxValue = nullptr,
                        bool required = false);
    void addRangeRule(const std::string& key, double minValue, double maxValue,
                      bool required = false);
    void addListRule(const std::string& key, const std::type_index& elementType,
                     size_t minItems = 0, size_t maxItems = 0, bool required = false);
    
    ValidationResult validate(const Config& config) const;
    bool hasRule(const std::string& key) const;
    std::vector<ValidationRule> getRules() const { return validationRules; }
    
    void injectDefaults(Config& config) const;
    
    static ConfigSchema getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ValidationRule> validationRules;
    
    bool validateValueAgainstRule(const ConfigValue& value, const ValidationRule& rule) const;
    std::type_index getTypeIndexFromVariant(const ConfigValue& value) const;
};

// ============= CONFIGURATION HEALTH MONITORING =============

class ConfigHealthMonitor {
public:
    struct HealthMetric {
        std::string metricName;
        double value;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        bool isHealthy;
    };
    
    ConfigHealthMonitor();
    ~ConfigHealthMonitor();
    
    void recordMetric(const std::string& name, double value, const std::string& unit = "", bool isHealthy = true);
    ValidationResult checkHealth() const;
    void addHealthCheck(std::function<ValidationResult()> check);
    
    std::vector<HealthMetric> getMetrics() const { return metrics; }
    double getMetricAverage(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<HealthMetric> metrics;
    std::vector<std::function<ValidationResult()>> healthChecks;
};

// ============= MAIN CONFIGURATION CLASS =============

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // ============= ENHANCED VALIDATION FUNCTIONS ==========
    
    // Enhanced validation methods
    ValidationResult validate() const;                                    // Validate against schema
    ValidationResult validateAgainstSchema(const ConfigSchema& schema) const;  // Schema-based validation
    std::vector<ConfigValidationError> getValidationErrors() const;    // Get all errors at once
    bool fixCommonIssues();                                              // Automatically fix common configuration problems
    
    // Configuration merging
    bool mergeConfigs(const Config& other, ConfigSource source = ConfigSource::Runtime);  // Merge multiple configuration sources
    
    // Python interoperability
    std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> toPythonDict() const;  // Convert to Python-like dict
    bool fromPythonDict(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>>& dict);  // Import from Python-like dict
    
    // Additional validation methods
    bool validateRuntime() const;                                         // Runtime configuration validation
    ValidationResult validateHealth() const;                              // Configuration health validation
    bool repairConfiguration();                                           // Automatic configuration repair
    
    // ============= ADVANCED CONFIGURATION FUNCTIONS ==========
    
    // Configure multiple settings at once
    bool configureBatch(const std::map<std::string, ConfigValue>& configMap,
                        ConfigSource source = ConfigSource::Runtime);
    
    // Get detailed configuration summary as Python-like dict structure
    std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::map<std::string, std::string>>> getConfigSummary() const;
    
    // Validate configuration and return errors (original method for backward compatibility)
    std::vector<ConfigValidationError> validateConfig() const;
    
    // Reset configuration to defaults
    void resetConfig();
    
    // Get configuration value by type with error checking
    template<typename T>
    std::vector<std::pair<std::string, T>> getAll(const std::string& keyPattern = "*") const;
    
    // Check if configuration is complete (required keys exist)
    bool isComplete(const std::set<std::string>& requiredKeys) const;
    
    // Merge configuration from another config object
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration statistics
    std::map<std::string, size_t> getStatistics() const;
    
    // ============= VALIDATION SPECIFIC METHODS ==========
    
    // Cross-validation between configuration sections
    ValidationResult crossValidate() const;
    
    // Integration validation
    ValidationResult validateIntegration() const;
    
    // Configuration statistics
    std::map<std::string, double> getValidationStatistics() const;
    
    // Performance tracking
    void recordValidationPerformance(const std::string& operation, double timeMs);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool keyMatchesPattern(const std::string& key, const std::string& pattern);
    
    // Private validation helpers
    ValidationResult validateCoreRules() const;
    ValidationResult validateAgainstBuiltInSchema() const;
    bool fixEmptyKeys();
    bool fixInvalidTypes();
    bool fixOutOfRangeValues();
};

} // namespace nlm
