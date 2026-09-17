#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <stdexcept>
#include <limits>

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

// Configuration validation error
struct ConfigValidationError {
    std::string field;
    std::string message;
    std::string value;
    
    ConfigValidationError(const std::string& f, const std::string& m, const std::string& v = "")
        : field(f), message(m), value(v) {}
};

// Configuration schema field definition
struct SchemaField {
    std::string name;
    std::string type;
    bool required;
    std::string defaultValue;
    std::string description;
    std::vector<std::string> allowedValues;
    
    SchemaField(const std::string& n, const std::string& t, bool r = false, 
                const std::string& d = "", const std::string& desc = "",
                const std::vector<std::string>& a = {})
        : name(n), type(t), required(r), defaultValue(d), 
          description(desc), allowedValues(a) {}
};

// Configuration schema
class ConfigSchema {
public:
    void addField(const std::string& name, const std::string& type, 
                  bool required = false, const std::string& defaultValue = "",
                  const std::string& description = "",
                  const std::vector<std::string>& allowedValues = {});
    
    bool validate(const nlohmann::json& config, std::vector<ConfigValidationError>& errors) const;
    
    std::vector<std::string> getRequiredFields() const;
    std::vector<std::string> getOptionalFields() const;
    
    // Get field info
    const SchemaField* getField(const std::string& name) const;
    
private:
    std::map<std::string, SchemaField> fields_;
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

// Main configuration class with JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath, const ConfigSchema* schema = nullptr);
    
    // Backward compatibility: load from key=value file
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToJsonFile(const std::string& filepath) const;
    
    // Backward compatibility: save to key=value file
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
    
    // Set entries (for loading from JSON)
    void setEntries(const std::vector<ConfigEntry>& entries, ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Parse key=value line
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    
    // Convert JSON to ConfigValue
    static bool jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    
    // Convert ConfigValue to JSON
    static nlohmann::json configValueToJson(const ConfigValue& configValue);
    
    // Extract value from ConfigValue to specific type
    template<typename T>
    static T extractValue(const ConfigValue& value);
};

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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
}
