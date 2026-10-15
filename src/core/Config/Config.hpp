#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigManager;
class ConfigTemplate;
class ConfigHistory;

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
    Template
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

// Validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::vector<std::string>> parameterErrors;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addParameterError(const std::string& param, const std::string& error) {
        valid = false;
        parameterErrors[param].push_back(error);
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::function<bool(const ConfigValue&, const Config&)>> validators;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::pair<double, double>> ranges;
    std::map<std::string, std::set<std::string>> enumValues;
    bool required = false;
    std::string format;
    
    ConfigSchema(const std::string& n = "", const std::string& desc = "")
        : name(n), description(desc) {}
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
    
    // Validation support
    bool validateWith(const ConfigValidator& validator) const;
    ValidationResult validateWithDetails(const ConfigValidator& validator) const;
    
    // Integration support
    bool applyTemplate(const ConfigTemplate& template);
    std::shared_ptr<Config> clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getCurrentTimestamp();
};

// Factory functions for configuration management
extern std::unique_ptr<ConfigValidator> createDefaultConfigValidator();
extern std::unique_ptr<ConfigManager> createDefaultConfigManager();
extern std::unique_ptr<ConfigTemplate> createConfigTemplate(const std::string& name);
extern std::unique_ptr<ConfigHistory> createConfigHistory();

// Convenience functions
std::shared_ptr<Config> loadConfig(const std::string& filepath);
bool saveConfig(const std::string& filepath, const std::shared_ptr<Config>& config);
ValidationResult validateConfig(const std::shared_ptr<Config>& config);
std::shared_ptr<Config> createConfigFromTemplate(const std::string& templateName);
std::map<std::string, std::pair<std::string, std::string>> compareConfigs(
    const std::string& config1, const std::string& config2);

} // namespace nlm


} // namespace nlm
