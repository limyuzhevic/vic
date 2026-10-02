#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <map>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace nlm {

// Forward declarations
class ConfigSchema;
class ConfigError;
class ConfigValidator;

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
    Environment,
    Runtime,
    Inherited
};

// Error codes for configuration errors
enum class ConfigErrorCode {
    None,
    FileNotFound,
    InvalidFileFormat,
    InvalidSyntax,
    MissingRequiredField,
    InvalidType,
    OutOfRange,
    CrossParameterValidationFailed,
    PermissionDenied,
    FileWriteError,
    SchemaValidationFailed,
    MissingEnvironmentVariable,
    InvalidEnvironmentValue,
    UnknownError
};

// Detailed error information
struct ErrorInfo {
    ConfigErrorCode code;
    std::string message;
    std::string file;
    size_t line;
    std::string context;
    std::chrono::system_clock::time_point timestamp;
};

// Configuration validation rule types
enum class ValidationRule {
    None,
    Required,
    TypeCheck,
    RangeValidation,
    PatternValidation,
    CrossParameterValidation,
    CustomValidation
};

// Configuration validation entry
struct ValidationRuleInfo {
    ValidationRule type;
    std::string field;
    std::string message;
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> formatter;
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;  // Type hint (e.g., "int", "string", "double")
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string pattern;  // Regex pattern for string validation
    std::vector<std::string> requiredFields;  // Cross-parameter validation
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), 
                   timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration change tracking
struct ConfigChange {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string reason;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t entriesBySource[5]; // 0:Default, 1:File, 2:CommandLine, 3:Environment, 4:Runtime, 5:Inherited
    size_t validationErrors;
    std::chrono::system_clock::time_point lastValidation;
};

// Configuration implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::mutex mutex;  // Thread safety for configuration
    std::vector<ConfigChange> changeHistory;
    std::map<std::string, std::string> environmentVariables;
    std::map<std::string, std::shared_ptr<ConfigSchema>> inheritedSchemas;
    ConfigStats stats;
    bool isValid;
    std::vector<ErrorInfo> lastValidationErrors;
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
    
    // Configuration management
    bool validateSchema() const;
    std::vector<ErrorInfo> getValidationErrors() const;
    void addSchema(const std::string& key, std::shared_ptr<ConfigSchema> schema);
    void inheritFrom(const Config& other, const std::string& prefix = "");
    void loadEnvironmentVariables();
    
    // Configuration management
    bool validateSchema() const;
    std::vector<ErrorInfo> getValidationErrors() const;
    void addSchema(const std::string& key, std::shared_ptr<ConfigSchema> schema);
    void inheritFrom(const Config& other, const std::string& prefix = "");
    void loadEnvironmentVariables();
    void clearHistory();
    std::vector<ConfigChange> getChangeHistory() const;
    ConfigStats getStats() const;
    std::string getChangeSummary(size_t maxHistory = 10) const;
    
    // Configuration validation
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidationIssues() const;
    
    // Configuration tracking
    void trackChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue,
                     ConfigSource source, const std::string& reason = "");
    
    // Configuration analysis
    std::string analyzeConfig() const;
    
    // Configuration initialization
    void initializeDefaults();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration logging
    void logChanges(std::ostream& output) const;
    void setLoggingEnabled(bool enabled);
    bool isLoggingEnabled() const;
    
    // Configuration performance
    size_t getEstimatedMemoryUsage() const;
    double getValidationTime() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static std::optional<ConfigValue> parseValue(const std::string& valueStr);
    static std::string valueToString(const ConfigValue& value);
    static bool compareValues(const ConfigValue& a, const ConfigValue& b);
    static ConfigErrorCode detectErrorCode(const std::string& message);
    
    // Internal validation helpers
    bool validateValueType(const std::string& key, const ConfigValue& value) const;
    bool validateValueRange(const std::string& key, const ConfigValue& value) const;
    bool validateValuePattern(const std::string& key, const ConfigValue& value) const;
    bool validateCrossParameters(const std::string& key) const;
    
    // Internal file helpers
    static std::string getBackupPath(const std::string& filepath);
    static bool createBackup(const std::string& filepath);
    static bool restoreFromBackup(const std::string& filepath);
    static bool isFileReadable(const std::string& filepath);
    static bool isFileWritable(const std::string& filepath);
};

} // namespace nlm
