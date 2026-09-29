#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types with nested structures and comprehensive array support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    // Basic array types
    std::vector<int>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    // Nested structures (objects)
    std::unordered_map<std::string, ConfigValue>,
    // Complex arrays of structs
    std::vector<std::unordered_map<std::string, ConfigValue>>
>;

// For nested structures and YAML mapping
using NestedConfig = std::unordered_map<std::string, ConfigValue>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    
    ValidationResult() : isValid(true) {}
    ValidationResult(bool valid, const std::string& error) : isValid(valid), errorMessage(error) {}
};

// Configuration schema entry with enhanced type support
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "array", "object", "vector"
    std::string elementType; // For arrays: "int", "double", "bool", "string", "object"
    bool required;
    std::string description;
    std::optional<std::string> defaultValue;
    std::optional<std::pair<double, double>> range; // min, max for numeric types
    std::optional<size_t> minSize; // For arrays
    std::optional<size_t> maxSize; // For arrays
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, bool req = false, 
                 const std::string& desc = "", const std::optional<std::string>& def = std::nullopt,
                 const std::optional<std::pair<double, double>>& rng = std::nullopt,
                 const std::optional<size_t>& minSz = std::nullopt,
                 const std::optional<size_t>& maxSz = std::nullopt)
        : key(k), type(t), elementType(""), required(req), description(desc), defaultValue(def), 
          range(rng), minSize(minSz), maxSize(maxSz) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& elemT, bool req = false,
                 const std::string& desc = "", const std::optional<std::string>& def = std::nullopt,
                 const std::optional<std::pair<double, double>>& rng = std::nullopt,
                 const std::optional<size_t>& minSz = std::nullopt,
                 const std::optional<size_t>& maxSz = std::nullopt)
        : key(k), type(t), elementType(elemT), required(req), description(desc), defaultValue(def),
          range(rng), minSize(minSz), maxSize(maxSz) {}
};

// Configuration metadata with enhanced tracking
struct ConfigMetadata {
    std::string sourceFile;
    std::chrono::system_clock::time_point timestamp;
    std::string format; // "json", "yaml", "ini", "text"
    std::string schemaName;
    std::map<std::string, std::string> customAttributes;
    
    ConfigMetadata() : sourceFile(""), timestamp(std::chrono::system_clock::now()), 
                      format(""), schemaName(""), customAttributes() {}
};

// Enhanced configuration entry with schema validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigMetadata metadata;
    bool validated; // Schema validation flag
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), metadata(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), metadata(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, const ConfigMetadata& meta)
        : key(k), value(v), source(s), description(desc), metadata(meta), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, 
                const ConfigMetadata& meta, bool validated)
        : key(k), value(v), source(s), description(desc), metadata(meta), validated(validated) {}
};

// JSON parser interface with enhanced type support
class JSONParser {
public:
    static bool parse(const std::string& jsonStr, std::unordered_map<std::string, ConfigValue>& config);
    static std::string serialize(const std::unordered_map<std::string, ConfigValue>& config, bool pretty = false);
    static bool isValidJSON(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string serializeValue(const ConfigValue& value);
    static bool parseValue(const std::string& str, ConfigValue& value);
    static std::string getJsonTypeName(const ConfigValue& value);
    static bool canConvertToType(const ConfigValue& from, const std::string& toType);
};

// YAML parser interface with enhanced support
class YAMLParser {
public:
    static bool parse(const std::string& yamlStr, std::unordered_map<std::string, ConfigValue>& config);
    static std::string serialize(const std::unordered_map<std::string, ConfigValue>& config, bool pretty = false);
    static bool isValidYAML(const std::string& str);
    static std::string trim(const std::string& str);
    static std::vector<std::string> splitLines(const std::string& str);
    static std::string serializeValue(const ConfigValue& value);
    static bool parseValue(const std::string& str, ConfigValue& value);
    static std::string getYamlTypeName(const ConfigValue& value);
    static std::string yamlEscapeString(const std::string& str);
};

// Enhanced configuration validator with comprehensive schema support
class ConfigValidator {
public:
    static ValidationResult validate(const std::unordered_map<std::string, ConfigValue>& config, 
                                    const std::vector<ConfigSchema>& schema);
    static ValidationResult validateValue(const std::string& type, const std::string& elementType,
                                         const ConfigValue& value,
                                         const std::optional<std::pair<double, double>>& range,
                                         const std::optional<size_t>& minSize,
                                         const std::optional<size_t>& maxSize);
    static bool isNumericType(const std::string& type);
    static bool isArrayType(const std::string& type);
    static bool isObjectType(const std::string& type);
    static std::string getTypeName(const ConfigValue& value);
    static std::string getElementTypeName(const ConfigValue& value);
    static bool validateArraySize(const ConfigValue& value, const std::optional<size_t>& minSize,
                                 const std::optional<size_t>& maxSize);
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
    Config& operator=(Config&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Get values with enhanced type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get array values with specific element type
    template<typename T>
    std::optional<std::vector<T>> getArray(const std::string& key) const;
    
    // Get object values
    std::optional<NestedConfig> getObject(const std::string& key) const;
    
    // Set values with enhanced metadata and validation support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& schemaName = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& schemaName = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& schemaName = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& schemaName = "");
    
    // Set array values
    void setArray(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime,
                  const std::string& description = "", const std::string& schemaName = "");
    void setArray(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime,
                  const std::string& description = "", const std::string& schemaName = "");
    void setArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime,
                  const std::string& description = "", const std::string& schemaName = "");
    void setObject(const std::string& key, const NestedConfig& value, ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "", const std::string& schemaName = "");
    
    // Get nested configuration values with enhanced support
    std::unordered_map<std::string, ConfigValue> getNested(const std::string& prefix = "") const;
    std::optional<ConfigValue> getNestedValue(const std::string& keyPath) const;
    
    // Configuration operations
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Get configuration summary with metadata
    std::string summary() const;
    
    // Enhanced configuration validation
    ValidationResult validate(const std::vector<ConfigSchema>& schema) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration merging with enhanced support
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get/set metadata
    std::optional<ConfigMetadata> getMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const ConfigMetadata& metadata);
    
    // Configuration inheritance
    void applyInheritance(const Config& parent, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration comparison tools
    std::string diff(const Config& other) const;
    
    // Type conversion utilities
    template<typename T>
    static T convertValue(const ConfigValue& value);
    
    // Schema utilities
    static std::string getDefaultSchemaName(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectFormat(const std::string& filepath) const;
    static std::string getFileExtension(const std::string& filepath) const;
    static std::string joinKeyPath(const std::vector<std::string>& parts);
    static std::vector<std::string> splitKeyPath(const std::string& keyPath);
};

} // namespace nlm
