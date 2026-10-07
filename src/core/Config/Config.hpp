#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration data types for better type safety
enum class ConfigDataType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector,
    BoolVector,
    Unknown
};

// Enhanced configuration validation
struct ConfigValidation {
    bool required = false;
    std::string description;
    std::any defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigValidation() = default;
    ConfigValidation(bool req, const std::string& desc) : required(req), description(desc) {}
};

// Configuration categories for organization
enum class ConfigCategory {
    General,
    Neural,
    Memory,
    Plasticity,
    Development,
    Neuromodulation,
    Perception,
    Motor,
    Simulation,
    Experimental
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    ConfigValidation validation;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), category(ConfigCategory::General), validation() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", ConfigCategory cat = ConfigCategory::General)
        : key(k), value(v), source(s), description(desc), category(cat), validation() {}
    
    // Type checking
    bool isType(ConfigDataType expectedType) const;
    template<typename T>
    bool isType() const;
    
    // Get value with type checking
    template<typename T>
    T getValue() const;
    
    // Get value safely with default
    template<typename T>
    T getValueOr(const T& defaultValue) const;
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
    
    // Enhanced command line argument parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format support
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values with enhanced type safety
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setter with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    void setDefault(const std::string& key, const ConfigValue& value, 
                    const std::string& description = "", ConfigCategory category = ConfigCategory::General);
    bool hasDefault(const std::string& key) const;
    
    // Get all keys organized by category
    std::map<ConfigCategory, std::vector<std::string>> getKeysByCategory() const;
    std::vector<std::string> getKeys(ConfigCategory category) const;
    std::vector<std::string> getKeys() const;
    
    // Enhanced configuration summary with statistics
    std::string summary() const;
    std::string summaryByCategory() const;
    
    // Configuration validation and error handling
    bool validate() const;
    std::vector<std::string> validate(const std::string& key) const;
    
    // Configuration merging and inheritance
    void merge(const Config& other);
    void mergeDefaults();
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    size_t getEntryCount(ConfigCategory category) const;
    
    // Export/import configuration
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json);
    
    // Get configuration schema information
    std::vector<std::pair<std::string, ConfigValidation>> getSchema() const;
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Convenience functions for common configuration operations
    void enableDebugging(bool enable);
    bool isDebuggingEnabled() const;
    void setSimulationTimestep(double timestep);
    double getSimulationTimestep() const;
    void setNeuronCount(size_t count);
    size_t getNeuronCount() const;
    void setRegionCount(size_t count);
    size_t getRegionCount() const;
    
    // Get configuration version
    std::string getVersion() const { return "2.0"; }
    
    // Check if configuration is valid
    bool isValid() const { return validate(); }
    
    // Get configuration help/usage
    std::string getHelp() const;
    
    // Configuration backup and restore
    bool backup(const std::string& backupPath) const;
    bool restore(const std::string& backupPath);
    
    // Configuration statistics for monitoring
    struct ConfigStats {
        size_t totalEntries = 0;
        size_t fileEntries = 0;
        size_t commandLineEntries = 0;
        size_t defaultEntries = 0;
        size_t runtimeEntries = 0;
        std::map<ConfigCategory, size_t> entriesByCategory;
        
        std::string toString() const;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigDataType getDataType(const ConfigValue& value);
    static bool validateValue(const ConfigValue& value, const ConfigValidation& validation);
    static std::string dataTypeToString(ConfigDataType type);
    static ConfigDataType inferDataType(const std::string& value);
    
    // File parsing helpers
    static bool parseJSONFile(const std::string& filepath, std::map<std::string, ConfigValue>& configMap);
    static bool parseINIFile(const std::string& filepath, std::map<std::string, ConfigValue>& configMap);
    static bool parseYAMLFile(const std::string& filepath, std::map<std::string, ConfigValue>& configMap);
    
    // Command line parsing helpers
    static void parseCommandLineArgs(int argc, char** argv, std::map<std::string, ConfigValue>& configMap);
    static bool parseKeyValuePair(const std::string& arg, std::string& key, ConfigValue& value);
    
    // Type conversion helpers
    static ConfigValue convertToConfigValue(const std::string& str);
    static std::string convertFromConfigValue(const ConfigValue& value);
    
    // Configuration validation helpers
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value, ConfigDataType expectedType);
    static std::string getConfigValueError(const ConfigValue& value, ConfigDataType expectedType);
    
    // Statistics helpers
    static void updateStats(size_t& total, ConfigSource source) const;
    
    // Utility functions
    static bool fileExists(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);
    static std::string getTimestamp();
    
    // Template implementations for ConfigEntry
    template<typename T>
    bool ConfigEntry::isType() const {
        if constexpr (std::is_same_v<T, int>) {
            return isType(ConfigDataType::Int);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return isType(ConfigDataType::Int64);
        } else if constexpr (std::is_same_v<T, double>) {
            return isType(ConfigDataType::Double);
        } else if constexpr (std::is_same_v<T, bool>) {
            return isType(ConfigDataType::Bool);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return isType(ConfigDataType::String);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return isType(ConfigDataType::IntVector);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return isType(ConfigDataType::DoubleVector);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return isType(ConfigDataType::StringVector);
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return isType(ConfigDataType::BoolVector);
        } else {
            return false;
        }
    }
    
    template<typename T>
    T ConfigEntry::getValue() const {
        if (!isType<T>()) {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Cannot get value of type for key: " + key);
        }
    }
    
    template<typename T>
    T ConfigEntry::getValueOr(const T& defaultValue) const {
        if (isType<T>()) {
            return getValue<T>();
        }
        return defaultValue;
    }
    
    template<typename T>
    T Config::get(const std::string& key) const {
        auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entry == pImpl->entries.end()) {
            throw std::runtime_error("Key not found: " + key);
        }
        
        return entry->getValue<T>();
    }
    
    template<typename T>
    T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (entry == pImpl->entries.end()) {
            return defaultValue;
        }
        
        return entry->getValueOr<T>(defaultValue);
    }
};

}};

// Utility functions for configuration management
namespace config_utils {
    // Configuration file path management
    std::string getDefaultConfigPath();
    std::string getUserConfigPath();
    std::string getEnvironmentVariablePath();
    
    // Configuration validation helpers
    bool isValidInt(const std::string& str);
    bool isValidDouble(const std::string& str);
    bool isValidBool(const std::string& str);
    
    // Configuration conversion helpers
    ConfigValue stringToConfigValue(const std::string& str);
    std::string configValueToString(const ConfigValue& value);
    
    // Configuration comparison
    bool configValueEquals(const ConfigValue& a, const ConfigValue& b);
    int configValueCompare(const ConfigValue& a, const ConfigValue& b);
    
    // Configuration merging
    void mergeConfigMaps(std::map<std::string, ConfigValue>& target,
                        const std::map<std::string, ConfigValue>& source,
                        ConfigSource sourcePriority = ConfigSource::File);
    
    // Configuration statistics
    std::string configValueStats(const ConfigValue& value);
    
    // Environment variable parsing
    void parseEnvironmentVariables(const std::string& prefix, std::map<std::string, ConfigValue>& configMap);
}};
