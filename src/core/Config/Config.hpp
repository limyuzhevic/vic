#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <chrono>
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
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Preset
};

// Configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::string validationRegex;
    std::string validationRange;
    bool required = false;
    bool validated = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), type(), validationRegex(), validationRange(), required(false), validated(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& t = "string", const std::string& vregex = "",
                const std::string& vrange = "", bool req = false)
        : key(k), value(v), source(s), description(desc), type(t), 
          validationRegex(vregex), validationRange(vrange), required(req), validated(false) {}
};

// Configuration format
enum class ConfigFormat {
    Unknown,
    CFG,
    JSON,
    YAML
};

// Configuration presets
struct ConfigPreset {
    std::string name;
    std::map<std::string, std::string> values;
    std::string description;
    double priority; // For merging presets
    
    ConfigPreset(const std::string& n, const std::map<std::string, std::string>& v,
                 const std::string& desc = "", double prio = 1.0)
        : name(n), values(v), description(desc), priority(prio) {}
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
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from parameters map
    void loadFromParams(const std::map<std::string, std::string>& params);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation and type checking
    bool validate() const;
    bool validateKey(const std::string& key) const;
    std::string validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get type information for keys
    std::string getType(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Configuration presets
    static std::vector<ConfigPreset> getAvailablePresets();
    bool applyPreset(const std::string& presetName, ConfigSource source = ConfigSource::Preset);
    
    // Get file format from extension
    static ConfigFormat getFormatFromExtension(const std::string& filename);
    
    // Get configuration summary
    std::string summary() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration statistics
    std::string getStatistics() const;
    
    // Configuration file management
    static std::string getDefaultConfigPath();
    static std::vector<std::string> getSearchPaths();
    
    // Get/set global configuration directory
    static void setGlobalConfigDir(const std::string& dir);
    static std::string getGlobalConfigDir();
    
    // Configuration merge strategies
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    void mergeFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Get source of configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key is overridden
    bool isOverridden(const std::string& key) const;
    
    // Get precedence order for sources
    static std::vector<ConfigSource> getSourcePrecedence();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static std::string detectType(const std::string& value);
    static bool validateAgainstRegex(const std::string& value, const std::string& regex);
    static bool validateAgainstRange(const std::string& value, const std::string& range);
    static ConfigFormat detectFileFormat(const std::string& content);
    
    // JSON/YAML parsing helpers
    static std::map<std::string, std::string> parseCFGContent(const std::string& content);
    static std::map<std::string, std::string> parseJSONContent(const std::string& content);
    static std::map<std::string, std::string> parseYAMLContent(const std::string& content);
    
    // File format detection
    static ConfigFormat detectFileFormatByExtension(const std::string& filename);
    
    // Configuration validation rules
    static std::map<std::string, std::string> getDefaultValidationRules();
    static std::map<std::string, std::string> getDefaultTypes();
    static std::map<std::string, std::string> getDefaultDescriptions();
    
    // Global configuration directory
    static std::string globalConfigDir_;
    
    // Source precedence
    static std::vector<ConfigSource> sourcePrecedence_;
    
    // Available presets
    static std::vector<ConfigPreset> getDefaultPresets();
};

} // namespace nlm
