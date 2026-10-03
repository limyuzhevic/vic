#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <fstream>
#include <regex>

// Forward declarations for JSON/YAML parsing libraries
namespace nlohmann {
    template<typename T> class json;
}

namespace YAML {
    class Node;
}

namespace nlm {

// Configuration value types with more comprehensive support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<double>> // For matrix parameters
>;

// Configuration data types for schema validation
enum class ConfigDataType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    StringVector,
    IntVector,
    DoubleVector,
    BoolVector,
    Matrix // 2D vector
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string path; // Configuration path for nested configs
};

// Configuration source tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Environment,
    Runtime,
    Inherited
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigDataType dataType;
    bool required;
    std::string defaultValue;
    std::string validationRegex;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), 
          dataType(ConfigDataType::String), required(false), defaultValue(),
          validationRegex() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "", ConfigDataType dt = ConfigDataType::String,
                bool req = false, const std::string& def = "")
        : key(k), value(v), source(s), description(desc), 
          dataType(dt), required(req), defaultValue(def), validationRegex() {}
};

// Schema definition for configuration validation
struct ConfigSchema {
    std::string name;
    std::unordered_map<std::string, ConfigEntry> entries;
    std::string version;
    std::string description;
    
    bool validate() const {
        for (const auto& pair : entries) {
            const auto& entry = pair.second;
            if (entry.required) {
                // Check if required entry is provided
                // This would need to be implemented in Config class
            }
        }
        return true;
    }
};

// Parent-child configuration relationship
class ParentConfig {
public:
    virtual ~ParentConfig() = default;
    virtual bool has(const std::string& key) const = 0;
    virtual ConfigValue get(const std::string& key) const = 0;
    virtual ConfigSource getSource(const std::string& key) const = 0;
};

// Main enhanced configuration class
class Config {
public:
    Config(const std::string& version = "1.0.0");
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with JSON/YAML support
    bool loadFromFile(const std::string& filepath, bool inheritFromDefaults = true);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr, bool inheritFromDefaults = true);
    
    // Load from YAML string  
    bool loadFromYAML(const std::string& yamlStr, bool inheritFromDefaults = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool inheritFromDefaults = true);
    
    // Save to file in JSON format
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced get with source tracking
    template<typename T>
    std::optional<T> get(const std::string& key, ConfigSource& sourceOut) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigDataType dataType = ConfigDataType::String);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigDataType dataType = ConfigDataType::String);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigDataType dataType = ConfigDataType::Int);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigDataType dataType = ConfigDataType::Double);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", ConfigDataType dataType = ConfigDataType::Bool);
    
    // Check existence and source
    bool has(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Configuration version management
    std::string getVersion() const { return version; }
    void setVersion(const std::string& ver) { version = ver; }
    
    // Environment variable expansion
    void setEnvironmentPrefix(const std::string& prefix) { envPrefix = prefix; }
    bool expandEnvironmentVariables();
    
    // Parameter inheritance
    void setParentConfig(std::shared_ptr<ParentConfig> parent) { parentConfig = parent; }
    void clearParentConfig() { parentConfig.reset(); }
    bool inheritFromParent(const std::string& key) const;
    
    // Validation and schema
    bool validate(const ConfigSchema& schema, std::vector<ValidationResult>& results) const;
    bool validate() const; // Basic validation
    
    // Configuration comparison and merging
    bool merge(const Config& other, bool overrideDefaults = true);
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Documentation generation
    std::string generateDocumentation() const;
    std::string generateMarkdownDocumentation() const;
    
    // Get configuration summary with enhanced info
    std::string summary() const;
    std::string detailedSummary() const;
    
    // Error handling
    std::string getLastError() const { return lastError; }
    void clearLastError() { lastError.clear(); }
    
    // File operations with error handling
    static bool fileExists(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    
    // Configuration schema management
    bool loadSchemaFromFile(const std::string& schemaPath);
    const ConfigSchema& getSchema() const { return schema; }
    
    // Type conversion utilities
    static std::string configValueToString(const ConfigValue& value);
    static bool stringToConfigValue(const std::string& str, ConfigValue& value, ConfigDataType targetType);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration version
    std::string version;
    
    // Parent configuration for inheritance
    std::shared_ptr<ParentConfig> parentConfig;
    
    // Environment variable prefix
    std::string envPrefix;
    
    // Schema validation
    ConfigSchema schema;
    
    // Error tracking
    std::string lastError;
    
    // File type detection and loading
    bool loadFromFileInternal(const std::string& filepath, bool inheritFromDefaults);
    bool loadFromJSONInternal(const std::string& content, bool inheritFromDefaults);
    bool loadFromYAMLInternal(const std::string& content, bool inheritFromDefaults);
    
    // Environment variable processing
    std::string expandVariable(const std::string& varName) const;
    bool isEnvironmentVariable(const std::string& str) const;
    
    // Type conversion helpers
    template<typename T>
    static bool convertToType(const ConfigValue& from, T& to);
    
    // Internal helpers (enhanced)
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractVariableName(const std::string& str);
    static bool validateRegex(const std::string& value, const std::string& pattern);
    
    // JSON/YAML parsing helpers
    static bool parseSimpleValue(const std::string& str, ConfigValue& value, ConfigDataType& type);
    static std::string serializeToJSON(const ConfigEntry& entry) const;
    static bool parseJSONValue(const std::string& jsonStr, ConfigValue& value, ConfigDataType& type);
};

} // namespace nlm