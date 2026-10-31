#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation rule
using ValidationRule = std::function<bool(const ConfigValue&)>;

// Configuration type
enum class ConfigType {
    Integer,
    Float,
    Boolean,
    String,
    IntArray,
    FloatArray,
    StringArray,
    BoolArray
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigType type;
    ValidationRule validator;
    bool required;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), 
          type(ConfigType::String), validator(nullptr), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", ConfigType t = ConfigType::String,
                ValidationRule vld = nullptr, bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), 
          validator(vld), required(req) {}
};

// Schema definition
struct ConfigSchema {
    std::string key;
    ConfigType type;
    std::string description;
    ValidationRule validator;
    bool required;
    std::string defaultValue;
    
    ConfigSchema() : type(ConfigType::String), description(), validator(nullptr), 
                     required(false), defaultValue("") {}
    
    ConfigSchema(const std::string& k, ConfigType t, const std::string& desc = "",
                 ValidationRule vld = nullptr, bool req = false,
                 const std::string& def = "")
        : key(k), type(t), description(desc), validator(vld), 
          required(req), defaultValue(def) {}
};

// Schema validation exception
class SchemaValidationError : public std::runtime_error {
public:
    SchemaValidationError(const std::string& message, const std::string& key, 
                         const std::string& expected, const std::string& actual)
        : std::runtime_error(message), key_(key), expected_(expected), actual_(actual) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getExpected() const { return expected_; }
    const std::string& getActual() const { return actual_; }
    
private:
    std::string key_;
    std::string expected_;
    std::string actual_;
};

// Main enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading methods
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromString(const std::string& content, const std::string& format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Schema-based validation
    bool validateSchema() const;
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    bool validateKey(const std::string& key) const;
    
    // Schema definition
    void defineSchema(const std::string& key, ConfigType type,
                     const std::string& description = "",
                     ValidationRule validator = nullptr,
                     bool required = false,
                     const std::string& defaultValue = "");
    
    // Type-safe getters with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key, 
                                 const std::optional<T>& defaultValue = std::nullopt) const;
    
    template<typename T>
    T getOrValidated(const std::string& key, const T& defaultValue = T()) const;
    
    // Enhanced value getters with type conversion
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    
    // Set values with validation
    void setValidated(const std::string& key, const ConfigValue& value, 
                     ConfigSource source = ConfigSource::Runtime,
                     bool validate = true);
    
    // Convenience setters
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, ConfigType expectedType) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by type
    std::vector<std::string> getKeysByType(ConfigType type) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get schema for key
    ConfigType getType(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Validation helpers
    static bool validateInt(const ConfigValue& value, int min = std::numeric_limits<int>::min(), 
                           int max = std::numeric_limits<int>::max());
    static bool validateDouble(const ConfigValue& value, double min = -std::numeric_limits<double>::max(),
                              double max = std::numeric_limits<double>::max());
    static bool validateString(const ConfigValue& value, size_t minLength = 0, size_t maxLength = std::string::max_size());
    static bool validateBool(const ConfigValue& value);
    static bool validateArraySize(const ConfigValue& value, size_t minSize = 0, size_t maxSize = std::string::max_size());
    
    // Static helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigType inferTypeFromString(const std::string& str);
    static std::string configTypeToString(ConfigType type);
    
    // JSON/YAML conversion helpers
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue JSONToConfigValue(const nlohmann::json& jsonValue);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static ConfigValue YAMLToConfigValue(const YAML::Node& yamlValue);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    bool validateValue(const ConfigEntry& entry, const ConfigValue& value) const;
    std::optional<ConfigValue> convertToTargetType(const ConfigValue& sourceValue, 
                                                   ConfigType targetType) const;
};

// Template implementations

inline std::optional<int> Config::getTyped<int>(const std::string& key) const {
    auto value = get<int>(key);
    return value.has_value() ? std::optional<int>(*value) : std::nullopt;
}

inline std::optional<double> Config::getTyped<double>(const std::string& key) const {
    auto value = get<double>(key);
    return value.has_value() ? std::optional<double>(*value) : std::nullopt;
}

inline std::optional<bool> Config::getTyped<bool>(const std::string& key) const {
    auto value = get<bool>(key);
    return value.has_value() ? std::optional<bool>(*value) : std::nullopt;
}

inline std::optional<std::string> Config::getTyped<std::string>(const std::string& key) const {
    auto value = get<std::string>(key);
    return value.has_value() ? std::optional<std::string>(*value) : std::nullopt;
}

// Type validation implementations
inline bool Config::validateInt(const ConfigValue& value, int min, int max) {
    try {
        int val = std::get<int>(value);
        return val >= min && val <= max;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

inline bool Config::validateDouble(const ConfigValue& value, double min, double max) {
    try {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

inline bool Config::validateString(const ConfigValue& value, size_t minLength, size_t maxLength) {
    try {
        std::string val = std::get<std::string>(value);
        return val.length() >= minLength && val.length() <= maxLength;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

inline bool Config::validateBool(const ConfigValue& value) {
    try {
        bool val = std::get<bool>(value);
        return true; // Boolean validation is trivial
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

inline bool Config::validateArraySize(const ConfigValue& value, size_t minSize, size_t maxSize) {
    try {
        if (std::holds_alternative<std::vector<int>>(value)) {
            auto& val = std::get<std::vector<int>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            auto& val = std::get<std::vector<double>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            auto& val = std::get<std::vector<std::string>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<bool>>(value)) {
            auto& val = std::get<std::vector<bool>>(value);
            return val.size() >= minSize && val.size() <= maxSize;
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
    return false;
}

// ConfigType to string conversion
inline std::string Config::configTypeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Integer: return "integer";
        case ConfigType::Float: return "float";
        case ConfigType::Boolean: return "boolean";
        case ConfigType::String: return "string";
        case ConfigType::IntArray: return "int_array";
        case ConfigType::FloatArray: return "float_array";
        case ConfigType::StringArray: return "string_array";
        case ConfigType::BoolArray: return "bool_array";
        default: return "unknown";
    }
}

} // namespace nlm

// Include implementation header
#include "ConfigImpl.hpp"