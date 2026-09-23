#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <sstream>
#include <functional>
#include <unordered_map>

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

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrorMessage() const {
        if (errors.empty()) return "Configuration is valid.";
        std::ostringstream oss;
        oss << "Configuration errors:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
        return oss.str();
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration validation function type
using ConfigValidator = std::function<bool(const ConfigValue&, std::vector<std::string>&)>;

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
    
    // Load from string (JSON or INI format)
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key,
                                 const std::vector<std::string>& allowedValues = {}) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set default values (with validation)
    void setDefault(const std::string& key, const ConfigValue& value, const std::string& description = "");
    void setDefault(const std::string& key, const std::string& value, const std::string& description = "");
    void setDefault(const std::string& key, int value, const std::string& description = "");
    void setDefault(const std::string& key, double value, const std::string& description = "");
    void setDefault(const std::string& key, bool value, const std::string& description = "");
    
    // Set validator for a key
    void setValidator(const std::string& key, ConfigValidator validator);
    
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
    
    // Validate configuration
    ValidationResult validate() const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getValidationErrors(const std::string& key) const;
    
    // Check if configuration is consistent
    bool isConsistent() const;
    
    // Get configuration source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Set configuration source
    void setSource(const std::string& key, ConfigSource source);
    
    // Get configuration value as string
    std::string getAsString(const std::string& key) const;
    
    // Merge with another configuration
    void merge(const Config& other);
    
    // Reset to defaults
    void resetToDefaults();
    
    // Export to JSON
    std::string toJSON() const;
    
    // Import from JSON
    bool fromJSON(const std::string& json);
    
    // Get all configuration entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Load from environment variables
    void loadFromEnv(const std::string& prefix = "NLM_");
    
    // Get environment variable name for a key
    static std::string getEnvVarName(const std::string& key);
    
    // Set configuration description
    void setDescription(const std::string& key, const std::string& description);
    
    // Get configuration description
    std::string getDescription(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseINI(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseTOML(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& value);
    
    // Validation helpers
    static bool validateRange(const ConfigValue& value, double min, double max, std::vector<std::string>& errors);
    static bool validatePositive(const ConfigValue& value, std::vector<std::string>& errors);
    static bool validateNonNegative(const ConfigValue& value, std::vector<std::string>& errors);
    static bool validateStringNotEmpty(const ConfigValue& value, std::vector<std::string>& errors);
    static bool validateVectorSize(const ConfigValue& value, size_t maxSize, std::vector<std::string>& errors);
    static bool validateConsistent(const ConfigValue& value, const std::string& key, 
                                  const std::map<std::string, ConfigValue>& allValues, 
                                  std::vector<std::string>& errors);
};

} // namespace nlm
