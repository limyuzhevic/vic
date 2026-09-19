#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// nlohmann/json for modern JSON parsing
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

// Type for validation functions
using ValidationFunc = std::function<bool(const ConfigValue&)>;

// Type for configuration schema
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "vector"
    ValidationFunc validator;
    std::string description;
    bool required;
    std::string defaultValue;
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
    
    // Load from file (supports JSON, YAML, and legacy key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSONFile(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
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
    
    // Get all keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration against schema
    bool validate(const std::vector<ConfigSchema>& schema) const;
    
    // Add validation rule for a key
    void addValidation(const std::string& key, const ValidationFunc& validator);
    
    // Check if key has a specific source
    bool hasSource(const std::string& key, ConfigSource source) const;
    
    // Compare two configurations
    bool equals(const Config& other) const;
    
    // Get configuration diff
    std::string getDiff(const Config& other) const;
    
    // Export configuration as JSON
    std::string toJSON() const;
    
    // Export configuration as YAML
    std::string toYAML() const;
    
    // Export configuration as INI
    std::string toINI() const;
    
    // Import configuration from JSON
    bool fromJSON(const std::string& jsonStr);
    
    // Import configuration from YAML
    bool fromYAML(const std::string& yamlStr);
    
    // Import configuration from INI
    bool fromINI(const std::string& iniStr);
    
    // Get all configuration entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Set description for a key
    void setDescription(const std::string& key, const std::string& description);
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Command line override methods
    void overrideFromArgs(int argc, char** argv);
    bool parseCommandLineArg(const std::string& arg);
    
    // Configuration validation with neural-specific schemas
    bool validateNeuralConfig(const std::vector<ConfigSchema>& schema) const;
    
    // Create configuration templates
    static Config createStandardTemplate();
    static Config createResearchTemplate();
    static Config createPerformanceTemplate();
    
    // Interactive configuration editor
    void startInteractiveEditor();
    
    // Configuration backup and versioning
    bool saveBackup(const std::string& filepath) const;
    bool restoreBackup(const std::string& filepath);
    bool createVersion(const std::string& comment = "");
    
    // Neural-specific configuration methods
    void applyNeuralDefaults();
    bool validateNeuralConstraints() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON parsing helpers
    void parseJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source);
    ConfigValue jsonToConfigValue(const nlohmann::json& value);
    std::string configValueToString(const ConfigValue& value) const;
    
    // Validation helpers
    bool validateValue(const ConfigEntry& entry, const ValidationFunc& validator) const;
    
    // Comparison helpers
    static bool compareValues(const ConfigValue& a, const ConfigValue& b);
    
    // Error handling helpers
    void logError(const std::string& message) const;
    void logWarning(const std::string& message) const;
    void logInfo(const std::string& message) const;
    
    // JSON conversion helpers
    nlohmann::json configValueToJSON(const ConfigValue& value) const;
    ConfigValue jsonToConfigValue(const nlohmann::json& value);
}; // namespace nlm
