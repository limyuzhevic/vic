#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Configuration schema definition for validation
struct ConfigSchema {
    std::string key;
    std::string description;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    bool required;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    
    ConfigSchema(const std::string& k, const std::string& desc, 
                 const std::variant<int, int64_t, double, bool, std::string>& def,
                 bool req = false)
        : key(k), description(desc), defaultValue(def), required(req), 
          validationRegex(""), allowedValues() {}
};

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::vector<double>>,  // For matrix parameters
    std::map<std::string, double>     // For named parameters
>;

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeName;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    bool readOnly;
    std::string lastModified;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), typeName(), 
          validationRegex(), allowedValues(), readOnly(false), lastModified() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), typeName(getTypeName(v)), 
          validationRegex(), allowedValues(), readOnly(false), lastModified() {}
    
    static std::string getTypeName(const ConfigValue& value) {
        std::string type;
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) type = "int";
            else if constexpr (std::is_same_v<T, int64_t>) type = "int64_t";
            else if constexpr (std::is_same_v<T, double>) type = "double";
            else if constexpr (std::is_same_v<T, bool>) type = "bool";
            else if constexpr (std::is_same_v<T, std::string>) type = "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) type = "vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) type = "vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) type = "vector<string>";
            else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) type = "matrix<double>";
            else if constexpr (std::is_same_v<T, std::map<std::string, double>>) type = "map<string,double>";
            else type = "unknown";
        }, value);
        return type;
    }
    
    // Validate the entry against its schema
    bool validate() const {
        // Check read-only constraint
        if (readOnly && source != ConfigSource::Default) {
            return false;
        }
        
        // Validate against allowed values if specified
        if (!allowedValues.empty()) {
            std::string strValue;
            std::visit([&](const auto& val) {
                if constexpr (std::is_same_v<decltype(val), std::string>) {
                    strValue = val;
                }
            }, value);
            if (!strValue.empty() && 
                std::find(allowedValues.begin(), allowedValues.end(), strValue) == allowedValues.end()) {
                return false;
            }
        }
        
        // Validate regex if specified
        if (!validationRegex.empty()) {
            // Simple regex validation - in production, use proper regex library
            std::string strValue;
            std::visit([&](const auto& val) {
                if constexpr (std::is_same_v<decltype(val), std::string>) {
                    strValue = val;
                }
            }, value);
            if (!strValue.empty()) {
                // Basic pattern validation (e.g., check for required format)
                if (validationRegex == "^[a-zA-Z0-9_]+$") {
                    if (!std::all_of(strValue.begin(), strValue.end(), 
                                  [](char c) { return std::isalnum(c) || c == '_'; })) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
};

// Enhanced configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Initialize with schema
    bool initializeWithSchema(const std::vector<ConfigSchema>& schema);
    
    // Load from JSON file (preferred format)
    bool loadFromFile(const std::string& filepath) override;
    
    // Load from TOML file
    bool loadFromToml(const std::string& filepath);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv) override;
    
    // Save to file in JSON format
    bool saveToFile(const std::string& filepath) const override;
    
    // Get values with enhanced type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const override;
    
    // Get value with type conversion and validation
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const override;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::map<std::string, double>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const override;
    
    // Remove key
    void remove(const std::string& key) override;
    
    // Get all keys
    std::vector<std::string> getKeys() const override;
    
    // Get keys by type
    std::vector<std::string> getKeysOfType(const std::string& typeName) const;
    
    // Get all configuration entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear() override;
    
    // Get configuration summary with more details
    std::string summary() const override;
    
    // Validate entire configuration
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get default value for key
    std::optional<ConfigValue> getDefaultValue(const std::string& key) const;
    
    // Check if key is read-only
    bool isReadOnly(const std::string& key) const;
    
    // Set read-only status
    void setReadOnly(const std::string& key, bool readOnly);
    
    // Merge with another configuration
    bool merge(const Config& other);
    
    // Validate a value against type constraints
    static bool validateValue(const std::string& key, const ConfigValue& value, 
                           const std::string& expectedType);
    
    // Convert string to appropriate type
    template<typename T>
    static T stringToValue(const std::string& str);
    
    // Convert value to string
    static std::string valueToString(const ConfigValue& value);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
        std::map<std::string, size_t> entriesByType;
        double memoryUsage;
        std::string oldestEntry;
        std::string newestEntry;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema for validation
    std::vector<ConfigSchema> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static double stringToDouble(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
    
    // Validate against schema
    bool validateAgainstSchema() const;
    
    // Update last modified timestamp
    void updateLastModified(std::string& lastModified);
    
    // Find entry by key
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
};

// Explicit template instantiations for get and getOr
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::map<std::string, double> Config::getOr<std::map<std::string, double>>(const std::string&, const std::map<std::string, double>&) const;

} // namespace nlm
