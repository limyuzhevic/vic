#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <functional>
#include <stdexcept>
#include <map>
#include <iostream>

namespace nlm {

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

// Error types for configuration
struct ConfigError {
    std::string message;
    std::string key;
    enum class Type { ParseError, ValidationError, ConversionError } type;
};

struct ConfigValidationError {
    std::string key;
    std::string message;
};

// Configuration schema validation
struct ConfigSchema {
    std::string key;
    std::string description;
    bool required;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Parser options
struct ParserOptions {
    bool validateSchema;
    bool supportNestedKeys;
    bool preserveOrder;
    std::string configSource;
};

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
    
    // Load from file (JSON format or legacy key=value format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromFileLegacy(const std::string& filepath); // Backward compatibility
    
    // Load from string (JSON or key=value)
    bool loadFromJSONString(const std::string& jsonString);
    bool loadFromStringLegacy(const std::string& text); // Backward compatibility
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string saveToJSONString() const;
    std::string saveToStringLegacy() const; // Backward compatibility
    
    // Error handling
    std::string getLastError() const;
    bool hasError() const;
    void clearError();
    
    // Validation
    bool validateSchema(const std::vector<ConfigSchema>& schema);
    std::vector<ConfigValidationError> validateCurrent() const;
    
    // Parser configuration
    void setParserOptions(const ParserOptions& options);
    ParserOptions getParserOptions() const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    void clearSchema();
    
    // Debug/logging
    std::string getConfigInfo() const;
    static std::string getVersion();
    
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
    
private:
    // Private methods for JSON parsing
    bool loadFromJSON(const std::string& filepath, const ParserOptions& options);
    bool loadFromJSONString(const std::string& jsonString, const ParserOptions& options);
    void flattenAndLoadJSON(const nlohmann::json& json, ConfigSource source, const ParserOptions& options);
    std::string flattenKey(const std::string& key, const nlohmann::json& parent) const;
    void setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source);
    
    // Validation
    bool validateParameter(const std::string& key, const ConfigValue& value) const;
    bool validateType(const std::string& key, const ConfigValue& value) const;
    bool validateRange(const std::string& key, const ConfigValue& value) const;
    
    // Save helpers
    void saveToJSON(const std::string& filepath) const;
    nlohmann::json buildJSONConfig() const;
    
    // Error handling
    void setError(const std::string& message, ConfigError::Type type, const std::string& key = "");
    
    // Parser helpers
    bool detectAndLoad(const std::string& content); // Auto-detect format
    
    // Legacy parsing
    bool parseLegacyLine(const std::string& line);
    
    // Legacy save
    void saveToLegacyFormat(std::ofstream& file) const;
    
    // Schema validation
    static bool validate(const nlohmann::json& config, std::vector<ConfigValidationError>& errors);
    static bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    static nlohmann::json configValueToJson(const ConfigValue& configValue);
    static std::string getErrorString(ConfigError::Type type);
    
    // Static schema validation
    static bool validate(const nlohmann::json& config, std::vector<ConfigValidationError>& errors);
    static bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    static nlohmann::json configValueToJson(const ConfigValue& configValue);
    static std::string getErrorString(ConfigError::Type type);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
