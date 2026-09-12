#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
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
    std::vector<std::string>,
    std::vector<std::vector<double>>  // Matrix support
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::map<std::string, std::string>& meta)
        : key(k), value(v), source(s), description(desc), metadata(meta) {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    
    bool isValid() const { return valid && errors.empty(); }
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t defaultEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    std::map<std::string, size_t> entriesByType;
    double memoryUsage; // In bytes
    
    ConfigStats() : totalEntries(0), defaultEntries(0), fileEntries(0), 
                   commandLineEntries(0), runtimeEntries(0), memoryUsage(0.0) {}
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
    
    // Load from file (JSON or YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars = {});
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Save to YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Save to JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> getWithType(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Advanced get methods
    std::optional<ConfigValue> getRaw(const std::string& key) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    std::vector<double> getVector(const std::string& key) const;
    std::vector<std::vector<double>> getMatrix(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::vector<double>>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get raw ConfigValue
    ConfigValue getRawValue(const std::string& key) const;
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool isBool(const std::string& key) const;
    bool isDouble(const std::string& key) const;
    bool isInt(const std::string& key) const;
    bool isString(const std::string& key) const;
    bool isVector(const std::string& key) const;
    bool isMatrix(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all or by source
    void clear();
    void clear(ConfigSource source);
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigSource source) const;
    
    // Validation
    ConfigValidationResult validate() const;
    bool validatePath(const std::string& key) const;
    
    // Statistics
    ConfigStats getStats() const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void overrideWith(const Config& other);
    
    // Batch operations
    void setBatch(const std::map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    std::map<std::string, ConfigValue> getBatch(const std::vector<std::string>& keys) const;
    
    // Environment variable support
    static std::map<std::string, std::string> parseEnvironment(const std::string& prefix = "NLM_");
    static std::string configKeyToEnvVar(const std::string& key);
    static std::string envVarToConfigKey(const std::string& envVar);
    
    // File operations with backup
    bool saveWithBackup(const std::string& filepath, const std::string& format = "json", bool createBackup = true);
    bool restoreFromBackup(const std::string& filepath);
    
    // Configuration templates
    static Config createDefaultConfig();
    static Config createDebugConfig();
    static Config createProductionConfig();
    
    // Incremental updates
    class Updater {
    public:
        Updater(Config& config) : config_(config) {}
        
        Updater& set(const std::string& key, const ConfigValue& value);
        Updater& set(const std::string& key, const std::string& value);
        Updater& set(const std::string& key, int value);
        Updater& set(const std::string& key, double value);
        Updater& set(const std::string& key, bool value);
        
        void apply(ConfigSource source = ConfigSource::Runtime);
        void applyToSource(ConfigSource source);
        
    private:
        Config& config_;
        std::map<std::string, ConfigValue> updates_;
    };
    
    Updater createUpdater();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    Config diff(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    static ConfigValue parseConfigValue(const std::string& str);
    
    // JSON conversion
    static void configToJson(const Config& config, nlohmann::json& json);
    static bool jsonToConfig(const nlohmann::json& json, Config& config);
    
    // Validation helpers
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value, const std::string& key);
    
    // Statistics calculation
    static ConfigStats calculateStats(const std::vector<ConfigEntry>& entries);
};

} // namespace nlm
