#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs = {})
        : valid(v), errors(errs) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validation; // Validation rule
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validation() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& val = "", std::function<bool(const ConfigValue&)> validator = nullptr)
        : key(k), value(v), source(s), description(desc), validation(val), validator(validator) {}
};

// NLM-specific configuration presets
enum class NLMConfigPreset {
    Minimal,      // Basic functionality only
    Standard,     // Balanced configuration
    Optimized,    // Performance optimized
    Advanced,     // All advanced features enabled
    Custom       // User-defined
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
    
    // Load from string
    bool loadFromString(const std::string& configString);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& validation = "", std::function<bool(const ConfigValue&)> validator = nullptr);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& validation = "", std::function<bool(const ConfigValue&)> validator = nullptr);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& validation = "", std::function<bool(const ConfigValue&)> validator = nullptr);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& validation = "", std::function<bool(const ConfigValue&)> validator = nullptr);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& validation = "", std::function<bool(const ConfigValue&)> validator = nullptr);
    
    // NLM-specific configuration presets
    bool applyPreset(NLMConfigPreset preset);
    void addPreset(NLMConfigPreset preset, const Config& config);
    Config getPreset(NLMConfigPreset preset) const;
    
    // Configuration validation
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    
    // Advanced configuration options
    void enableFeature(const std::string& feature, bool enable = true);
    bool isFeatureEnabled(const std::string& feature) const;
    
    // Configuration introspection
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Get configuration summary
    std::string summary() const;
    std::string validationReport() const;
    
    // Clear all
    void clear();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // NLM default presets
    static Config createMinimalPreset();
    static Config createStandardPreset();
    static Config createOptimizedPreset();
    static Config createAdvancedPreset();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidType(const ConfigValue& value, const std::string& typeSpec);
    
    // Validation helpers
    static bool validateIntRange(const ConfigValue& value, const std::string& validation);
    static bool validateDoubleRange(const ConfigValue& value, const std::string& validation);
    static bool validateBool(const ConfigValue& value, const std::string& validation);
    static bool validateStringLength(const ConfigValue& value, const std::string& validation);
    static bool validateKeyValuePair(const ConfigValue& value, const std::string& validation);
};

} // namespace nlm
