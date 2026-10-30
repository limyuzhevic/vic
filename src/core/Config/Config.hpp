#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;
class ConfigValidator;

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

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    std::string code;
    
    ValidationError() : key(), message(), code() {}
    ValidationError(const std::string& k, const std::string& msg, const std::string& c)
        : key(k), message(msg), code(c) {}
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

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    std::map<std::string, std::string> constraints;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigSchema() : key(), type("string"), description(), required(false), defaultValue(), constraints(), validator(nullptr) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc, bool req = false, const ConfigValue& def = ConfigValue())
        : key(k), type(t), description(desc), required(req), defaultValue(def), validator(nullptr) {}
};

// Configuration validator
class ConfigValidator {
public:
    static std::vector<ValidationError> validate(const Config& config, const ConfigSchema& schema);
    static std::vector<ValidationError> validateAll(const Config& config, const std::vector<ConfigSchema>& schemas);
    static bool validateValue(const std::string& key, const ConfigValue& value, const std::string& type, const std::map<std::string, std::string>& constraints);
    static std::vector<std::string> getAvailableTypes();
    static std::string valueToString(const ConfigValue& value);
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
    
    // Load from file (INI format)
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
    
    // Validation methods
    std::vector<ValidationError> validate(const std::vector<ConfigSchema>& schemas) const;
    std::vector<ValidationError> validate(const std::string& schemaName) const;
    bool isValid(const std::vector<ConfigSchema>& schemas) const;
    bool isValid(const std::string& schemaName) const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void addSchema(const std::string& name, const ConfigSchema& schema);
    bool hasSchema(const std::string& name) const;
    ConfigSchema getSchema(const std::string& name) const;
    std::vector<std::string> getSchemaNames() const;
    
    // Preset management
    void addPreset(const std::string& name, const Config& presetConfig);
    bool hasPreset(const std::string& name) const;
    Config getPreset(const std::string& name) const;
    std::vector<std::string> getPresetNames() const;
    
    // Utility methods
    std::string getValueType(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static std::string normalizeType(const std::string& type);
};

// Predefined configuration schemas
namespace ConfigSchemas {
    extern const std::vector<ConfigSchema> Phase2Defaults;
    extern const std::vector<ConfigSchema> Phase6Defaults;
    extern const std::vector<ConfigSchema> LearningPresets;
    extern const std::vector<ConfigSchema> DevelopmentPresets;
    extern const std::vector<ConfigSchema> StructuralPresets;
}

// Helper functions
namespace ConfigUtils {
    extern Config createDefaultConfig();
    extern Config createLearningConfig();
    extern Config createDevelopmentConfig();
    extern Config createStructuralConfig();
    extern Config applyPreset(const Config& baseConfig, const std::string& presetName);
    extern std::vector<ValidationError> validateConfig(const Config& config, const std::string& presetName = "default");
}

} // namespace nlm
