#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <regex>
#include <fstream>
#include <sstream>

// Forward declarations
class Config;

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<float>
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
    std::string validationPattern;
    std::string validationMessage;
    bool isRequired;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), validationPattern(),
          validationMessage(), isRequired(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& pattern = "", const std::string& msg = "",
                bool required = false)
        : key(k), value(v), source(s), description(desc), validationPattern(pattern),
          validationMessage(msg), isRequired(required) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::vector<std::string> validTypes;
    std::string defaultValue;
    bool isRequired;
    std::string description;
    
    ConfigSchema() : isRequired(false) {}
    ConfigSchema(const std::string& k, const std::vector<std::string>& types,
                 const std::string& defaultVal = "", bool required = false,
                 const std::string& desc = "")
        : key(k), validTypes(types), defaultValue(defaultVal),
          isRequired(required), description(desc) {}
};

// Validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getSummary() const {
        std::stringstream ss;
        ss << "Valid: " << (isValid ? "Yes" : "No");
        if (!errors.empty()) {
            ss << ", Errors: " << errors.size();
        }
        if (!warnings.empty()) {
            ss << ", Warnings: " << warnings.size();
        }
        return ss.str();
    }
};

// JSON utilities (simplified version - in production would use nlohmann::json)
class JSONUtils {
public:
    static bool parseJSONFile(const std::string& filepath, std::unordered_map<std::string, std::string>& result);
    static std::string serializeToJSON(const std::unordered_map<std::string, std::string>& data);
    static bool validateJSON(const std::string& jsonStr);
    
    // Convert between ConfigValue and JSON string representation
    static std::string configValueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str, const std::string& typeHint);
};

// YAML utilities (simplified version - in production would use yaml-cpp)
class YAMLUtils {
public:
    static bool parseYAMLFile(const std::string& filepath, std::unordered_map<std::string, std::string>& result);
    static std::string serializeToYAML(const std::unordered_map<std::string, std::string>& data);
    static bool validateYAML(const std::string& yamlStr);
};

// Configuration validation utilities
class ConfigValidator {
public:
    static ValidationResult validate(const Config& config, const std::vector<ConfigSchema>& schema);
    static std::vector<std::string> getRequiredKeys(const std::vector<ConfigSchema>& schema);
    static bool validateValue(const ConfigValue& value, const std::string& typeName);
    static std::string getTypeName(const ConfigValue& value);
};

// Main configuration class with enhanced functionality
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or simple key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON or YAML)
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format by default)
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<float>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced setter with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
                      const std::string& pattern = "", const std::string& message = "");
    
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
    
    // Validation methods
    ValidationResult validate(const std::vector<ConfigSchema>& schema) const;
    bool validateValue(const std::string& key, const std::string& typeName) const;
    
    // Configuration schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    std::vector<ConfigSchema> getSchemas() const;
    void clearSchemas();
    
    // Import/Export utilities
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    bool importFromJSON(const std::string& jsonStr);
    bool importFromYAML(const std::string& yamlStr);
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    void override(const Config& other);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    float getSourceRatio(ConfigSource source) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Configuration history (for debugging)
    std::vector<std::pair<std::string, ConfigSource>> getChangeHistory() const;
    void enableChangeTracking(bool enable);
    bool isChangeTrackingEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string detectFileFormat(const std::string& filepath);
    static bool parseSimpleKeyValue(const std::string& line, std::string& key, std::string& value);
    static std::string escapeJSONString(const std::string& str);
    static std::string unescapeJSONString(const std::string& str);
    
    // Configuration entry management
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source,
                  const std::string& description = "", const std::string& pattern = "");
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
    
    // Schema validation
    bool validateAgainstSchema(const ConfigSchema& schema) const;
    bool validateEntryAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema) const;
};

} // namespace nlm