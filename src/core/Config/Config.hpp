#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration validation and type checking
class ConfigValidator {
public:
    using ValidationCallback = std::function<bool(const std::string&, const ConfigValue&, const std::string&)>;
    
    struct ValidationRule {
        std::string key;
        ValidationCallback callback;
        std::string errorMessage;
    };
    
    // Add validation rule
    void addValidationRule(const std::string& key, ValidationCallback callback, const std::string& errorMessage);
    
    // Validate entire configuration
    std::vector<std::string> validate(const Config& config) const;
    
    // Clear all validation rules
    void clearValidationRules();
    
    // Set required keys
    void setRequiredKeys(const std::unordered_set<std::string>& requiredKeys);
    
    // Get required keys
    const std::unordered_set<std::string>& getRequiredKeys() const;
    
    // Set type validation
    void enableTypeValidation(bool enable);
    
    // Check if type validation is enabled
    bool isTypeValidationEnabled() const;
    
    // Add custom validation for specific key ranges/types
    void addRangeValidation(const std::string& key, double minValue, double maxValue, const std::string& errorMessage);
    void addStringLengthValidation(const std::string& key, size_t minLength, size_t maxLength, const std::string& errorMessage);
    void addEnumValidation(const std::string& key, const std::vector<std::string>& allowedValues, const std::string& errorMessage);
};

// Create global validator instance
// Configuration validation and type checking
class ConfigValidator {
public:
    using ValidationCallback = std::function<bool(const std::string&, const ConfigValue&, const std::string&)>;
    
    struct ValidationRule {
        std::string key;
        ValidationCallback callback;
        std::string errorMessage;
    };
    
    // Add validation rule
    void addValidationRule(const std::string& key, ValidationCallback callback, const std::string& errorMessage);
    
    // Validate entire configuration
    std::vector<std::string> validate(const Config& config) const;
    
    // Clear all validation rules
    void clearValidationRules();
    
    // Set required keys
    void setRequiredKeys(const std::unordered_set<std::string>& requiredKeys);
    
    // Get required keys
    const std::unordered_set<std::string>& getRequiredKeys() const;
    
    // Set type validation
    void enableTypeValidation(bool enable);
    
    // Check if type validation is enabled
    bool isTypeValidationEnabled() const;
    
    // Add custom validation for specific key ranges/types
    void addRangeValidation(const std::string& key, double minValue, double maxValue, const std::string& errorMessage);
    void addStringLengthValidation(const std::string& key, size_t minLength, size_t maxLength, const std::string& errorMessage);
    void addEnumValidation(const std::string& key, const std::vector<std::string>& allowedValues, const std::string& errorMessage);
};

// Configuration inheritance and template system
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::string baseConfigPath;
    std::unordered_map<std<string, std::string>> defaultOverrides;
    std::vector<std::string> validationRules;
    bool isSystemTemplate;
    
    ConfigTemplate(const std::string& n, const std::string& desc, const std::string& basePath = "")
        : name(n), description(desc), baseConfigPath(basePath), isSystemTemplate(false) {}
};

struct ValidationSchema {
    std::string name;
    std::string description;
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> typeValidators;
    std::unordered_map<std::string, std::pair<double, double>> rangeValidators;
    std::unordered_map<std::string, std::pair<size_t, size_t>> lengthValidators;
    std::unordered_map<std::string, std::vector<std::string>> enumValidators;
    std::vector<std::string> customValidators;
};

class ConfigManager {
private:
    std::vector<ConfigTemplate> templates_;
    std::vector<ValidationSchema> validationSchemas_;
    std::string userConfigDir_;
    
public:
    ConfigManager();
    ~ConfigManager();
    
    // Template management
    void addTemplate(const ConfigTemplate& template);
    bool removeTemplate(const std::string& name);
    std::vector<ConfigTemplate> getAllTemplates() const;
    bool saveTemplate(const ConfigTemplate& template);
    std::optional<ConfigTemplate> loadTemplate(const std::string& name);
    
    // Validation schema management
    void addValidationSchema(const ValidationSchema& schema);
    bool removeValidationSchema(const std::string& name);
    std::vector<ValidationSchema> getAllValidationSchemas() const;
    bool saveValidationSchema(const ValidationSchema& schema);
    std::optional<ValidationSchema> loadValidationSchema(const std::string& name);
    
    // Configuration management
    bool saveUserConfig(const Config& config, const std::string& profileName = "default");
    std::optional<Config> loadUserConfig(const std::string& profileName = "default");
    std::vector<std::string> getAvailableProfiles() const;
    
    // Configuration inheritance
    bool applyInheritance(Config& config, const std::string& profileName = "default");
    std::vector<std::string> getInheritedConfigFiles(const std::string& profileName = "default") const;
};

// Create global validator instance
ConfigValidator& getConfigValidator();

// Convenience function for validation
bool validateConfig(const Config& config);
std::vector<std::string> getConfigValidationErrors(const Config& config);

// Configuration manager functions
ConfigManager& getConfigManager();

// Convenience functions for advanced configuration
bool saveUserProfile(const Config& config, const std::string& profileName = "default");
std::optional<Config> loadUserProfile(const std::string& profileName = "default");
std::vector<std::string> getAvailableProfiles();

// Legacy compatibility
static bool validate(const Config& config) {
    return validateConfig(config);
}

static std::vector<std::string> validateWithErrors(const Config& config) {
    return getConfigValidationErrors(config);
}

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
    
    // Validation methods
    bool validate() const;
    std::vector<std::string> validateWithErrors() const;
    
    // Advanced features
    bool saveProfile(const std::string& profileName = "default") const;
    bool loadProfile(const std::string& profileName = "default");
    bool applyTemplate(const std::string& templateName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
