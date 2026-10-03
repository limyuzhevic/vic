#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    Runtime,
    SchemaDefault
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          validationRegex(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& regex = "", 
                bool req = false)
        : key(k), value(v), source(s), description(desc),
          validationRegex(regex), required(req) {}
    
    bool isValid() const {
        if (required && value.index() == 0) return false; // Empty string is "default"
        if (!validationRegex.empty()) {
            // Simple regex validation - for now just check if pattern matches
            // Could be enhanced with proper regex engine
            if (std::string* str = std::get_if<std::string>(&value)) {
                if (!str->empty()) {
                    // Very basic regex check - this is a placeholder
                    // In production, use a proper regex library like std::regex or re2
                    return true;
                }
            }
        }
        return true;
    }
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "vec_int", "vec_double", "vec_string"
    ConfigSource source;
    ConfigValue defaultValue;
    std::string description;
    std::string validationRegex;
    bool required;
    
    ConfigSchema() : source(ConfigSource::Default), required(false) {}
    
    ConfigSchema(const std::string& k, const std::string& t, ConfigSource s,
                 const ConfigValue& def = "", const std::string& desc = "",
                 const std::string& regex = "", bool req = false)
        : key(k), type(t), source(s), defaultValue(def),
          description(desc), validationRegex(regex), required(req) {}
    
    bool isValid(const ConfigValue& value) const {
        // Validate based on type
        if (type == "int" && std::holds_alternative<int>(value)) return true;
        if (type == "int64_t" && std::holds_alternative<int64_t>(value)) return true;
        if (type == "double" && std::holds_alternative<double>(value)) return true;
        if (type == "bool" && std::holds_alternative<bool>(value)) return true;
        if (type == "string" && std::holds_alternative<std::string>(value)) return true;
        if (type == "vec_int" && std::holds_alternative<std::vector<int>>(value)) return true;
        if (type == "vec_double" && std::holds_alternative<std::vector<double>>(value)) return true;
        if (type == "vec_string" && std::holds_alternative<std::vector<std::string>>(value)) return true;
        
        // If it's empty string, check if required or has default
        if (type == "string" && std::holds_alternative<std::string>(value) && 
            std::get<std::string>(value).empty()) {
            return !required || (defaultValue.index() != 0);
        }
        
        return false;
    }
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// Main configuration class with enhanced features
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
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
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
    
    // Set values from JSON
    void setFromJSON(const std::string& key, const json& j, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    bool validateSchema() const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Configuration merging
    void merge(const Config& other, bool override = true);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Runtime configuration updates
    void updateFromJSON(const std::string& jsonString);
    void reloadFromFile(const std::string& filepath);
    
    // Configuration debugging
    std::string getConfigErrors() const;
    
    // Get raw entries for inspection
    std::vector<ConfigEntry> getEntries() const;
    
    // Get source-specific entries
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getTypeName(const ConfigValue& value);
    static json configValueToJSON(const ConfigValue& value);
    static bool jsonToConfigValue(const json& j, ConfigValue& value);
};

} // namespace nlm
