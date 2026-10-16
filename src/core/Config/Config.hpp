#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <variant>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry for schema validation
struct ConfigEntry {
    std::string key;
    nlohmann::json value;
    ConfigSource source;
    std::string description;
    std::string type; // Type for schema validation ("int", "double", "string", "bool", "array", "object")
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type("any"), required(false) {}
    
    ConfigEntry(const std::string& k, const nlohmann::json& v, ConfigSource s, 
               const std::string& desc, const std::string& t = "any", bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), required(req) {}
};

// Schema validation errors
struct ConfigValidationError {
    std::string key;
    std::string message;
    std::string errorType;
    
    ConfigValidationError(const std::string& k, const std::string& msg, const std::string& type)
        : key(k), message(msg), errorType(type) {}
};

// Type helpers for schema validation
namespace ConfigType {
    const std::string INTEGER = "int";
    const std::string INTEGER64 = "int64";
    const std::string DOUBLE = "double";
    const std::string STRING = "string";
    const std::string BOOLEAN = "bool";
    const std::string ARRAY = "array";
    const std::string OBJECT = "object";
    const std::string ANY = "any";
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
    
    // Load from JSON/YAML file
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromString(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const nlohmann::json& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe convenience methods
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Schema validation
    std::vector<ConfigValidationError> validateSchema(const nlohmann::json& schema) const;
    
    // Schema definition helpers
    static nlohmann::json createSchema(const std::vector<ConfigEntry>& entries);
    
    // Type conversion helpers
    static nlohmann::json toJson(int value);
    static nlohmann::json toJson(int64_t value);
    static nlohmann::json toJson(double value);
    static nlohmann::json toJson(bool value);
    static nlohmann::json toJson(const std::string& value);
    static nlohmann::json toJson(const std::vector<int>& value);
    static nlohmann::json toJson(const std::vector<double>& value);
    static nlohmann::json toJson(const std::vector<std::string>& value);
    
    // JSON to type conversion
    static std::optional<int> fromJson(const nlohmann::json& json);
    static std::optional<int64_t> fromJson(const nlohmann::json& json);
    static std::optional<double> fromJson(const nlohmann::json& json);
    static std::optional<bool> fromJson(const nlohmann::json& json);
    static std::optional<std::string> fromJson(const nlohmann::json& json);
    static std::optional<std::vector<int>> fromJson(const nlohmann::json& json);
    static std::optional<std::vector<double>> fromJson(const nlohmann::json& json);
    static std::optional<std::vector<std::string>> fromJson(const nlohmann::json& json);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Schema validation helpers
    static bool validateValue(const nlohmann::json& value, const std::string& type) const;
    static std::string getJsonType(const nlohmann::json& value) const;
};

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;
extern template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
extern template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
extern template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

} // namespace nlm
