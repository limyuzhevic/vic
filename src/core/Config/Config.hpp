#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <set>
#include <cctype>
#include <nlohmann/json.hpp>

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

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    // Type tracking for validation
    std::string typeHint;
    
    // Validation info
    bool isValidated = false;
    std::vector<std::string> validationErrors;
    
    // Metadata
    std::string fileSource;
    size_t lineNumber = 0;
    std::string originalFormat;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   typeHint(), isValidated(false), lineNumber(0) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), typeHint(), isValidated(false), lineNumber(0) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, 
                const std::string& type, const std::string& file, size_t line)
        : key(k), value(v), source(s), description(desc), typeHint(type), isValidated(false),
          fileSource(file), lineNumber(line) {}
};

// Known configuration schema
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64_t", "double", "bool", "string", "array", "object"
    std::string description;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    std::map<std::string, std::string> constraints;
    bool required = false;
};

// Configuration parser class
class ConfigParser {
public:
    // Parse string to appropriate type
    static std::optional<ConfigValue> parseValue(const std::string& str, const std::string& typeHint = "");
    
    // Parse key=value format
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    
    // Parse JSON from string
    static nlohmann::json parseJSON(const std::string& jsonStr);
    
    // Parse YAML from string (simple implementation)
    static bool parseYAML(const std::string& yamlStr, std::map<std::string, std::string>& result);
    
    // Merge multiple config sources
    static std::vector<ConfigEntry> mergeConfigs(const std::vector<std::vector<ConfigEntry>>& configSources);
    
    // Validate config against schema
    static std::vector<std::string> validateConfig(const std::vector<ConfigEntry>& entries);
    
    // Check if file is JSON
    static bool isJSONFile(const std::string& filepath);
    
    // Check if file is YAML
    static bool isYAMLFile(const std::string& filepath);
    
    // Get file extension
    static std::string getFileExtension(const std::string& filepath);
    
    // Convert value to string for JSON/YAML output
    static std::string configValueToString(const ConfigValue& value);
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
    
    // Load from file (JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values - backward compatible
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values with enhanced type safety - throws exceptions on type mismatch
    template<typename T>
    T getStrict(const std::string& key) const;
    
    // Get values with detailed error information
    template<typename T>
    std::pair<std::optional<T>, std::string> getWithError(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Parse helper methods
    bool parseFromJSON(const std::string& content, const std::string& filepath);
    bool parseFromYAML(const std::string& content, const std::string& filepath);
    bool parseFromKeyValue(const std::string& content, const std::string& filepath);
    
    // Merge helper methods
    void mergeEntry(const ConfigEntry& entry);
    
    // Validation helper methods
    static bool validateSchema(const ConfigEntry& entry);
    static bool validateType(const ConfigEntry& entry);
    static bool validateConstraints(const ConfigEntry& entry);
    
    // Type conversion helpers
    template<typename T>
    static bool valueMatchesType(const ConfigValue& value);
    
    // JSON/YAML serialization helpers
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static bool jsonToConfigValue(const nlohmann::json& json, ConfigValue& value);
    
    // Type safety helpers
    static std::string getTypeDescription(const ConfigValue& value);
    static std::string getExpectedTypeDescription(const std::string& typeHint);
};

} // namespace nlm

// Type safety exception classes
class ConfigTypeError : public std::runtime_error {
public:
    explicit ConfigTypeError(const std::string& message, 
                            const std::string& key = "",
                            const std::string& expected = "",
                            const std::string& actual = "");
    
    const std::string& getKey() const { return key_; }
    const std::string& getExpectedType() const { return expected_; }
    const std::string& getActualType() const { return actual_; }
    
    std::string whatDetailed() const;
private:
    std::string key_;
    std::string expected_;
    std::string actual_;
};

class ConfigCastError : public std::runtime_error {
public:
    explicit ConfigCastError(const std::string& message,
                            const std::string& key = "",
                            const std::string& fromType = "",
                            const std::string& toType = "");
    
    const std::string& getKey() const { return key_; }
    const std::string& getFromType() const { return fromType_; }
    const std::string& getToType() const { return toType_; }
    
    std::string whatDetailed() const;
private:
    std::string key_;
    std::string fromType_;
    std::string toType_;
};

// Type checking namespace
namespace TypeUtils {
    // Get the actual type name from ConfigValue
    template<typename T>
    std::string getActualTypeName(const ConfigValue& value);
    
    // Check if a requested type matches the actual stored type
    template<typename T>
    bool isTypeMatch(const ConfigValue& value, const std::string& key = "");
    
    // Get actual type name from ConfigValue using visitor
    std::string getActualTypeName(const ConfigValue& value);
    
    // Check type compatibility for variant access
    bool isTypeCompatible(const std::string& expected, const std::string& actual);
    
    // Get detailed type description
    std::string getTypeDescription(const ConfigValue& value);
    
    // Get expected type description
    std::string getExpectedTypeDescription(const std::string& typeHint);
}

// Safe type casting utilities
namespace SafeCast {
    // Cast between numeric types
    template<typename From, typename To>
    To numericCast(const From& value, const std::string& key = "");
    
    // Safe string conversion
    template<typename T>
    std::string toString(const T& value);
}

// Configuration validation with enhanced type checking
namespace ConfigValidation {
    // Comprehensive validation with detailed error reporting
    std::vector<std::string> validateConfigWithTypeChecking(const std::vector<ConfigEntry>& entries);
}
