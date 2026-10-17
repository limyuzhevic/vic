// Enhanced Config.hpp with JSON support and validation
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

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
    Runtime,
    Programmatic
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    static ValidationResult success() { return {true, ""}; }
    static ValidationResult error(const std::string& msg) { return {false, msg}; }
};

// Type conversion utilities
class ConfigTypeConverter {
public:
    static bool stringToBool(const std::string& str, bool& result);
    static bool stringToInt(const std::string& str, int& result);
    static bool stringToDouble(const std::string& str, double& result);
    static bool stringToInt64(const std::string& str, int64_t& result);
    
    static std::string boolToString(bool value);
    static std::string intToString(int value);
    static std::string doubleToString(double value);
    static std::string int64ToString(int64_t value);
};

// Configuration validation schema
class ConfigSchema {
public:
    struct FieldInfo {
        std::string key;
        ConfigValue defaultValue;
        ConfigSource defaultSource;
        std::string description;
        std::function<ValidationResult(const ConfigValue&)> validator;
        bool required;
    };
    
    void addField(const std::string& key, const ConfigValue& defaultValue, 
                  ConfigSource source, const std::string& description,
                  std::function<ValidationResult(const ConfigValue&)> validator = nullptr,
                  bool required = false);
    
    const FieldInfo* getField(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    std::string getDescription(const std::string& key) const;
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getRequiredFields() const;
    
private:
    std::unordered_map<std::string, FieldInfo> fields_;
};

// Enhanced ConfigEntry with better metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    uint64_t timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", uint64_t ts = 0)
        : key(k), value(v), source(s), description(desc), timestamp(ts) {}
};

// Main configuration class with JSON support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Initialize with schema for validation
    void initializeFromSchema(const ConfigSchema& schema);
    
    // Load from various sources
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromJSONFile(const std::string& filepath, bool validate = true);
    bool loadFromYAMLFile(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool includeDescriptions = true) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    
    // Enhanced configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    std::string summary() const;
    std::string getDescription(const std::string& key) const;
    bool isValidated(const std::string& key) const;
    
    // Configuration validation
    ValidationResult validate() const;
    bool validate(const std::string& key) const;
    void setValidator(const std::string& key, std::function<ValidationResult(const ConfigValue&)> validator);
    
    // Configuration comparison and merging
    bool merge(const Config& other, bool overwrite = true);
    bool equals(const Config& other) const;
    
    // Clear and reset
    void clear();
    void resetToDefaults();
    
    // Configuration information
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    uint64_t getTimestamp(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool isBoolean(const std::string& str);
    
    // JSON parsing helpers
    static std::string escapeJSON(const std::string& str);
    static std::string unescapeJSON(const std::string& str);
    
    // File parsing helpers
    static bool parseSimpleConfig(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseJSONConfig(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseYAMLConfig(const std::string& content, std::vector<ConfigEntry>& entries);
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