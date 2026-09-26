#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <chrono>
#include <any>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

// Enhanced configuration source with detailed options
enum class ConfigSource {
    Default = 0,           // Default configuration value
    File = 1,              // Loaded from configuration file
    CommandLine = 2,       // Provided via command line arguments
    Runtime = 3,           // Set at runtime via API
    Environment = 4,       // Loaded from environment variables
    Template = 5,          // Applied from configuration template
    HotReload = 6,         // Hot reloaded from file
    Validation = 7,        // Validated value
    Computed = 8           // Computed from other values
};

// Configuration entry with enhanced metadata and nested support
struct ConfigEntry {
    std::string key;
    std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>,
        std::any  // For nested/complex types
    > value;
    ConfigSource source;
    std::string description;
    std::string type;
    bool required;
    std::vector<std::string> dependsOn;
    std::chrono::system_clock::time_point timestamp;
    mutable bool validated;
    std::string validationError;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), type(""), 
          required(false), validated(false), validationError() {}
    
    ConfigEntry(const std::string& k, const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any>& v, 
                ConfigSource s, const std::string& desc = "", const std::string& t = "", bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), 
          required(req), dependsOn(), timestamp(std::chrono::system_clock::now()), 
          validated(false), validationError() {}
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
    
    // Enhanced file loading (JSON/YAML)
    bool loadFromFile(const std::string& filepath);
    bool loadFromFile(const std::string& filepath, ConfigSource source);
    
    // Load from JSON/YAML content strings
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from configuration template
    bool loadFromTemplate(const std::string& templateName);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced value getting with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe getters for specific types
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    
    // Enhanced value setting with validation
    bool set(const std::string& key, const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any>& value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration dependencies
    bool addDependency(const std::string& key, const std::string& dependsOnKey);
    
    // Configuration metadata and properties
    void setDescription(const std::string& key, const std::string& description);
    void setType(const std::string& key, const std::string& type);
    void setRequired(const std::string& key, bool required);
    
    // Check existence and get metadata
    bool has(const std::string& key) const;
    bool has(const std::string& key, ConfigSource source) const;
    
    // Get configuration metadata
    std::optional<std::string> getDescription(const std::string& key) const;
    std::optional<std::string> getType(const std::string& key) const;
    std::optional<bool> getRequired(const std::string& key) const;
    std::optional<ConfigSource> getSource(const std::string& key) const;
    
    // Configuration operations
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear and reload
    void clear();
    bool reload();
    
    // Configuration management
    void setHotReload(bool enabled, std::chrono::seconds interval = std::chrono::seconds(5));
    bool isHotReloadEnabled() const;
    std::string summary() const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Validation
    class ValidationError {
    public:
        std::string key;
        std::string error;
        ConfigSource source;
        
        ValidationError(const std::string& k, const std::string& e, ConfigSource s) : key(k), error(e), source(s) {}
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any> jsonToVariant(const nlohmann::json& jsonValue);
    static nlohmann::json variantToJson(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any>& value);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

} // namespace nlm

// Include nlohmann::json for JSON parsing
#include <nlohmann/json.hpp>
