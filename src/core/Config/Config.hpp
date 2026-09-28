#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
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
    std::vector<std::string>,
    std::map<std::string, ConfigValue>  // Support for nested objects
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
enum class ConfigErrorType {
    FileNotFound,
    ParseError,
    InvalidFormat,
    TypeMismatch,
    ValidationError,
    SchemaError
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(ConfigErrorType type, const std::string& message, int line = -1, const std::string& filename = "")
        : std::runtime_error(message), errorType(type), lineNumber(line), filename(filename) {}
    
    ConfigErrorType getErrorType() const { return errorType; }
    int getLineNumber() const { return lineNumber; }
    const std::string& getFilename() const { return filename; }
    
private:
    ConfigErrorType errorType;
    int lineNumber;
    std::string filename;
};

// File format detection
enum class FileFormat {
    Unknown,
    KeyValue,
    JSON,
    YAML
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
    
    // Load from file (auto-detects JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from key=value format (legacy)
    bool loadFromKeyValue(const std::string& keyValueString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format from extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // Save to YAML string
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
    
    // Set nested value (e.g., set("a.b.c", value))
    bool setNested(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get nested value
    template<typename T>
    std::optional<T> getNested(const std::string& key) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Remove nested key
    bool removeNested(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys with given prefix
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // File type detection
    static FileFormat detectFileFormat(const std::string& filename);
    
    // Validation
    bool validate() const;
    
    // Schema validation (basic)
    bool validateWithSchema(const std::string& schemaFile) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filename);
    
    // Parser helpers
    bool parseJSONInternal(const std::string& content);
    bool parseYAMLInternal(const std::string& content);
    bool parseKeyValueInternal(const std::string& content);
    
    // Serializer helpers
    void serializeToJSON(std::ostream& os) const;
    void serializeToYAML(std::ostream& os) const;
    void serializeToKeyValue(std::ostream& os) const;
    
    // Key extraction for nested values
    static std::vector<std::string> splitKey(const std::string& key);
    static std::string joinKey(const std::vector<std::string>& parts);
};

} // namespace nlm
