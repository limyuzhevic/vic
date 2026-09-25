#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <map>
#include <set>

namespace nlm {

// Forward declarations
class Config;

// Enhanced Error handling
namespace Error {

enum class ErrorCode {
    // Configuration errors
    ConfigFileNotFound,
    InvalidConfigValue,
    MissingRequiredConfig,
    ConfigValidationFailed,
    ConfigLoadError,
    ConfigSaveError,
    InvalidJSON,
    InvalidYAML,
    
    // Type conversion errors
    TypeConversionFailed,
    InvalidType,
    
    // Value range errors
    OutOfRange,
    InvalidValueFormat,
    
    // File system errors
    FilePermissionError,
    DiskFullError
};

class ConfigError : public std::runtime_error {
public:
    ConfigError(ErrorCode code, const std::string& message, 
                const std::string& context = "")
        : std::runtime_error(message), code_(code), context_(context) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getContext() const { return context_; }
    
private:
    ErrorCode code_;
    std::string context_;
};

// Global error handler
class ErrorHandler {
public:
    using ErrorCallback = std::function<void(const ConfigError&)>;
    
    static void registerErrorHandler(ErrorCallback callback);
    static void handleError(const ConfigError& error);
    static std::vector<ConfigError> getRecentErrors();
    static void clearErrorHistory();
    static bool hasErrors() { return !getRecentErrors().empty(); }
};

} // namespace Error

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
    Runtime,
    JSON,
    YAML
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::optional<std::string> dependsOn;
    
    ValidationRule() = default;
    ValidationRule(const std::string& k, std::function<bool(const ConfigValue&)> v, 
                   const std::string& errorMsg, const std::optional<std::string>& depends = std::nullopt)
        : key(k), validator(v), errorMessage(errorMsg), dependsOn(depends) {}
};

// Configuration metadata for schema and documentation
struct ConfigMetadata {
    std::string key;
    std::string type;
    std::string description;
    std::string defaultValue;
    std::vector<std::string> validValues;
    bool required;
    std::string category;
    std::map<std::string, std::string> constraints;
    
    ConfigMetadata() = default;
    ConfigMetadata(const std::string& k, const std::string& t, const std::string& desc,
                   const std::string& defaultVal = "", bool req = false,
                   const std::string& cat = "general")
        : key(k), type(t), description(desc), defaultValue(defaultVal), 
          required(req), category(cat) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::optional<ValidationRule> validationRule;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const ValidationRule& rule, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRule(rule) {}
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading from file formats
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromURL(const std::string& url); // Future enhancement
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file formats
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Enhanced value getting with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setting with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Validation and error handling
    bool validate() const;
    bool validate(const std::string& key) const;
    void addValidationRule(const ValidationRule& rule);
    void addValidationRule(const std::string& key, 
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage,
                          const std::optional<std::string>& dependsOn = std::nullopt);
    
    // Configuration schema and documentation
    void addMetadata(const ConfigMetadata& metadata);
    std::vector<ConfigMetadata> getSchema() const;
    std::string getFieldDescription(const std::string& key) const;
    std::string getFieldType(const std::string& key) const;
    std::vector<std::string> getValidValues(const std::string& key) const;
    
    // Advanced configuration management
    bool mergeConfig(const Config& other);
    bool overwriteConfig(const Config& other);
    bool removeConfig(const std::string& key);
    
    // Configuration statistics and analysis
    size_t getConfigSize() const;
    std::map<std::string, ConfigSource> getSources() const;
    std::string getConfigSummary() const;
    std::string getValidationSummary() const;
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const std::string& type) const;
    
    // Remove key and validation rule
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    void clearValidationRules();
    
    // Configuration snapshots and rollback
    bool saveConfigSnapshot(const std::string& name);
    bool loadConfigSnapshot(const std::string& name);
    bool removeConfigSnapshot(const std::string& name);
    std::vector<std::string> getAvailableSnapshots() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBoolean(const std::string& str);
    static std::string parseConfigValue(const std::string& str, const std::string& key);
    static std::string getConfigFileExtension(const std::string& filepath);
    
    // Validation helpers
    static bool validateRange(const ConfigValue& value, const std::map<std::string, std::string>& constraints);
    static bool validateEnum(const ConfigValue& value, const std::vector<std::string>& validValues);
    static bool validateType(const ConfigValue& value, const std::string& expectedType);
};

} // namespace nlm

// Convenience functions for common validation rules
namespace nlm::ValidationHelpers {
    bool isPositive(const ConfigValue& value);
    bool isNonNegative(const ConfigValue& value);
    bool isBetween(const ConfigValue& value, double min, double max);
    bool isInRange(const ConfigValue& value, double min, double max);
    bool isValidEnum(const ConfigValue& value, const std::vector<std::string>& validValues);
    bool isNotEmpty(const ConfigValue& value);
    bool isValidIPAddress(const ConfigValue& value);
    bool isValidPath(const ConfigValue& value);
}