#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

//============================================================================
// CONFIG SYSTEM - Enhanced Configuration Management
//============================================================================

// Forward declarations
class Config;

// Configuration value types supporting primitive types and collections
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

/**
 * @brief Configuration source indicating where the value originated
 * 
 * This enum tracks the precedence of configuration values:
 * Default < File < CommandLine < Runtime
 * Later sources can override earlier ones
 */
enum class ConfigSource {
    Default = 0,      ///< Built-in default values
    File = 1,         ///< Loaded from configuration file
    CommandLine = 2,  ///< Set via command line arguments
    Runtime = 3       ///< Set programmatically at runtime
};

/**
 * @brief Individual configuration entry with metadata
 * 
 * Represents a single key-value pair in the configuration system,
 * including source tracking and descriptive documentation for tooling.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key (e.g., "neuron_count")
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Origin of this configuration value
    std::string description;    ///< Human-readable description for documentation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Optional description for documentation
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Configuration validator interface for extensibility
 * 
 * Enables custom validation logic for specific configuration keys
 * and value types. Validators are executed in order and must all pass
 * for a configuration to be considered valid.
 */
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    /**
     * @brief Validate a configuration value
     * 
     * @param key Configuration key being validated
     * @param value Configuration value to validate
     * @return true if the value is valid, false otherwise
     */
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    
    /**
     * @brief Get error message for validation failure
     * 
     * @param key Configuration key that failed validation
     * @param value Invalid configuration value
     * @return Descriptive error message
     */
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

/**
 * @brief Type-safe configuration accessor with validation
 * 
 * Provides type-safe access to configuration values with automatic
 * validation through registered validators. This ensures that
 * configuration values meet application-specific requirements.
 */
class TypedConfigAccessor {
public:
    TypedConfigAccessor(const Config& config, const std::vector<std::shared_ptr<ConfigValidator>>& validators)
        : config_(config), validators_(validators) {}
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key
     * @return Optional configuration value (empty if not found or invalid)
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto value = config_.get<T>(key);
        if (!value) {
            return std::nullopt;
        }
        
        // Validate the value
        for (const auto& validator : validators_) {
            if (!validator->validate(key, value.value())) {
                NLM_LOG_WARNING(std::string("Config validation failed for key '" + key + "': " + 
                    validator->getErrorMessage(key, value.value())));
                return std::nullopt;
            }
        }
        
        return value;
    }
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found or invalid
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        return value.has_value() ? value.value() : defaultValue;
    }
    
private:
    const Config& config_;
    const std::vector<std::shared_ptr<ConfigValidator>>& validators_;
};

/**
 * @brief Factory for creating configuration objects
 * 
 * Provides factory methods for creating configuration objects from
 * different sources (JSON, files, command line, defaults) with consistent
 * initialization patterns.
 */
class ConfigFactory {
public:
    /**
     * @brief Create configuration from JSON string
     * 
     * @param jsonString JSON configuration string
     * @return Shared pointer to configuration object
     */
    static std::shared_ptr<Config> createFromJSON(const std::string& jsonString);
    
    /**
     * @brief Create configuration from file
     * 
     * @param filepath Path to configuration file
     * @return Shared pointer to configuration object
     */
    static std::shared_ptr<Config> createFromFile(const std::string& filepath);
    
    /**
     * @brief Create configuration from command line arguments
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return Shared pointer to configuration object
     */
    static std::shared_ptr<Config> createFromArgs(int argc, char** argv);
    
    /**
     * @brief Create configuration with default values
     * 
     * @return Shared pointer to default configuration object
     */
    static std::shared_ptr<Config> createDefault();
};

//============================================================================
// CONFIGURATION SYSTEM - Main Configuration Class
//============================================================================

/**
 * @brief Enhanced configuration management system with JSON/YAML support
 * 
 * This configuration system provides:
 * - JSON and YAML format support with automatic detection
 * - Type-safe value access with optional validation
 * - Source tracking for configuration precedence
 * - Comprehensive error handling and logging
 * - Serialization/deserialization capabilities
 * - Validation framework with custom validators
 * 
 * The system supports both simple key=value format (for backward compatibility)
 * and structured JSON/YAML formats (for complex configurations).
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving for safety
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Automatically detects file format based on extension:
     * .json, .yaml, .yml -> JSON/YAML format
     * .cfg, .txt, .ini, or no extension -> Simple key=value format
     * 
     * @param filepath Path to configuration file
     * @return true if configuration loaded successfully
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports both --key=value and -key value formats
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if configuration parsed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Automatically selects format based on file extension
     * 
     * @param filepath Path to save configuration
     * @return true if configuration saved successfully
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key
     * @return Optional configuration value (empty if not found or type mismatch)
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Create type-safe accessor with validation
     * 
     * @param validators Vector of validators for configuration values
     * @return Shared pointer to type-safe accessor
     */
    std::shared_ptr<TypedConfigAccessor> createAccessor(
        const std::vector<std::shared_ptr<ConfigValidator>>& validators) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value with convenience overloads
     * 
     * Overloads for common types (string, int, double, bool) with
     * automatic conversion to ConfigValue.
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all entries from configuration
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return Formatted string containing configuration information
     */
    std::string summary() const;
    
    /**
     * @brief Validate all configuration values
     * 
     * Checks all configuration values against registered validators
     * 
     * @return true if all values are valid
     */
    bool validate() const;
    
    /**
     * @brief Add validator for configuration values
     * 
     * @param validator Shared pointer to validator
     */
    void addValidator(const std::shared_ptr<ConfigValidator>& validator);
    
    /**
     * @brief Load configuration from JSON string
     * 
     * @param jsonString JSON configuration string
     * @return true if configuration loaded successfully
     */
    bool loadFromJSON(const std::string& jsonString);
    
    /**
     * @brief Import configuration from JSON string (alias for loadFromJSON)
     * 
     * @param jsonString JSON configuration string
     * @return true if configuration imported successfully
     */
    bool importFromJSON(const std::string& jsonString);
    
    /**
     * @brief Export configuration to JSON string
     * 
     * @return JSON string representation of configuration
     */
    std::string exportToJSON() const;
    
    /**
     * @brief Get configuration value as string
     * 
     * @param key Configuration key
     * @return String representation of configuration value
     */
    std::string getValueAsString(const std::string& key) const;
    
protected:
    /**
     * @brief Process JSON data and populate configuration
     * 
     * @param j JSON object
     * @param prefix Key prefix for nested structures
     * @param source Configuration source
     */
    void processJSON(const nlohmann::json& j, const std::string& prefix, ConfigSource source);
    
    /**
     * @brief Set JSON value from configuration value
     * 
     * @param j JSON object to modify
     * @param key JSON key
     * @param value Configuration value to convert
     */
    void setJSONValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Get raw configuration value
     * 
     * @param key Configuration key
     * @return Raw configuration value
     */
    ConfigValue getValue(const std::string& key) const;
    
    /**
     * @brief Validate individual configuration entry
     * 
     * @param entry Configuration entry to validate
     * @return true if entry is valid
     */
    bool validateEntry(const ConfigEntry& entry) const;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return Lowercase string
     */
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Pimpl pattern for implementation hiding
};

} // namespace nlm
