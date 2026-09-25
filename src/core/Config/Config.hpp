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
#include <chrono>
#include <nlohmann/json.hpp>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration type enum
enum class ConfigType {
    Integer,
    Int64,
    Double,
    Boolean,
    String,
    IntArray,
    DoubleArray,
    StringArray
};

// Validation rule function type
using ValidationRule = std::function<bool(const ConfigValue&)>;

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigType type;
    std::optional<ConfigValue> defaultValue;
    std::string description;
    ValidationRule validationRule;
    std::vector<std::string> dependencies;  // Keys that must exist for this to be valid
    bool required;
    std::string validationErrorMessage;
    
    ConfigSchemaEntry() : type(ConfigType::String), required(false) {}
    ConfigSchemaEntry(const std::string& k, ConfigType t, 
                     const std::optional<ConfigValue>& def = std::nullopt,
                     const std::string& desc = "",
                     const ValidationRule& validator = nullptr,
                     const std::vector<std::string>& deps = std::vector<std::string>(),
                     bool req = false,
                     const std::string& errMsg = "")
        : key(k), type(t), defaultValue(def), description(desc), 
          validationRule(validator), dependencies(deps), required(req), validationErrorMessage(errMsg) {}
};

// Configuration schema
class ConfigSchema {
public:
    std::string version;
    std::vector<ConfigSchemaEntry> entries;
    std::unordered_map<std::string, ConfigSchemaEntry*> keyToEntry;
    
    ConfigSchema(const std::string& ver = "1.0") : version(ver) {}
    
    void addEntry(const ConfigSchemaEntry& entry) {
        entries.push_back(entry);
        keyToEntry[entry.key] = &entries.back();
    }
    
    const ConfigSchemaEntry* getEntry(const std::string& key) const {
        auto it = keyToEntry.find(key);
        return it != keyToEntry.end() ? it->second : nullptr;
    }
    
    bool hasEntry(const std::string& key) const {
        return keyToEntry.find(key) != keyToEntry.end();
    }
    
    std::vector<std::string> getRequiredKeys() const {
        std::vector<std::string> required;
        for (const auto& entry : entries) {
            if (entry.required) {
                required.push_back(entry.key);
            }
        }
        return required;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Schema (v" << version << ") with " << entries.size() << " entries:\n";
        for (const auto& entry : entries) {
            oss << "  " << entry.key << ": ";
            switch (entry.type) {
                case ConfigType::Integer: oss << "int"; break;
                case ConfigType::Int64: oss << "int64"; break;
                case ConfigType::Double: oss << "double"; break;
                case ConfigType::Boolean: oss << "bool"; break;
                case ConfigType::String: oss << "string"; break;
                case ConfigType::IntArray: oss << "int[]"; break;
                case ConfigType::DoubleArray: oss << "double[]"; break;
                case ConfigType::StringArray: oss << "string[]"; break;
            }
            if (entry.required) oss << " [REQUIRED]";
            if (entry.defaultValue.has_value()) oss << " [default available]";
            oss << " - " << entry.description << "\n";
        }
        return oss.str();
    }
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    enum class ErrorType {
        ValidationError,
        ParseError,
        FileError,
        TypeError,
        MissingRequired
    };
    
    ConfigException(const std::string& message, ErrorType type = ErrorType::ValidationError)
        : std::runtime_error(message), errorType(type) {}
    
    ErrorType getErrorType() const { return errorType; }
    
private:
    ErrorType errorType;
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
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
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Schema and validation (implementation in .cpp)
    const ConfigSchema& getSchema() const;
    void setValidateOnLoad(bool validate);
    void setSaveDefaults(bool save);
    void setVersion(const std::string& version);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Additional internal methods (implementation in .cpp)
    bool parseJson(const nlohmann::json& jsonData, ConfigSource source);
    void set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description);
    void setFromString(const std::string& key, const std::string& valueStr, 
                      ConfigType expectedType, ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Conversion functions (implementation in .cpp)
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& jsonValue, ConfigType expectedType);
    static std::string configValueToString(const ConfigValue& value);
    static bool validateConfigValue(const ConfigValue& value, ConfigType expectedType, 
                                   const std::string& key = "", const ConfigSchema* schema = nullptr);
    static void applySchemaDefaults(std::vector<ConfigEntry>& entries, const ConfigSchema& schema);
    static std::string errorToString(const std::exception& e);
    
    void initializeDefaultSchema();
};

} // namespace nlm
