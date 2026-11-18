#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    double,
    float,
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

// Type validation functions
inline bool isNumericType(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<uint64_t>(value) || std::holds_alternative<double>(value) ||
           std::holds_alternative<float>(value);
}

inline bool isBooleanType(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

inline bool isStringType(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

inline bool isArrayType(const ConfigValue& value) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

inline std::string configValueTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<uint64_t>(value)) return "uint64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<float>(value)) return "float";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
    if (std::holds_alternative<std::vector<double>>(value)) return "double[]";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
    return "unknown";
}

inline double getNumericValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
    if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
    if (std::holds_alternative<uint64_t>(value)) return static_cast<double>(std::get<uint64_t>(value));
    if (std::holds_alternative<double>(value)) return std::get<double>(value);
    if (std::holds_alternative<float>(value)) return static_cast<double>(std::get<float>(value));
    return 0.0;
}

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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
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
    
#ifdef HAVE_NLOHMANN_JSON
    // JSON support
    void loadFromJson(const nlohmann::json& json);
    nlohmann::json toJson() const;
    std::string toPrettyString() const;
#endif
    
    // Configuration validation
    bool validateType(const std::string& key, const ConfigValue& expectedType) const;
    bool validateRange(const std::string& key, double min, double max) const;
    bool validate(const std::string& schemaFile) const;
    
    // Type conversion utilities
    static bool isNumericType(const ConfigValue& value);
    static bool isBooleanType(const ConfigValue& value);
    static bool isStringType(const ConfigValue& value);
    static bool isArrayType(const ConfigValue& value);
    static std::string configValueTypeName(const ConfigValue& value);
    static double getNumericValue(const ConfigValue& value);
    
    // Schema validation
    static bool validateSchema(const std::string& schemaFile) const;
    static nlohmann::json createDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
