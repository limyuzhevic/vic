#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Type name for values
inline std::string configTypeToString(const ConfigValue& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<std::string>";
        else return "unknown";
    }, value);
}

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

// Configuration field metadata for schema validation
struct ConfigField {
    std::string key;
    std::string type;           // Type as string ("int", "double", "bool", "string", "vector<int>", etc.)
    std::string description;    // Human-readable description
    bool required = false;       // Whether this field is required
    std::string defaultValue;   // Default value as string (optional)
    std::string validation;     // Validation pattern/constraints (optional)
    
    ConfigField() = default;
    ConfigField(const std::string& k, const std::string& t, const std::string& d, 
                bool req = false, const std::string& def = "", const std::string& val = "")
        : key(k), type(t), description(d), required(req), defaultValue(def), validation(val) {}
};

// Configuration schema definition
class ConfigSchema {
public:
    ConfigSchema();
    
    // Add a field definition
    void addField(const std::string& key, const std::string& type, 
                  const std::string& description, bool required = false,
                  const std::string& defaultValue = "", 
                  const std::string& validation = "");
    
    // Get all field definitions
    const std::vector<ConfigField>& getFields() const { return pImpl->fields_; }
    
    // Validate configuration against schema
    std::vector<std::string> validate(const Config& config) const;
    
    // Get field by key
    const ConfigField* getField(const std::string& key) const;
    
    // Get field type string for validation
    std::string getFieldType(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global default schema for NLM
ConfigSchema createDefaultNLMConfigSchema();

// Configuration value type helper functions
inline std::string valueToString(const ConfigValue& value);
inline std::string stringToValue(const std::string& str, const std::string& typeHint = "");

// Type conversion helpers
inline std::string toConfigString(bool value);
inline std::string toConfigString(int value);
inline std::string toConfigString(int64_t value);
inline std::string toConfigString(double value);

// Configuration validation
inline bool validateConfigEntry(const std::string& key, const ConfigValue& value);

// Value description helper
inline std::string getValueDescription(const ConfigEntry& entry);

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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
