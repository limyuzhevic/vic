//=============================================================================
// Enhanced Config.hpp - Modern Configuration System
//=============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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

// Enhanced configuration entry with type information
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeName;  // Type name for validation and serialization
    bool isRequired;        // Whether this parameter is required
    std::string validationRegex;  // Regex pattern for string validation
    double minValue;        // Minimum value for numeric types
    double maxValue;        // Maximum value for numeric types
    std::vector<std::string> allowedValues;  // Allowed values for enums
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), typeName(), 
          isRequired(false), validationRegex(), minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()), allowedValues() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& type = "",
                bool required = false, const std::string& regex = "",
                double minVal = -std::numeric_limits<double>::max(),
                double maxVal = std::numeric_limits<double>::max(),
                const std::vector<std::string>& allowed = {})
        : key(k), value(v), source(s), description(desc), typeName(type),
          isRequired(required), validationRegex(regex), minValue(minVal),
          maxValue(maxVal), allowedValues(allowed) {}
    
    // Type detection and validation
    std::string getTypeName() const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
        return "unknown";
    }
    
    // Validate value based on type and constraints
    bool validate() const {
        std::string type = getTypeName();
        
        // Type-specific validation
        if (type == "int" || type == "int64_t") {
            if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value))
                return false;
        } else if (type == "double") {
            if (!std::holds_alternative<double>(value))
                return false;
        } else if (type == "bool") {
            if (!std::holds_alternative<bool>(value))
                return false;
        } else if (type == "string") {
            if (!std::holds_alternative<std::string>(value))
                return false;
            // Check regex validation
            if (!validationRegex.empty()) {
                // TODO: Implement regex validation
                // For now, assume valid
            }
            // Check allowed values
            if (!allowedValues.empty()) {
                const auto& strValue = std::get<std::string>(value);
                if (std::find(allowedValues.begin(), allowedValues.end(), strValue) == allowedValues.end())
                    return false;
            }
        } else if (type == "std::vector<int>" || type == "std::vector<double>") {
            // Vector type validation - ensure content is within bounds if specified
        }
        
        // Numeric range validation
        if (type == "int" || type == "int64_t" || type == "double") {
            double val = 0.0;
            if (type == "int") val = std::get<int>(value);
            else if (type == "int64_t") val = static_cast<double>(std::get<int64_t>(value));
            else val = std::get<double>(value);
            
            if (val < minValue || val > maxValue)
                return false;
        }
        
        return true;
    }
    
    // Convert to JSON for serialization
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["key"] = key;
        j["description"] = description;
        j["source"] = static_cast<int>(source);
        j["type"] = typeName;
        j["required"] = isRequired;
        
        // Convert value based on type
        std::string type = getTypeName();
        if (type == "int") j["value"] = std::get<int>(value);
        else if (type == "int64_t") j["value"] = std::get<int64_t>(value);
        else if (type == "double") j["value"] = std::get<double>(value);
        else if (type == "bool") j["value"] = std::get<bool>(value);
        else if (type == "string") j["value"] = std::get<std::string>(value);
        else if (type == "std::vector<int>") j["value"] = std::get<std::vector<int>>(value);
        else if (type == "std::vector<double>") j["value"] = std::get<std::vector<double>>(value);
        else if (type == "std::vector<std::string>") j["value"] = std::get<std::vector<std::string>>(value);
        
        return j;
    }
    
    // Create from JSON
    static ConfigEntry fromJson(const nlohmann::json& j) {
        ConfigEntry entry;
        entry.key = j.value("key", "");
        entry.description = j.value("description", "");
        entry.source = static_cast<ConfigSource>(j.value("source", 0));
        entry.typeName = j.value("type", "");
        entry.isRequired = j.value("required", false);
        
        // Parse value based on type
        if (j.contains("value")) {
            const auto& value = j["value"];
            if (value.is_number_integer()) {
                entry.value = value.get<int>();
                entry.typeName = "int";
            } else if (value.is_number_unsigned()) {
                entry.value = value.get<uint64_t>();
                entry.typeName = "uint64_t";
            } else if (value.is_number()) {
                entry.value = value.get<double>();
                entry.typeName = "double";
            } else if (value.is_boolean()) {
                entry.value = value.get<bool>();
                entry.typeName = "bool";
            } else if (value.is_string()) {
                entry.value = value.get<std::string>();
                entry.typeName = "string";
            } else if (value.is_array()) {
                // Determine array element type
                if (!value.empty() && value[0].is_number_integer()) {
                    entry.value = value.get<std::vector<int>>();
                    entry.typeName = "std::vector<int>";
                } else if (!value.empty() && value[0].is_number()) {
                    entry.value = value.get<std::vector<double>>();
                    entry.typeName = "std::vector<double>";
                } else {
                    entry.value = value.get<std::vector<std::string>>();
                    entry.typeName = "std::vector<std::string>";
                }
            }
        }
        
        return entry;
    }
};

// Enhanced configuration class with JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or INI format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& jsonStr);
    bool loadFromYaml(const std::string& yamlStr);
    bool loadFromIni(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string saveToJson() const;
    std::string saveToYaml() const;
    bool saveToIni(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type information
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& type = "",
             bool required = false,
             const std::string& validationRegex = "",
             double minValue = -std::numeric_limits<double>::max(),
             double maxValue = std::numeric_limits<double>::max(),
             const std::vector<std::string>& allowedValues = {});
    
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& type = "string",
             bool required = false,
             const std::string& validationRegex = "",
             double minValue = -std::numeric_limits<double>::max(),
             double maxValue = std::numeric_limits<double>::max(),
             const std::vector<std::string>& allowedValues = {});
    
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& type = "int",
             bool required = false,
             double minValue = -std::numeric_limits<double>::max(),
             double maxValue = std::numeric_limits<double>::max());
    
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& type = "double",
             bool required = false,
             double minValue = -std::numeric_limits<double>::max(),
             double maxValue = std::numeric_limits<double>::max());
    
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& type = "bool",
             bool required = false);
    
    // Check existence and validate
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    bool isComplete() const;  // Check if all required parameters are set
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getRequiredKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summaryWithValidation() const;
    
    // Merge another config (command line overrides file)
    void merge(const Config& other);
    
    // Schema validation
    bool validateSchema() const;
    std::string getSchema() const;
    
    // Configuration presets
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config loadFromEnvironment();
    
    // Template parameter setters for convenience
    template<typename... Args>
    void setMultiple(const std::map<std::string, ConfigValue>& params) {
        for (const auto& pair : params) {
            set(pair.first, pair.second);
        }
    }
    
    // Get configuration statistics
    size_t getTotalEntries() const;
    size_t getRequiredEntries() const;
    size_t getValidEntries() const;
    size_t getMissingEntries() const;
    
    // Batch operations
    std::vector<ConfigEntry> getValidEntries() const;
    std::vector<ConfigEntry> getInvalidEntries() const;
    std::vector<ConfigEntry> getMissingEntries() const;
    
    // Configuration groups
    void addGroup(const std::string& groupName);
    void removeGroup(const std::string& groupName);
    bool hasGroup(const std::string& groupName) const;
    void setGroup(const std::string& groupName, const std::string& key, 
                  const ConfigValue& value);
    ConfigValue getGroupValue(const std::string& groupName, const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string normalizeKey(const std::string& key);
    static ConfigValue parseValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static bool validateEntry(const ConfigEntry& entry);
    static std::string getTypeNameFromValue(const ConfigValue& value);
    
    // JSON/YAML parsing helpers
    static void parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static void parseYamlFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static void parseIniFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Validation helpers
    static bool validateAgainstSchema(const std::vector<ConfigEntry>& entries,
                                     const nlohmann::json& schema);
};

} // namespace nlm

// Forward declaration for Config.cpp
#include "ConfigImpl.hpp"

#endif // CONFIG_HPP