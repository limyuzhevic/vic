#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <set>
#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <sstream>
#include <ctime>

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
    Environment,
    Template,
    Inherited
};

// Configuration file format
enum class ConfigFormat {
    Unknown,
    JSON,
    YAML,
    INI,
    CFG,
    Properties,
    XML,
    Simple
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string validator;
    bool is_required;
    std::string default_value;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   timestamp(std::chrono::system_clock::now()), validator(), 
                   is_required(false), default_value() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()), validator(), 
          is_required(false), default_value() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc, bool required, const std::string& def)
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()), validator(), 
          is_required(required), default_value(def) {}
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::string key;
    ConfigValue old_value;
    ConfigValue new_value;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string change_reason;
    
    ConfigHistoryEntry(const std::string& k, const ConfigValue& oldV, const ConfigValue& newV,
                      ConfigSource src, const std::string& reason = "")
        : key(k), old_value(oldV), new_value(newV), source(src),
          timestamp(std::chrono::system_clock::now()), change_reason(reason) {}
};

// Validation rule
struct ValidationRule {
    std::string key;
    std::string type; // "range", "regex", "custom", "enum"
    std::string rule;
    std::string error_message;
    
    ValidationRule(const std::string& k, const std::string& t, const std::string& r, 
                   const std::string& err = "")
        : key(k), type(t), rule(r), error_message(err) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::unordered_map<std::string, ConfigValue> defaults;
    std::vector<std::string> inheritance_chain;
    
    ConfigTemplate(const std::string& n) : name(n) {}
    ConfigTemplate(const std::string& n, const std::unordered_map<std::string, ConfigValue>& defs)
        : name(n), defaults(defs) {}
};

// Schema definition for JSON validation
struct SchemaDefinition {
    std::string type; // "object", "string", "number", "boolean", "array"
    std::string pattern;
    std::vector<std::string> required_fields;
    std::unordered_map<std::string, SchemaDefinition> properties;
    std::vector<SchemaDefinition> items;
    double minimum;
    double maximum;
    
    SchemaDefinition() : type(""), pattern(""), minimum(0.0), maximum(0.0) {}
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
    
    // JSON/YAML support
    static bool loadFromJSON(const std::string& filepath);
    static bool loadFromYAML(const std::string& filepath);
    static bool loadFromSimple(const std::string& filepath);
    static bool saveToJSON(const std::string& filepath) const;
    static bool saveToYAML(const std::string& filepath) const;
    static bool saveToSimple(const std::string& filepath) const;
    
    // Type conversion utilities
    static std::optional<int> convertToInt(const std::string& str);
    static std::optional<double> convertToDouble(const std::string& str);
    static std::optional<bool> convertToBool(const std::string& str);
    static std::optional<std::string> convertToString(const std::string& str);
    
    // Configuration validation
    static bool validateConfigEntry(const std::string& key, const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
