#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <any>

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
    std::vector<std::string>
>;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    YAML,
    JSON,
    CommandLine,
    Runtime
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& key, const std::string& message)
        : std::runtime_error("Config validation error for key \"" + key + "\": " + message) {}
};

// Parameter type for validation and schema
struct ParameterDefinition {
    std::string name;
    ConfigValue defaultValue;
    std::string description;
    std::string unit;
    std::function<bool(const ConfigValue&)> validator;
    std::optional<std::pair<double, double>> range;
    bool required = false;
    std::string category;
    std::string example;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string unit;
    std::string category;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), unit(), category() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), unit(), category() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc, const std::string& unit, const std::string& category)
        : key(k), value(v), source(s), description(desc), unit(unit), category(category) {}
};

// Schema for configuration validation
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    void addParameter(const std::string& name, const ConfigValue& defaultValue,
                     const std::string& description = "",
                     const std::string& unit = "",
                     std::function<bool(const ConfigValue&)> validator = nullptr,
                     const std::optional<std::pair<double, double>>& range = std::nullopt,
                     bool required = false,
                     const std::string& category = "",
                     const std::string& example = "") {
        
        ParameterDefinition param;
        param.name = name;
        param.defaultValue = defaultValue;
        param.description = description;
        param.unit = unit;
        param.validator = validator;
        param.range = range;
        param.required = required;
        param.category = category;
        param.example = example;
        
        parameters_[name] = param;
    }
    
    const ParameterDefinition* getParameter(const std::string& name) const {
        auto it = parameters_.find(name);
        return (it != parameters_.end()) ? &it->second : nullptr;
    }
    
    std::vector<std::string> getParameterNames() const {
        std::vector<std::string> names;
        for (const auto& pair : parameters_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::string> getCategories() const {
        std::vector<std::string> categories;
        for (const auto& pair : parameters_) {
            if (std::find(categories.begin(), categories.end(), pair.second.category) == categories.end()) {
                categories.push_back(pair.second.category);
            }
        }
        return categories;
    }
    
    void clear() {
        parameters_.clear();
    }
    
    bool hasParameter(const std::string& name) const {
        return parameters_.find(name) != parameters_.end();
    }
    
private:
    std::map<std::string, ParameterDefinition> parameters_;
    friend class Config;
};

// Main configuration class
class Config {
public:
    Config();
    explicit Config(const std::string& filename);
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from JSON file
    bool loadFromJSONFile(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    
    // Save to JSON string
    std::string saveToJSON(bool pretty = true) const;
    
    // Save to YAML string
    std::string saveToYAML(bool pretty = true) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Helper functions for common parameter types
    std::vector<int> getIntList(const std::string& key) const;
    std::vector<double> getDoubleList(const std::string& key) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation errors
    std::string getValidationErrors() const;
    
    // Set schema
    void setSchema(const ConfigSchema& schema);
    
    // Validate entire configuration
    bool validate() const;
    
    // Merge with another config
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Get source of a key
    ConfigSource getSource(const std::string& key) const;
    
    // Get description of a key
    std::string getDescription(const std::string& key) const;
    
    // Get unit of a key
    std::string getUnit(const std::string& key) const;
    
    // Get category of a key
    std::string getCategory(const std::string& key) const;
    
    // Auto-load from filename if constructor with filename is used
    bool autoLoad();
    
    // Force reload from file
    bool reload();
    
    // Check if config was loaded from file
    bool wasFileLoaded() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string filename_;
    bool autoLoad_ = true;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    static std::string serializeToString(const ConfigValue& value);
    static bool tryConvertStringToValue(const std::string& str, ConfigValue& value);
    
    // Validation helpers
    static bool validateValue(const ConfigValue& value, const ParameterDefinition& param);
    static std::string validationErrorMessage(const std::string& key, const std::string& paramName, const std::string& reason);
};