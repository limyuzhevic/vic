#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>

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
    std::vector<std::string>,
    std::unordered_map<std::string, std::string> // Nested configuration
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

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    enum class Type {
        FileNotFound,
        ParseError,
        ValidationError,
        SchemaError,
        IoError
    };
    
    ConfigError(Type type, const std::string& message) 
        : std::runtime_error(formatMessage(type, message)),
          errorType(type), message(message) {}
    
    Type getType() const { return errorType; }
    const std::string& getMessage() const { return message; }
    
private:
    Type errorType;
    std::string message;
    
    static std::string formatMessage(Type type, const std::string& message) {
        std::string typeStr;
        switch(type) {
            case Type::FileNotFound: typeStr = "FileNotFound"; break;
            case Type::ParseError: typeStr = "ParseError"; break;
            case Type::ValidationError: typeStr = "ValidationError"; break;
            case Type::SchemaError: typeStr = "SchemaError"; break;
            case Type::IoError: typeStr = "IoError"; break;
            default: typeStr = "Unknown"; break;
        }
        return "[" + typeStr + "] " + message;
    }
};

// Configuration schema validation
struct ConfigSchema {
    struct Field {
        std::string key;
        std::string type;
        bool required = false;
        std::string description;
        std::optional<std::variant<int, double>> min = std::nullopt;
        std::optional<std::variant<int, double>> max = std::nullopt;
        std::vector<std::string> allowedValues;
        // Additional schema properties
        std::string format;  // String format, e.g., "email", "uuid", "path"
        std::vector<std::string> dependencies;  // Keys that must be present
        std::vector<std::vector<std::string>> mutuallyExclusiveGroups;  // Groups of keys that cannot be set together
        std::vector<std::vector<std::string>> requiredCombinations;  // Sets of keys that must all be present
    };
    
    std::vector<Field> fields;
    std::string filename;
    std::string version;  // Schema version
    std::map<std::string, std::string> metadata;  // Additional metadata
    
    void addField(const std::string& key, const std::string& type, bool required = false, 
                  const std::string& description = "", const std::string& min = "",
                  const std::string& max = "", const std::vector<std::string>& allowed = {});

    void addFieldWithConstraints(const Field& field);
    
    bool validate(const std::unordered_map<std::string, ConfigValue>& config) const;
    std::string getValidationErrors(const std::unordered_map<std::string, ConfigValue>& config) const;
    static ConfigSchema loadFromFile(const std::string& schemaPath);
    static ConfigSchema loadFromJSON(const std::string& jsonString);
    static ConfigSchema loadFromYAML(const std::string& yamlString);
    
    std::string saveToJSON() const;
    std::string saveToYAML() const;
    
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Schema inheritance and merging
    void merge(const ConfigSchema& other);
    bool inheritsFrom(const ConfigSchema& parent) const;
    
    // Schema documentation generation
    std::string generateDocumentation() const;
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
    
    // Load from file (supports JSON, YAML formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (supports JSON, YAML formats)
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    std::string saveToJSON() const;
    
    // Save as YAML
    std::string saveToYAML() const;
    
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
    
    // Configuration validation
    bool validate(const ConfigSchema& schema) const;
    std::string getValidationErrors(const ConfigSchema& schema) const;
    
    // Load schema
    static ConfigSchema loadSchemaFromFile(const std::string& schemaPath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::string detectFileFormat(const std::string& filepath) const;
    std::string getValueAsString(const ConfigValue& value) const;
};

} // namespace nlm
