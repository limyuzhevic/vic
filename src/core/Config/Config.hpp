#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>
#include <stdexcept>

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Configuration template (inheritance)
struct ConfigTemplate {
    std::string name;
    std::vector<ConfigEntry> baseEntries;
    std::vector<ValidationRule> validationRules;
    bool inheritFromDefault;
};

// Preset profile configuration
struct PresetProfile {
    enum class ProfileType {
        Basic,
        Advanced,
        Research
    };
    
    std::string name;
    ProfileType type;
    std::vector<ConfigEntry> entries;
    std::string description;
    std::string useCase;
};

// File format
enum class ConfigFileFormat {
    PlainText,
    JSON,
    YAML
};

// Configuration options
struct ConfigOptions {
    bool validateOnSet = true;
    bool allowInheritance = true;
    ConfigFileFormat preferredFormat = ConfigFileFormat::JSON;
    bool mergeOnLoad = false;
    std::string defaultFileExtension = ".json";
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
    
    // Configuration loading/saving
    bool loadFromFile(const std::string& filepath, ConfigFileFormat format = ConfigFileFormat::PlainText);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    bool saveToFile(const std::string& filepath, ConfigFileFormat format = ConfigFileFormat::PlainText) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Preset profiles
    static std::vector<std::string> getAvailablePresets();
    static PresetProfile loadPreset(const std::string& presetName);
    bool applyPreset(const std::string& presetName);
    
    // Configuration inheritance
    bool loadFromTemplate(const ConfigTemplate& template);
    bool mergeWith(const Config& other, bool overwriteConflicting = false);
    
    // Validation
    void addValidationRule(const ValidationRule& rule);
    void clearValidationRules();
    void validate() const;
    bool isValid() const;
    
    // Set values with validation
    template<typename T>
    void setValidated(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration statistics
    size_t getEntryCount() const;
    ConfigSource getEntrySource(const std::string& key) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration options
    void setOptions(const ConfigOptions& options);
    ConfigOptions getOptions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSON(const std::string& json, std::vector<ConfigEntry>& entries);
    static bool parseYAML(const std::string& yaml, std::vector<ConfigEntry>& entries);
    static std::string configValueToString(const ConfigValue& value);
    static ConfigValue stringToConfigValue(const std::string& str);
};

} // namespace nlm
