#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

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
    JSON,
    YAML
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

// Type traits for ConfigValue to enable type checking
namespace config_traits {
    template<typename T, typename = void>
    struct is_valid : std::false_type {};
    
    template<typename T>
    struct is_valid<T, std::void_t<decltype(std::declval<ConfigValue>().template operator T())>> : std::true_type {};
    
    template<typename T>
    struct is_vector : std::is_same<T, std::vector<typename T::value_type>> {};
    
    template<typename T>
    struct is_primitive : std::disjunction<std::is_same<T, int>, std::is_same<T, int64_t>, 
                                           std::is_same<T, double>, std::is_same<T, bool>, 
                                           std::is_same<T, std::string>> {};
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string toJSON() const;
    
    // Save to YAML string
    std::string toYAML() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get value with type checking
    template<typename T>
    std::optional<T> getChecked(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set vector values
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
    
    // Validation
    bool validate() const;
    
    // Configuration descriptions (for help/documentation)
    static const std::unordered_map<std::string, std::string> getDefaultDescriptions();
    
    // Configuration schemas (for validation)
    struct ConfigSchema {
        std::string key;
        std::string type; // "int", "double", "bool", "string", "vector"
        ConfigValue defaultValue;
        std::string description;
        bool required = false;
    };
    
    static const std::vector<ConfigSchema>& getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static ConfigValue stringToValue(const std::string& str);
    static std::string valueToString(const ConfigValue& value);
    
    // JSON/YAML parsing helpers
    static bool parseJSON(const std::string& jsonString, std::vector<ConfigEntry>& entries);
    static bool parseYAML(const std::string& yamlString, std::vector<ConfigEntry>& entries);
    static bool serializeToJSON(const std::vector<ConfigEntry>& entries, std::string& jsonString) const;
    static bool serializeToYAML(const std::vector<ConfigEntry>& entries, std::string& yamlString) const;
    
    // Validation helpers
    static bool validateAgainstSchema(const std::vector<ConfigEntry>& entries);
};

} // namespace nlm
