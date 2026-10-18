#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <set>
#include <cmath>
#include <algorithm>
#include <filesystem>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

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

// Error categories
enum class ValidationErrorCategory {
    TypeError,
    RangeError,
    LogicError,
    SchemaError,
    FileError,
    SystemError
};

// Individual validation error
struct ValidationError {
    ValidationErrorCategory category;
    std::string parameter;
    std::string message;
    std::string suggestion;
    double actualValue;
    double minValue;
    double maxValue;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationError()
        : category(ValidationErrorCategory::TypeError),
          actualValue(0.0), minValue(0.0), maxValue(0.0), timestamp(std::chrono::system_clock::now()) {}
    
    ValidationError(ValidationErrorCategory cat, const std::string& param, const std::string& msg,
                   const std::string& suggest = "", double actual = 0.0, double min = 0.0, double max = 0.0)
        : category(cat), parameter(param), message(msg), suggestion(suggest),
          actualValue(actual), minValue(min), maxValue(max),
          timestamp(std::chrono::system_clock::now()) {}
};

// Validation result summary
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::vector<std::string> warnings;
    size_t validatedParameters;
    double validationTimeMs;
    
    ValidationResult() : valid(true), validatedParameters(0), validationTimeMs(0.0) {}
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Validation " << (valid ? "PASSED" : "FAILED") << ": " << errors.size() << " errors, " << warnings.size() << " warnings";
        if (!valid && !errors.empty()) {
            oss << " - First error: " << errors[0].message;
        }
        return oss.str();
    }
};

// Custom exception for validation errors
class ValidationException : public std::runtime_error {
public:
    ValidationException(const ValidationResult& result) 
        : std::runtime_error("Configuration validation failed"), result(result) {}
    
    const ValidationResult& getResult() const { return result; }
    
private:
    ValidationResult result;
};

// Parameter metadata for schema validation
struct ParameterDefinition {
    std::string name;
    std::string type;
    std::string description;
    ConfigSource defaultSource;
    bool required;
    double minValue;
    double maxValue;
    std::string unit;
    std::string validationRule;
    std::map<std::string, std::string> constraints;
    
    ParameterDefinition()
        : defaultSource(ConfigSource::Default), required(false), minValue(0.0), maxValue(0.0) {}
    
    ParameterDefinition(const std::string& n, const std::string& t, const std::string& desc = "")
        : name(n), type(t), description(desc), defaultSource(ConfigSource::Default), required(false),
          minValue(0.0), maxValue(0.0) {}
};

// Parameter change event
struct ParameterChangeEvent {
    std::string parameter;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    bool validated;
    std::vector<ValidationError> errors;
    
    ParameterChangeEvent() : validated(true), timestamp(std::chrono::system_clock::now()) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> getAs(const std::string& key) const; // Type-safe retrieval with validation
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe setting with validation
    template<typename T>
    void setWithValidation(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Ensure parameter has expected type
    template<typename T>
    bool ensureType(const std::string& key) const;
    
    // Safe type conversion with validation
    template<typename T>
    bool convertType(const std::string& key, const ConfigValue& newValue);
    
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
    
    // Get configuration as JSON string for debugging
    std::string getConfigString() const;
    
    // ===== VALIDATION METHODS =====
    
    // Individual parameter validation
    bool validateNeuronCount() const;
    bool validateSynapseDensity() const;
    bool validateTimeParameters() const;
    bool validatePlasticityParameters() const;
    bool validateNeuromodulationParameters() const;
    bool validateDevelopmentParameters() const;
    
    // Configuration consistency checks
    bool validateConsistency() const;
    bool validateCompleteness() const;
    
    // Advanced validation
    ValidationResult validateConfig() const;
    ValidationResult getValidationReport() const;
    bool validateAgainstSchema() const;
    
    // File validation
    bool validateFilePath(const std::string& filepath) const;
    bool validateFileFormat(const std::string& filepath) const;
    bool validateFileContent(const std::string& content) const;
    
    // Get last validation error
    std::optional<ValidationError> getLastValidationError() const;
    
    // ===== SCHEMA VALIDATION =====
    
    // Add parameter definition
    bool addParameterDefinition(const ParameterDefinition& definition);
    
    // Validate parameter metadata
    bool validateParameterMetadata() const;
    
    // Get metadata about specific parameters
    std::optional<ParameterDefinition> getParameterInfo(const std::string& key) const;
    
    // Export configuration schema
    std::string exportSchema() const;
    
    // ===== RUNTIME MONITORING =====
    
    // Register parameter change callback
    size_t registerParameterChangeCallback(std::function<void(const ParameterChangeEvent&)> callback);
    
    // Unregister parameter change callback
    bool unregisterParameterChangeCallback(size_t callbackId);
    
    // Get parameter history
    std::vector<ParameterChangeEvent> getParameterHistory(const std::string& key) const;
    
    // Validate parameter change
    ValidationResult validateParameterChange(const std::string& key, const ConfigValue& newValue,
                                             ConfigSource source = ConfigSource::Runtime);
    
    // Check if parameter change is safe
    bool isParameterChangeSafe(const std::string& key, const ConfigValue& newValue) const;
    
    // ===== INITIALIZATION AND SETUP =====
    
    // Initialize validation systems
    void initializeValidationSystem();
    
    // Set up parameter schemas
    void setupParameterSchemas();
    
    // Validate configuration before brain setup
    bool validateForBrain() const;
    
    // Get the path used for loading configuration
    std::string getConfigPath() const;
    
    // Set the configuration path
    void setConfigPath(const std::string& path);
    
    // ===== ERROR HANDLING =====
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalValidations;
        size_t successfulValidations;
        size_t failedValidations;
        double averageValidationTimeMs;
        std::map<ValidationErrorCategory, size_t> errorCounts;
    };
    
    ValidationStats getValidationStats() const;
    
    // Reset validation statistics
    void resetValidationStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration source file path
    std::string configPath_;
    
    // Parameter schema definitions
    std::unordered_map<std::string, ParameterDefinition> parameterSchemas_;
    
    // Validation system state
    ValidationResult lastValidationResult_;
    mutable ValidationStats validationStats_;
    
    // Parameter change history
    std::unordered_map<std::string, std::vector<ParameterChangeEvent>> parameterHistory_;
    
    // Callback system
    std::vector<std::function<void(const ParameterChangeEvent&)>> changeCallbacks_;
    size_t nextCallbackId_;
    
    // File validation helpers
    static bool validateFilePermissions(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);
    static std::string parseJsonContent(const std::string& content);
    static std::string configToJson(const Config& config);
    
    // Parameter validation helpers
    template<typename T>
    bool validateType(const std::string& key, const ConfigValue& value) const;
    
    template<typename T>
    bool validateRange(const std::string& key, const T& value) const;
    
    // Type conversion helpers
    template<typename T>
    T convertVariantTo(const ConfigValue& value) const;
    
    // Internal helpers (from original implementation)
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Legacy validation method (for backward compatibility)
    ValidationResult validateConfigInternal() const;
};

} // namespace nlm