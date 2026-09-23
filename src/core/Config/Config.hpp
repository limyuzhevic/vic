#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "AdvancedConfig.hpp"

namespace nlm {

// Forward declarations
class Config;

// Configuration value types (backward compatibility)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    float,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>,
    PlasticityFlags
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

// Main configuration class (backward compatibility)
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML or key=value format)
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
    
    /**
     * Get enhanced configuration manager (advanced features)
     */
    ConfigManager& getAdvancedConfig();
    
    /**
     * Get enhanced configuration manager (const version)
     */
    const ConfigManager& getAdvancedConfig() const;
    
    /**
     * Set enhanced configuration schema
     */
    void setAdvancedSchema(const ConfigSchema& schema);
    
    /**
     * Get enhanced configuration schema
     */
    const ConfigSchema& getAdvancedSchema() const;
    
    /**
     * Enable/disable validation
     */
    void setValidation(bool enabled);
    
    /**
     * Check if validation is enabled
     */
    bool isValidationEnabled() const;
    
    /**
     * Enable/disable environment variable expansion
     */
    void setEnvironmentExpansion(bool enabled);
    
    /**
     * Check if environment variable expansion is enabled
     */
    bool isEnvironmentExpansionEnabled() const;
    
    /**
     * Enable/disable type coercion
     */
    void setTypeCoercion(bool enabled);
    
    /**
     * Check if type coercion is enabled
     */
    bool isTypeCoercionEnabled() const;
    
    /**
     * Convert to enhanced configuration manager
     */
    explicit operator ConfigManager&();
    
    /**
     * Convert to enhanced configuration manager (const version)
     */
    explicit operator const ConfigManager&() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigManager> advancedConfig;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
