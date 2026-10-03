#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

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
    SchemaValidation
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error(message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

// Configuration type constraints
enum class ConfigType {
    Integer,
    Float,
    Boolean,
    String,
    IntVector,
    FloatVector,
    StringVector,
    Any
};

// Configuration constraint
struct ConfigConstraint {
    std::string key;
    ConfigType type;
    std::optional<std::string> description;
    std::optional<std::string> defaultValue;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigConstraint(const std::string& k, ConfigType t, const std::string& desc = "")
        : key(k), type(t), description(desc) {}
    
    bool validate(const ConfigValue& value) const {
        if (validator) {
            return validator(value);
        }
        
        // Type checking
        switch (type) {
            case ConfigType::Integer:
                return std::holds_alternative<int>(value);
            case ConfigType::Float:
                return std::holds_alternative<double>(value);
            case ConfigType::Boolean:
                return std::holds_alternative<bool>(value);
            case ConfigType::String:
                return std::holds_alternative<std::string>(value);
            case ConfigType::IntVector:
                return std::holds_alternative<std::vector<int>>(value);
            case ConfigType::FloatVector:
                return std::holds_alternative<std::vector<double>>(value);
            case ConfigType::StringVector:
                return std::holds_alternative<std::vector<std::string>>(value);
            case ConfigType::Any:
                return true;
        }
        return false;
    }
    
    bool checkRange(const ConfigValue& value) const {
        if (!minValue.has_value() && !maxValue.has_value()) return true;
        
        try {
            double val = 0.0;
            if (std::holds_alternative<int>(value)) {
                val = static_cast<double>(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                val = std::get<double>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                val = static_cast<double>(std::get<int64_t>(value));
            }
            
            if (minValue.has_value() && val < minValue.value()) return false;
            if (maxValue.has_value() && val > maxValue.value()) return false;
        } catch (...) {
            return false;
        }
        return true;
    }
    
    bool checkAllowedValues(const ConfigValue& value) const {
        if (allowedValues.empty()) return true;
        
        std::string strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            strValue = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            strValue = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            strValue = std::get<bool>(value) ? "true" : "false";
        } else {
            return false;
        }
        
        return std::find(allowedValues.begin(), allowedValues.end(), strValue) != allowedValues.end();
    }
};

// Enhanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filepath; // Track which file loaded this entry
    int lineNumber;       // Track line number in config file
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), filepath(), lineNumber(-1) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& filepath = "", int lineNumber = -1)
        : key(k), value(v), source(s), description(desc), filepath(filepath), lineNumber(lineNumber) {}
};

// Configuration file format detector
enum class ConfigFileFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML,
    TOML
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
    
    // Load from file with automatic format detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from TOML file
    bool loadFromTOML(const std::string& filepath);
    
    // Load from simple key=value file
    bool loadFromSimpleFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, ConfigFileFormat format = ConfigFileFormat::SimpleKeyValue) const;
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Save as TOML
    bool saveToTOML(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Configuration schema management
    void addSchema(const ConfigConstraint& constraint);
    bool validateSchema() const;
    std::vector<ConfigValidationError> getSchemaValidationErrors() const;
    
    // Configuration introspection
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    std::string getStatistics() const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Configuration file format detection
    static ConfigFileFormat detectFileFormat(const std::string& filepath);
    
    // Get all keys by source
    std::map<ConfigSource, std::vector<std::string>> getKeysBySource() const;
    
    // Export configuration to various formats
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    std::string exportToTOML() const;
    std::string exportToINI() const;
    
    // Import configuration from various formats
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    bool importFromTOML(const std::string& toml);
    bool importFromINI(const std::string& ini);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigFileFormat detectFormatFromContent(const std::string& content);
    static bool parseSimpleLine(const std::string& line, std::string& key, std::string& value, int& lineNumber);
    
    // Schema validation helpers
    void validateEntry(const ConfigEntry& entry) const;
    bool validateConstraint(const ConfigConstraint& constraint, const ConfigValue& value) const;
    
    // File format helpers
    static bool parseJSONValue(const std::string& str, ConfigValue& value);
    static bool parseYAMLValue(const std::string& str, ConfigValue& value);
    static bool parseTOMLValue(const std::string& str, ConfigValue& value);
    
    // Export helpers
    static std::string configValueToString(const ConfigValue& value);
    static ConfigValue stringToConfigValue(const std::string& str, ConfigType type);
};

} // namespace nlm
