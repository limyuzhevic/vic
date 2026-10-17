#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>
#include <chrono>
#include <regex>
#include <set>
#include <filesystem>
#include <ctime>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigSchema;
class ConfigPresets;
class ConfigTemplates;

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
    Environment,
    CommandLine,
    Runtime,
    Preset,
    Template
};

// Configuration validation error types
enum class ConfigErrorType {
    None,
    InvalidType,
    OutOfRange,
    EmptyValue,
    InvalidFormat,
    MissingRequired,
    ConstraintViolation,
    FileError,
    EnvironmentError,
    ValidationError,
    SchemaError,
    PresetError,
    TemplateError
};

// Configuration data structure for inheritance
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::optional<double> min;
    std::optional<double> max;
    std::optional<std::string> pattern;
    std::optional<std::vector<std::string>> choices;
    std::vector<std::string> dependsOn;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), 
                   min(), max(), pattern(), choices(), dependsOn() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& t = "auto", const std::optional<double>& mn = std::nullopt,
                const std::optional<double>& mx = std::nullopt,
                const std::optional<std::string>& pat = std::nullopt,
                const std::optional<std::vector<std::string>>& ch = std::nullopt,
                const std::vector<std::string>& dep = std::vector<std::string>())
        : key(k), value(v), source(s), description(desc), type(t), min(mn), max(mx), 
          pattern(pat), choices(ch), dependsOn(dep) {}
};

// Configuration schema entry
struct SchemaEntry {
    std::string key;
    std::string type;
    bool required;
    ConfigValue defaultValue;
    std::optional<double> min;
    std::optional<double> max;
    std::optional<std::string> pattern;
    std::optional<std::vector<std::string>> choices;
    std::string description;
    std::vector<std::string> dependsOn;
    
    SchemaEntry() : key(), type("auto"), required(false), min(), max(), pattern(), choices(), 
                   description(), dependsOn() {}
    SchemaEntry(const std::string& k, const std::string& t, bool req = false,
                const ConfigValue& def = ConfigValue(),
                const std::optional<double>& mn = std::nullopt,
                const std::optional<double>& mx = std::nullopt,
                const std::optional<std::string>& pat = std::nullopt,
                const std::optional<std::vector<std::string>>& ch = std::nullopt,
                const std::string& desc = "",
                const std::vector<std::string>& dep = std::vector<std::string>())
        : key(k), type(t), required(req), defaultValue(def), min(mn), max(mx), 
          pattern(pat), choices(ch), description(desc), dependsOn(dep) {}
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string description;
    std::unordered_map<std::string, ConfigValue> values;
    std::unordered_map<std::string, std::string> overrides;
    
    ConfigPreset() : name(), description() {}
    ConfigPreset(const std::string& n, const std::string& desc)
        : name(n), description(desc) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::vector<SchemaEntry> schema;
    std::unordered_map<std::string, std::string> defaultOverrides;
    std::vector<std::string> tags;
    
    ConfigTemplate() : name(), description() {}
    ConfigTemplate(const std::string& n, const std::string& desc)
        : name(n), description(desc) {}
};

// Configuration comparison result
struct ConfigDiff {
    std::string key;
    std::optional<ConfigValue> oldValue;
    std::optional<ConfigValue> newValue;
    ConfigSource oldSource;
    ConfigSource newSource;
    bool isAdded;
    bool isRemoved;
    bool isModified;
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
    
    // Load from file (JSON or .cfg format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from configuration preset
    bool loadFromPreset(const std::string& presetName);
    
    // Load from configuration template
    bool loadFromTemplate(const std::string& templateName, const std::unordered_map<std::string, std::string>& overrides);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Validate configuration against schema
    bool validate(const ConfigSchema& schema) const;
    
    // Get values with validation
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
    
    // Set value from schema
    bool setFromSchema(const SchemaEntry& entry);
    
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
    
    // Get configuration with source precedence
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Compare two configurations
    std::vector<ConfigDiff> compare(const Config& other) const;
    
    // Export configuration to JSON
    std::string toJSON() const;
    
    // Import configuration from JSON
    bool fromJSON(const std::string& json);
    
    // Get configuration value type
    std::string getValueType(const std::string& key) const;
    
    // Check if value matches pattern
    bool validatePattern(const std::string& key, const std::string& value) const;
    
    // Check if value is in valid range
    bool validateRange(const std::string& key, double value) const;
    
    // Check if value is in valid choices
    bool validateChoices(const std::string& key, const std::string& value) const;
    
    // Get dependencies for a key
    std::vector<std::string> getDependencies(const std::string& key) const;
    
    // Resolve dependencies and validate
    bool validateDependencies() const;
    
    // Get configuration statistics
    struct Stats {
        size_t totalEntries;
        size_t fileEntries;
        size_t envEntries;
        size_t cliEntries;
        size_t runtimeEntries;
        size_t presetEntries;
        size_t templateEntries;
    };
    
    Stats getStats() const;
    
    // Set validation options
    void setValidationEnabled(bool enabled);
    void setSchemaValidationEnabled(bool enabled);
    void setDependencyValidationEnabled(bool enabled);
    
    // Configuration presets and templates
    static std::vector<std::string> getAvailablePresets();
    static std::vector<std::string> getAvailableTemplates();
    static std::vector<ConfigPreset> getAllPresets();
    static std::vector<ConfigTemplate> getAllTemplates();
    
    // Configuration schema management
    static bool addSchema(const SchemaEntry& entry);
    static bool removeSchema(const std::string& key);
    static std::vector<SchemaEntry> getSchema();
    static bool loadSchemaFromFile(const std::string& filepath);
    static bool saveSchemaToFile(const std::string& filepath) const;
    
    // Configuration export/import
    bool exportConfig(const std::string& filepath, const std::string& format = "cfg") const;
    bool importConfig(const std::string& filepath, const std::string& format = "cfg");
    
    // Configuration validation
    static bool validateConfigValue(const std::string& key, const ConfigValue& value, const SchemaEntry& schema = SchemaEntry());
    static bool validateConfigValue(const std::string& key, const std::string& value, const SchemaEntry& schema = SchemaEntry());
    static std::string getValidationError(const std::string& key, const ConfigValue& value, const SchemaEntry& schema = SchemaEntry()) const;
    
    // Command line subcommands
    static bool runPhase6(const Config& config, int argc, char** argv);
    static bool validateConfig(const Config& config);
    static bool exportConfig(const std::string& configFile, const std::string& outputFile, const std::string& format = "cfg");
    static bool info(const Config& config);
    static bool compareConfigs(const std::string& file1, const std::string& file2);
    static bool listPresets();
    static bool listTemplates();
    static bool help(const std::string& command = "");
    
    // Configuration management
    static void initializeDefaults();
    static std::string getDefaultConfigFile();
    static std::string getDefaultPreset();
    static std::string getDefaultTemplate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    static std::string valueToString(const ConfigValue& value) const;
    static ConfigValue stringToValue(const std::string& str, const std::string& type = "auto") const;
    
    // Configuration precedence management
    static std::map<ConfigSource, std::unordered_map<std::string, ConfigEntry>> m_sources;
    static std::mutex m_mutex;
    
    // Configuration inheritance
    static std::unordered_map<std::string, std::vector<std::string>> m_dependencies;
    static std::unordered_map<std::string, std::vector<SchemaEntry>> m_inheritance;
    
    // Configuration validation
    static std::vector<SchemaEntry> m_schema;
    static bool m_validationEnabled;
    static bool m_schemaValidationEnabled;
    static bool m_dependencyValidationEnabled;
    
    // Configuration presets and templates
    static std::unordered_map<std::string, ConfigPreset> m_presets;
    static std::unordered_map<std::string, ConfigTemplate> m_templates;
};

} // namespace nlm
