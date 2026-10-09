#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlm {

// Forward declarations
class Config;
class ConfigParser;
class JSONParser;
class YAMLParser;
class ConfigSchema;
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
    std::vector<std::vector<int>>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Parser interface for different configuration formats
class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    virtual bool parse(const std::string& content, std::vector<ConfigEntry>& entries) = 0;
    virtual std::string getFormatName() const = 0;
    virtual std::vector<std::string> getSupportedExtensions() const = 0;
};

// Schema validation types
struct ValidationRule {
    std::string fieldName;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool required = false;
    std::string defaultValue;
};

// Configuration schema definition
class ConfigSchema {
public:
    void addRule(const ValidationRule& rule);
    bool validate(const ConfigEntry& entry) const;
    bool validateAll(const std::vector<ConfigEntry>& entries) const;
    std::vector<std::string> getErrors() const;
    
private:
    std::vector<ValidationRule> rules_;
    mutable std::vector<std::string> errors_;
};

// Configuration validator
class ConfigValidator {
public:
    explicit ConfigValidator(const ConfigSchema& schema);
    bool validate(const ConfigEntry& entry) const;
    bool validateAll(const std::vector<ConfigEntry>& entries) const;
    std::vector<std::string> getErrors() const;
    
private:
    const ConfigSchema& schema_;
};

// File format detection
enum class ConfigFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML
};

class ConfigFormatDetector {
public:
    static ConfigFormat detectFromExtension(const std::string& filepath);
    static ConfigFormat detectFromContent(const std::string& content);
    static std::string formatToString(ConfigFormat format);
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
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
    
    // Load from file with explicit format
    bool loadFromFile(const std::string& filepath, ConfigFormat format);
    
    // Load from string content with format specification
    bool loadFromString(const std::string& content, ConfigFormat format);
    
    // Load from JSON string (for backward compatibility)
    bool loadFromJSON(const std::string& jsonContent);
    
    // Load from YAML string (for backward compatibility)
    bool loadFromYAML(const std::string& yamlContent);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::Unknown) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get complex types
    std::vector<int> getIntList(const std::string& key) const;
    std::vector<double> getDoubleList(const std::string& key) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    std::vector<std::vector<int>> getIntMatrix(const std::string& key) const;
    
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
    
    // Validation and schema
    bool setSchema(const ConfigSchema& schema);
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Format detection
    static ConfigFormat detectFormat(const std::string& filepath);
    static ConfigFormat detectFormatFromContent(const std::string& content);
    
    // Parser management
    static std::unique_ptr<ConfigParser> createParser(ConfigFormat format);
    static std::vector<ConfigFormat> getSupportedFormats();
    
    // Command line options
    bool loadFromAdvancedArgs(int argc, char** argv);
    std::string getHelpText() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigSchema> schema_;
    mutable std::vector<std::string> validationErrors_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseSimpleKeyValue(const std::string& line, std::string& key, std::string& value);
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
};
