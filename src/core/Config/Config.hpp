#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <regex>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Config;
class Logger;

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

// Configuration validation types
enum class ValidationType {
    None,
    Range,
    Enum,
    Pattern,
    Custom
};

struct ValidationConstraint {
    ValidationType type;
    std::string errorMessage;
    
    // Range constraints
    double minValue;
    double maxValue;
    
    // Enum constraints
    std::vector<std::string> enumValues;
    
    // Pattern constraint
    std::string patternRegex;
    
    ValidationConstraint() : type(ValidationType::None), minValue(0.0), maxValue(0.0) {}
    
    bool validate(const ConfigValue& value) const;
    std::string getTypeName() const;
};

struct ConfigValidation {
    std::string key;
    ValidationConstraint constraint;
    bool enabled = true;
};

class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& key, const std::string& message)
        : std::runtime_error("Configuration validation error for key '" + key + "': " + message) {}
};

class ConfigParseError : public std::runtime_error {
public:
    ConfigParseError(const std::string& filepath, const std::string& message)
        : std::runtime_error("Configuration parse error in file '" + filepath + "': " + message) {}
};

class ConfigLoadError : public std::runtime_error {
public:
    ConfigLoadError(const std::string& filepath, const std::string& message)
        : std::runtime_error("Configuration load error in file '" + filepath + "': " + message) {}
};

// Parser result
struct ParseResult {
    std::string key;
    std::string rawValue;
    std::string source;
    bool valid;
    std::string errorMessage;
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
    
    // Validation methods
    void addValidation(const std::string& key, const ValidationConstraint& constraint);
    bool validate(const std::string& key) const;
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Logging
    void setLogger(Logger* logger) { logger_ = logger; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Logger* logger_ = nullptr;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isFileExists(const std::string& filepath);
    static std::string getFileExtension(const std::string& filename);
    static std::string detectFileFormat(const std::string& filepath);
    
    // Parsing helpers
    static ParseResult parseLine(const std::string& line);
    static ConfigValue parseValue(const std::string& valueStr, const std::string& key);
    static std::string valueToString(const ConfigValue& value) const;
    static bool isNumeric(const std::string& str);
    static bool isBoolean(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    
    // JSON helpers
    static rapidjson::Document parseJsonFile(const std::string& filepath);
    static rapidjson::Value jsonValueToConfigValue(const rapidjson::Value& jsonValue, const std::string& key, rapidjson::Document::AllocatorType& allocator);
    static rapidjson::Value configValueToJson(const ConfigValue& configValue, rapidjson::Document::AllocatorType& allocator);
    
    // YAML helpers
    static YAML::Node parseYamlFile(const std::string& filepath);
    static ConfigValue yamlValueToConfigValue(const YAML::Node& yamlValue, const std::string& key);
    static YAML::Node configValueToYaml(const ConfigValue& configValue);
    
    // File format detection
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isLegacyConfigFile(const std::string& filepath);
};