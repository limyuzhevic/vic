#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <any>
#include <mutex>
#include <map>
#include <unordered_map>
#include <ctime>

namespace nlm {

// Forward declarations
class ConfigurationBuilder;
class ConfigurationValidator;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Builder
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

// Configuration change notification
struct ConfigChange {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string timestamp;
    
    ConfigChange(const std::string& k, const ConfigValue& old, const ConfigValue& new, ConfigSource src)
        : key(k), oldValue(old), newValue(new), source(src) {
        // Generate timestamp
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        timestamp = buf;
    }
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const ConfigChange&)>;

// Validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string path;
    
    ValidationResult(bool v = true, const std::string& error = "", const std::string& path = "")
        : valid(v), errorMessage(error), path(path) {}
};

// Configuration interface
class Configuration {
public:
    virtual ~Configuration() = default;
    
    // Core configuration operations
    virtual void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) = 0;
    virtual std::optional<ConfigValue> get(const std::string& key) const = 0;
    virtual bool has(const std::string& key) const = 0;
    virtual void remove(const std::string& key) = 0;
    virtual std::vector<std::string> getKeys() const = 0;
    virtual void clear() = 0;
    
    // Type-safe accessors
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // File operations
    virtual bool loadFromJSON(const std::string& jsonStr) = 0;
    virtual bool loadFromYAML(const std::string& yamlStr) = 0;
    virtual bool loadFromFile(const std::string& filepath) = 0;
    virtual bool saveToJSON(std::string& output) const = 0;
    virtual bool saveToYAML(std::string& output) const = 0;
    
    // Validation
    virtual ValidationResult validate() const = 0;
    
    // Callbacks
    virtual void addChangeCallback(const ConfigChangeCallback& callback) = 0;
    virtual void removeChangeCallback(const ConfigChangeCallback& callback) = 0;
    
    // Statistics
    virtual size_t getEntryCount() const = 0;
    virtual std::string summary() const = 0;
    
    // Builder access
    virtual std::shared_ptr<ConfigurationBuilder> getBuilder() = 0;
    virtual std::shared_ptr<ConfigurationValidator> getValidator() = 0;
};

// Configuration builder interface
class ConfigurationBuilder {
public:
    virtual ~ConfigurationBuilder() = default;
    
    // Add configuration definition
    virtual ConfigurationBuilder& addInt(const std::string& key, int64_t defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addDouble(const std::string& key, double defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addFloat(const std::string& key, float defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addBool(const std::string& key, bool defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addString(const std::string& key, const std::string& defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addIntList(const std::string& key, const std::vector<int64_t>& defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addDoubleList(const std::string& key, const std::vector<double>& defaultValue, const std::string& description = "") = 0;
    virtual ConfigurationBuilder& addStringList(const std::string& key, const std::vector<std::string>& defaultValue, const std::string& description = "") = 0;
    
    // Validation rules
    virtual ConfigurationBuilder& addValidator(const std::string& key, std::function<ValidationResult(const ConfigValue&)> validator) = 0;
    virtual ConfigurationBuilder& addDependency(const std::string& key, const std::string& dependsOn) = 0;
    
    // Build configuration
    virtual std::shared_ptr<Configuration> build() = 0;
};

// Configuration validator interface
class ConfigurationValidator {
public:
    virtual ~ConfigurationValidator() = default;
    
    // Validate configuration
    virtual ValidationResult validate(const Configuration& config) const = 0;
    
    // Add validation rule
    virtual ConfigurationValidator& addRule(const std::string& key, std::function<ValidationResult(const ConfigValue&)> rule) = 0;
    virtual ConfigurationValidator& addCrossValidation(const std::string& key1, const std::string& key2, std::function<ValidationResult(const ConfigValue&, const ConfigValue&)> rule) = 0;
};

// Main Config class - backward compatible with existing codebase
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Core configuration operations (original API)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values (original API)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
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
    
    // Load from file (with JSON/YAML support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // New API methods
    bool loadFromJSON(const std::string& jsonStr);
    bool loadFromYAML(const std::string& yamlStr);
    bool saveToJSON(std::string& output) const;
    bool saveToYAML(std::string& output) const;
    
    // Configuration change notifications
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
    // Validation
    ValidationResult validate() const;
    
    // Builder and validator access
    std::shared_ptr<ConfigurationBuilder> getBuilder();
    std::shared_ptr<ConfigurationValidator> getValidator();
    
    // Type-safe access methods
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    
    template<typename T>
    T getOrTyped(const std::string& key, const T& defaultValue) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Configuration management utilities
namespace ConfigUtils {
    // Parse simple key=value format (original format)
    static std::map<std::string, std::string> parseSimpleFormat(const std::string& content);
    
    // Convert ConfigValue to string
    static std::string configValueToString(const ConfigValue& value);
    
    // Convert string to ConfigValue (basic type inference)
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str);
    
    // Generate JSON schema from configuration entries
    static std::string generateJSONSchema(const std::vector<ConfigEntry>& entries);
    
    // Generate YAML schema from configuration entries
    static std::string generateYAMLSchema(const std::vector<ConfigEntry>& entries);
}

// Factory functions
std::shared_ptr<Configuration> createAdvancedConfiguration();
std::shared_ptr<ConfigurationBuilder> createConfigurationBuilder();
std::shared_ptr<ConfigurationValidator> createConfigurationValidator();

} // namespace nlm

// Template implementations for type-safe accessors
namespace nlm {

template<typename T>
std::optional<T> Configuration::getTyped(const std::string& key) const {
    auto value = get(key);
    if (!value) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(*value) || std::holds_alternative<int>(*value)) {
                return std::get<int64_t>(*value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(*value) || std::holds_alternative<float>(*value)) {
                return std::get<double>(*value);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (std::holds_alternative<float>(*value)) {
                return std::get<float>(*value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(*value)) {
                return std::get<bool>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(*value)) {
                return std::get<std::string>(*value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            if (std::holds_alternative<std::vector<int64_t>>(*value) || std::holds_alternative<std::vector<int>>(*value)) {
                if (std::holds_alternative<std::vector<int64_t>>(*value)) {
                    return std::get<std::vector<int64_t>>(*value);
                } else {
                    std::vector<int64_t> result;
                    for (int val : std::get<std::vector<int>>(*value)) {
                        result.push_back(val);
                    }
                    return result;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(*value) || std::holds_alternative<std::vector<float>>(*value)) {
                if (std::holds_alternative<std::vector<double>>(*value)) {
                    return std::get<std::vector<double>>(*value);
                } else {
                    std::vector<double> result;
                    for (float val : std::get<std::vector<float>>(*value)) {
                        result.push_back(val);
                    }
                    return result;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(*value)) {
                return std::get<std::vector<std::string>>(*value);
            }
        }
    } catch (const std::bad_variant_access&) {
        // Type mismatch
    }
    
    return std::nullopt;
}

template<typename T>
T Configuration::getOr(const std::string& key, const T& defaultValue) const {
    auto value = getTyped<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

// Template implementations for Config class
template<typename T>
std::optional<T> Config::getTyped(const std::string& key) const {
    return pImpl->getTyped<T>(key);
}

template<typename T>
T Config::getOrTyped(const std::string& key, const T& defaultValue) const {
    return pImpl->getOrTyped<T>(key, defaultValue);
}

// Original template implementations (for backward compatibility)
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return pImpl->getTyped<T>(key);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->getOrTyped<T>(key, defaultValue);
}

} // namespace nlm
