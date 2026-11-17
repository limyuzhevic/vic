#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <set>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types (same as before for backward compatibility)
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
    EnvironmentVariable
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::string validationPattern;
    std::string validationErrorMessage;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), required(false), 
          validationPattern(), validationErrorMessage() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false,
                const std::string& pattern = "", 
                const std::string& errorMsg = "")
        : key(k), value(v), source(s), description(desc), required(req),
          validationPattern(pattern), validationErrorMessage(errorMsg) {}
};

// Type for JSON schema definition
using JsonSchema = nlohmann::json;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs, 
                     const std::vector<std::string>& warns = std::vector<std::string>())
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrorString() const {
        if (errors.empty()) return "";
        std::string result;
        for (size_t i = 0; i < errors.size(); ++i) {
            if (i > 0) result += "; ";
            result += errors[i];
        }
        return result;
    }
};

// Main configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with schema validation)
    bool loadFromFile(const std::string& filepath, const JsonSchema& schema = JsonSchema());
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    void set(const std::string& key, int64_t value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false,
             const std::string& validationPattern = "",
             const std::string& validationErrorMessage = "");
    
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
    
    // Schema validation
    ValidationResult validate(const JsonSchema& schema) const;
    
    // Environment variable support
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Configuration defaults management
    void setDefault(const std::string& key, const ConfigValue& value,
                    const std::string& description = "",
                    bool required = false);
    void setDefaultSchema(const JsonSchema& schema);
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getDefaultCount() const;
    size_t getFileCount() const;
    
    // Type conversion utilities
    static std::string toString(const ConfigValue& value);
    static std::optional<ConfigValue> fromString(const std::string& str);
    
    // Validation helpers
    static bool validatePattern(const std::string& value, const std::string& pattern);
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateChoices(const ConfigValue& value, const std::vector<std::string>& choices);
    
    // Configuration presets
    static JsonSchema getDefaultSchema();
    static JsonSchema getPhase2Schema();
    static JsonSchema getPhase6Schema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static nlohmann::json convertConfigValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> convertJsonToConfigValue(const nlohmann::json& jsonValue);
    static std::string generateKeyPath(const std::vector<std::string>& keys) const;
    
    // Schema validation helpers
    static void validateEntryAgainstSchema(const ConfigEntry& entry, const JsonSchema& schema, 
                                           ValidationResult& result, const std::string& keyPath = "");
    static std::optional<JsonSchema> findSchemaForKey(const JsonSchema& schema, const std::string& key);
    
    // JSON serialization helpers
    static nlohmann::json convertAllToJson() const;
};

// Type-specific get methods for common types
std::optional<int> getInt(const Config& config, const std::string& key);
std::optional<double> getDouble(const Config& config, const std::string& key);
std::optional<bool> getBool(const Config& config, const std::string& key);
std::optional<std::string> getString(const Config& config, const std::string& key);

// Type-specific set methods for common types
void setInt(Config& config, const std::string& key, int value, 
            ConfigSource source = ConfigSource::Runtime,
            const std::string& description = "",
            bool required = false);
void setDouble(Config& config, const std::string& key, double value,
               ConfigSource source = ConfigSource::Runtime,
               const std::string& description = "",
               bool required = false);
void setBool(Config& config, const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             bool required = false);
void setString(Config& config, const std::string& key, const std::string& value,
               ConfigSource source = ConfigSource::Runtime,
               const std::string& description = "",
               bool required = false);

} // namespace nlm