#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>
#include <any>

namespace nlm {

// Forward declarations
class Config;

// Advanced configuration value types supporting nested structures
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<ConfigValue>, // Nested arrays
    std::unordered_map<std::string, ConfigValue>, // Nested objects
    std::map<std::string, ConfigValue>, // Sorted nested objects
    std::any
>;

// Configuration source with enhanced tracking
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Override,      // User override in code
    Profile,       // Learning profile preset
    Template,      // Configuration template application
    Inherited      // Inherited from parent configuration
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;      // e.g., "learning", "performance", "memory"
    std::string typeName;      // String representation of type
    bool isAdvanced = false;   // Hidden from beginner view
    bool isExperimental = false; // Marked as experimental
    std::string validationRules; // JSON schema for validation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   category("general"), typeName("any"), isAdvanced(false), 
                   isExperimental(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
               const std::string& desc = "", const std::string& cat = "general")
        : key(k), value(v), source(s), description(desc), category(cat), 
          typeName("any"), isAdvanced(false), isExperimental(false) {}
    
    // Helper to extract nested values using dot notation
    template<typename T>
    T getNestedValue(const std::string& fullKey) const {
        size_t pos = fullKey.find('.');
        if (pos == std::string::npos) {
            // Direct key access
            try {
                return std::get<T>(value);
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Type mismatch for key: " + key);
            }
        } else {
            // Nested access - would need to be implemented in Config class
            throw std::runtime_error("Nested access not yet implemented for individual entries");
        }
    }
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "object", "array"
    bool required = false;
    ConfigValue defaultValue;
    std::string description;
    std::string category;
    bool isAdvanced = false;
    std::string validationPattern; // Regex pattern for strings
    std::map<std::string, std::string> constraints; // min, max, etc.
};

// Configuration profile/type for advanced presets
struct ConfigProfile {
    std::string name;
    std::string description;
    std::string category; // learning, performance, research, production
    std::map<std::string, ConfigValue> settings;
    bool isExperimental = false;
    double estimatedMemoryMB = 0.0;
    double estimatedPerformanceScore = 0.0;
};

// Main configuration class with enhanced features
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
    bool loadFromFile(const std::string& filepath, bool validateSchema = true);
    
    // Load from JSON/YAML strings
    bool loadFromJSON(const std::string& jsonContent, bool validateSchema = true);
    bool loadFromYAML(const std::string& yamlContent, bool validateSchema = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file in various formats
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    std::string saveToJSON() const;
    std::string saveToYAML() const;
    
    // Get values with enhanced key support (dot notation for nested keys)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Advanced key-based access
    std::optional<ConfigValue> getRaw(const std::string& key) const;
    std::vector<std::string> getKeys(const std::string& prefix = "") const;
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    std::vector<std::string> getAdvancedKeys() const;
    
    // Set values with enhanced features
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "general",
             bool isAdvanced = false);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "general",
             bool isAdvanced = false);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "general",
             bool isAdvanced = false);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "general",
             bool isAdvanced = false);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& category = "general",
             bool isAdvanced = false);
    
    // Nested key support
    void setNested(const std::string& fullKey, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    std::optional<ConfigValue> getNested(const std::string& fullKey) const;
    
    // Configuration inheritance and merging
    void inheritFrom(const Config& parentConfig, bool override = true);
    void merge(const Config& otherConfig, ConfigSource source = ConfigSource::Profile);
    
    // Check existence and type
    bool has(const std::string& key) const;
    std::string getTypeName(const std::string& key) const;
    std::string getCategory(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(const std::string& category) const;
    
    // Schema validation
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    bool validateConstraints() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Profile management
    void addProfile(const ConfigProfile& profile);
    ConfigProfile getProfile(const std::string& name) const;
    std::vector<std::string> getAvailableProfiles() const;
    bool applyProfile(const std::string& profileName, ConfigSource source = ConfigSource::Profile);
    
    // Performance analysis
    double estimateMemoryUsage() const;
    double estimatePerformanceScore() const;
    
    // Configuration history for versioning
    void checkpoint(const std::string& label);
    bool rollback(const std::string& label);
    std::vector<std::string> getCheckpointLabels() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNestedKey(const std::string& key);
    static std::string getParentKey(const std::string& key);
    static std::string getChildKey(const std::string& parentKey, const std::string& childKey);
};

// Template implementations

inline Config::Config() : pImpl(std::make_unique<Impl>()) {}
inline Config::~Config() = default;
inline Config::Config(Config&&) noexcept = default;
inline Config& Config::operator=(Config&&) noexcept = default;

} // namespace nlm
