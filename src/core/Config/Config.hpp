// Complete configuration management for NLM
// Advanced configuration with file support, validation, and merging capabilities

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <algorithm>
#include <functional>

namespace nlm {

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
class ConfigFileManager;

// Validation rule for configuration
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ValidationRule(const std::string& k, const std::function<bool(const ConfigValue&)>& v, const std::string& err)
        : key(k), validator(v), errorMessage(err) {}
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
    
    // Load from file (multiple formats supported)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with specific format
    bool loadFromFile(const std::string& filepath, const std::string& format);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from command line arguments with config prefix
    bool loadFromArgs(int argc, char** argv, const std::string& prefix);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with description
    void setWithDescription(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Get source of configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // Get description
    std::string getDescription(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration entries
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration file management
    void setFileManager(std::shared_ptr<ConfigFileManager> fileManager);
    
    // Merge with another configuration
    void merge(const Config& other, bool override = false);
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Get default configuration
    static Config getDefaultConfig();
    
    // Get environment variable configuration
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Convert to/from various formats
    static bool parseJSON(const std::string& jsonString, std::vector<ConfigEntry>& entries);
    std::string generateJSON() const;
    
    // Configuration validation rules
    using ValidationRule = nlm::ValidationRule;
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule);
    
    // Remove validation rule
    void removeValidationRule(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    
    // Type conversion helpers
    template<typename T>
    static std::optional<T> variantTo(const ConfigValue& variant);
    
    template<typename T>
    static ConfigValue toVariant(const T& value);
};

// Convenience functions
inline std::shared_ptr<Config> createConfig() {
    return std::make_shared<Config>();
}

inline std::shared_ptr<Config> createConfigFromFile(const std::string& filename) {
    auto config = createConfig();
    if (config->loadFromFile(filename)) {
        return config;
    }
    return nullptr;
}

inline bool validateConfig(const Config& config, const std::vector<std::string>& errors = {}) {
    std::vector<std::string> validationErrors = config.validate();
    return validationErrors.empty();
}

} // namespace nlm