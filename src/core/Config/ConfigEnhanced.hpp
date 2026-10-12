// NLM (熙然) - Neural Learning Machine
// Phase 8: Enhanced Configuration System
//
// This phase enhances the configuration system with:
// - JSON/YAML support for configuration files
// - Proper value serialization/deserialization
// - Enhanced error handling and validation
// - Configuration schema validation
// - Command-line argument parsing improvements

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <regex>
#include <filesystem>

namespace nlm {

// Enhanced exception hierarchy for configuration errors
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error("Config Error: " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    explicit ConfigFileException(const std::string& message)
        : ConfigException("File Error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message)
        : ConfigException("Parse Error: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigException("Validation Error: " + message) {}
};

// Enhanced configuration value types with better type safety
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

// Enhanced configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    bool required = false;
    std::string validationRegex;
    std::string defaultValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type("string"), required(false), defaultValue() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "string",
                bool req = false, const std::string& regex = "",
                const std::string& def = "")
        : key(k), value(v), source(s), description(desc), type(t), 
          required(req), validationRegex(regex), defaultValue(def) {}
};

// Configuration schema for validation
class ConfigSchema {
public:
    using Validator = std::function<bool(const ConfigValue&)>;
    
    void addField(const std::string& key, const std::string& type,
                  bool required = false, const std::string& description = "",
                  const std::string& validationRegex = "",
                  const std::string& defaultValue = "");
    
    void addValidator(const std::string& key, Validator validator);
    
    std::vector<std::string> validate(const std::vector<ConfigEntry>& entries) const;
    
    std::string getFieldDescription(const std::string& key) const;
    
    std::string getFieldType(const std::string& key) const;
    
    bool isRequired(const std::string& key) const;
    
    const std::unordered_map<std::string, std::string>& getFieldMetadata() const { return fieldMetadata; }
    
private:
    struct FieldInfo {
        std::string type;
        bool required;
        std::string description;
        std::string validationRegex;
        std::string defaultValue;
        std::vector<Validator> validators;
    };
    
    std::unordered_map<std::string, FieldInfo> schema;
    std::unordered_map<std::string, std::string> fieldMetadata;
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from YAML file
    bool loadFromYaml(const std::string& filepath, bool validate = true);
    
    // Load from JSON file
    bool loadFromJson(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYaml(const std::string& filepath) const;
    
    // Save to JSON file
    bool saveToJson(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<int>& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<double>& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::vector<std::string>& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
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
    
    // Validation
    std::vector<std::string> validate(const ConfigSchema& schema) const;
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema) { configSchema = schema; }
    
    // Get source information
    ConfigSource getSource(const std::string& key) const;
    
    // Check if value changed from default
    bool isModified(const std::string& key) const;
    
    // Get all entries with metadata
    std::vector<ConfigEntry> getAllEntries() const { return pImpl->entries; }
    
    // Configuration merge operations
    void merge(const Config& other, ConfigSource source = ConfigSource::CommandLine);
    
    // Configuration diff operations
    std::vector<std::string> diff(const Config& other) const;
    
    // Import/export operations
    std::string exportToJson() const;
    bool importFromJson(const std::string& json);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable ConfigSchema configSchema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesRegex(const std::string& value, const std::string& regex);
    static std::string valueToString(const ConfigValue& value);
    static std::string configValueToJson(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const std::string& jsonValue, const std::string& type);
    
    // File parsing helpers
    static std::string parseJsonValue(const std::string& json, size_t& pos);
    static void skipWhitespace(const std::string& str, size_t& pos);
    static std::string parseString(const std::string& str, size_t& pos);
    static bool parseBool(const std::string& str);
    static double parseNumber(const std::string& str);
    
    // Schema validation helpers
    static bool validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema::FieldInfo& fieldInfo);
    
    // Type conversion helpers
    template<typename T>
    static std::string typeToString();
    template<typename T>
    static T convertToType(const ConfigValue& value);
    
    // Singleton schema for common configuration
    static ConfigSchema getDefaultSchema();
    
    friend class ConfigJsonParser;
    friend class ConfigYamlParser;
};

// JSON Parser helper class
class ConfigJsonParser {
public:
    static bool parse(const std::string& json, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
private:
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

// YAML Parser helper class  
class ConfigYamlParser {
public:
    static bool parse(const std::string& yaml, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
private:
    static std::string yamlEscape(const std::string& str);
};

} // namespace nlm