#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <regex>
#include <functional>
#include <set>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

using json = nlohmann::json;

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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    std::string type;
    std::string validationRegex;
    std::string examples;
    std::string minValue;
    std::string maxValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   category("general"), type("string"), validationRegex(), 
                   examples(), minValue(), maxValue() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general"), 
          type("string"), validationRegex(), examples(), minValue(), maxValue() {}
};

// Configuration validation error
enum class ConfigErrorType {
    None,
    MissingRequired,
    InvalidType,
    InvalidValue,
    ValidationError,
    NotFound,
    CircularInheritance,
    InvalidFormat,
    SchemaError
};

class ConfigError {
public:
    ConfigError() : type(ConfigErrorType::None), message(""), key("") {}
    ConfigError(ConfigErrorType t, const std::string& msg, const std::string& k = "")
        : type(t), message(msg), key(k) {}
    
    ConfigErrorType type;
    std::string message;
    std::string key;
    std::vector<std::string> path;
};

class ConfigValidationResult {
public:
    bool isValid() const { return errors.empty(); }
    const std::vector<ConfigError>& getErrors() const { return errors; }
    void addError(const ConfigError& error) { errors.push_back(error); }
    
private:
    std::vector<ConfigError> errors;
};

// Configuration schema parameter
struct ConfigParameter {
    std::string name;
    std::string type;
    std::string description;
    std::string category;
    bool required;
    ConfigValue defaultValue;
    std::string examples;
    std::string validationRegex;
    std::string minValue;
    std::string maxValue;
    std::vector<std::string> allowedValues;
    
    ConfigParameter() : required(false) {}
};

// Configuration schema
class ConfigSchema {
public:
    void addParameter(const ConfigParameter& param);
    const ConfigParameter* getParameter(const std::string& name) const;
    const std::vector<ConfigParameter>& getParameters() const { return parameters; }
    const std::vector<std::string>& getCategories() const { return categories; }
    
    bool validate(const std::string& key, const ConfigValue& value) const;
    ConfigValidationResult validateAll(const std::unordered_map<std::string, ConfigValue>& config) const;
    
    static ConfigSchema fromJSON(const json& j);
    json toJSON() const;
    
private:
    std::vector<ConfigParameter> parameters;
    std::vector<std::string> categories;
    std::unordered_map<std::string, ConfigParameter> paramMap;
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load with inheritance
    bool loadWithInheritance(const std::vector<std::string>& filepaths);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration
    ConfigValidationResult validate() const;
    bool validateParameter(const std::string& key, const ConfigValue& value) const;
    
    // Schema operations
    void setSchema(const ConfigSchema& schema) { currentSchema = schema; }
    const ConfigSchema& getSchema() const { return currentSchema; }
    
    // Environment variable substitution
    static std::string substituteEnvVars(const std::string& input);
    
    // Configuration comparison
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration diff
    static std::string diff(const Config& config1, const Config& config2);
    
    // Configuration history
    void addToHistory(const std::string& description);
    std::vector<std::string> getHistory() const;
    
    // Configuration export/import
    static json exportToJSON(const Config& config);
    static Config importFromJSON(const json& j);
    
    // Configuration categories
    std::vector<std::string> getCategories() const;
    std::vector<ConfigEntry> getParametersInCategory(const std::string& category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema currentSchema;
    std::vector<std::string> history;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchRegex(const std::string& value, const std::string& pattern);
    static std::string resolveEnvVar(const std::string& input);
    static void parseJSONFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& config);
    static void parseYAMLFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& config);
};

// Configuration utilities
namespace config_utils {
    std::string toString(const ConfigValue& value);
    std::string detectConfigFormat(const std::string& filepath);
    bool isCommentLine(const std::string& line);
}

} // namespace nlm
