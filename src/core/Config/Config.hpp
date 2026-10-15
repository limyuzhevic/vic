#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <optional>
#include <vector>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using JsonValue = nlohmann::json;
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
    Environment
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

// Configuration schema validation
class ConfigSchema {
public:
    using Validator = std::function<bool(const ConfigValue&)>;
    
    struct ValidationRule {
        std::string key;
        Validator validator;
        std::string errorMessage;
    };
    
    ConfigSchema() = default;
    
    void addRequired(const std::string& key, const std::string& errorMsg = "Required") {
        requiredKeys_.push_back(key);
        errorMessages_[key] = errorMsg;
    }
    
    void addValidator(const std::string& key, Validator validator, const std::string& errorMsg) {
        validationRules_.emplace_back(key, validator, errorMsg);
    }
    
    void addRangeValidator(const std::string& key, double min, double max, const std::string& errorMsg) {
        addValidator(key, [min, max](const ConfigValue& val) {
            if (const double* d = std::get_if<double>(&val)) {
                return *d >= min && *d <= max;
            }
            if (const int* i = std::get_if<int>(&val)) {
                return static_cast<double>(*i) >= min && static_cast<double>(*i) <= max;
            }
            if (const int64_t* i64 = std::get_if<int64_t>(&val)) {
                return static_cast<double>(*i64) >= min && static_cast<double>(*i64) <= max;
            }
            return false;
        }, errorMsg);
    }
    
    void addStringLengthValidator(const std::string& key, size_t minLen, size_t maxLen, const std::string& errorMsg) {
        addValidator(key, [minLen, maxLen](const ConfigValue& val) {
            if (const std::string* s = std::get_if<std::string>(&val)) {
                return s->size() >= minLen && s->size() <= maxLen;
            }
            return false;
        }, errorMsg);
    }
    
    bool validate(const std::unordered_map<std::string, ConfigEntry>& entries) const {
        bool isValid = true;
        
        // Check required keys
        for (const auto& key : requiredKeys_) {
            if (entries.find(key) == entries.end()) {
                std::cerr << "Configuration validation error: Missing required key: " << key << " - " << errorMessages_[key] << std::endl;
                isValid = false;
            }
        }
        
        // Check validation rules
        for (const auto& rule : validationRules_) {
            auto it = entries.find(rule.key);
            if (it != entries.end()) {
                if (!rule.validator(it->second.value)) {
                    std::cerr << "Configuration validation error: Key " << rule.key << " - " << rule.errorMessage << std::endl;
                    isValid = false;
                }
            }
        }
        
        return isValid;
    }
    
private:
    std::vector<std::string> requiredKeys_;
    std::vector<ValidationRule> validationRules_;
    std::unordered_map<std::string, std::string> errorMessages_;
};

// Main configuration class
class Config {
public:
    Config() = default;
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept = default;
    Config& operator=(Config&&) noexcept = default;
    
    // Load from JSON file
    bool loadFromJson(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from YAML file (requires YAML library)
    bool loadFromYaml(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToJson(const std::string& filepath, bool pretty = true) const;
    
    // Get values with type safety
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
    
    // Convenience methods for common types
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setNumber(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    bool validateWithSchema(const ConfigSchema& schema) const;
    
    // Environment variable support
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Get the internal entries map
    const std::unordered_map<std::string, ConfigEntry>& getEntries() const { return entries_; }
    
    // Set the internal entries map
    void setEntries(const std::unordered_map<std::string, ConfigEntry>& entries) { entries_ = entries; }
    
private:
    std::unordered_map<std::string, ConfigEntry> entries_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJsonToConfig(const JsonValue& json, std::unordered_map<std::string, ConfigEntry>& entries);
    static JsonValue convertToJson(const ConfigValue& value);
    
    // JSON conversion helpers
    template<typename T>
    static void addJsonValue(JsonValue& json, const std::string& key, const T& value);
    template<typename T>
    static bool extractJsonValue(const JsonValue& json, const std::string& key, T& value);
}; 

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

#include "Config.cpp"
