#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
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

// Configuration validation error
// Thrown when configuration values fail validation against schema
// Includes the key that caused validation failure for easy debugging
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error("Config validation error" + (key.empty() ? "" : " at key \"" + key + "\"") + ": " + message),
          key_(key) {}
    
    // Get the configuration key that caused the validation error
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

// Configuration entry with source tracking and description
// Stores configuration key-value pairs with metadata about origin and validity
// Supports typed configuration values through variant type system
struct ConfigEntry {
    std::string key;                     // Configuration key (e.g., "neuron_count", "simulation_timestep")
    ConfigValue value;                   // Typed configuration value
    ConfigSource source;                 // Origin of this configuration (Default, File, CommandLine, Runtime)
    std::string description;             // Human-readable description for documentation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Create configuration entry with all parameters
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration schema for validation and documentation
// Defines expected configuration keys, their types, constraints, and descriptions
// Enables validation of configuration values and provides documentation
struct ConfigSchema {
    std::string key;                                     // Configuration key name
    std::vector<std::function<bool(const ConfigValue&)>> validators;  // Validation functions
    std::string description;                            // Human-readable description
    bool required;                                       // Whether key must be present
    
    // Create configuration schema with basic parameters
    ConfigSchema(const std::string& k, bool req = false, const std::string& desc = "")
        : key(k), validators(), description(desc), required(req) {}
    
    // Add validation function to the schema
    // Each validator returns true if the value is valid for this key
    void addValidator(std::function<bool(const ConfigValue&)> validator) {
        validators.push_back(validator);
    }
};

// Parser result for mixed format support
struct ParseResult {
    bool success;
    std::string errorMessage;
    std::string format; // "key=value", "json", "yaml"
    
    static ParseResult success(const std::string& format) {
        return {true, "", format};
    }
    
    static ParseResult error(const std::string& message, const std::string& format = "unknown") {
        return {false, message, format};
    }
};

// Main configuration class
// Provides centralized configuration management for NLM
// Supports multiple configuration sources (file, command line, runtime)
// Includes validation, persistence, and type-safe access methods
// Uses a key-value store with variant types for flexible configuration
class Config {
public:
    // Default constructor creates empty configuration
    Config();
    
    // Destructor with proper cleanup
    ~Config();
    
    // Disable copying for memory safety
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable move semantics for efficiency
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from JSON file
    // File should be in standard JSON format with key-value pairs
    // Returns true if file loaded successfully
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports both --key and --key=value formats
    // Parses argument strings into configuration key-value pairs
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Serializes current configuration to JSON format
    // Returns true if save completed successfully
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value with type safety
    // Returns optional containing value if key exists, otherwise empty
    // Template parameter T must match variant type (int, double, string, etc.)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    // Returns default value if key doesn't exist or type doesn't match
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value with source tracking
    // Source defaults to Runtime but can be set to File, CommandLine, or Default
    // Enables tracking of configuration origin for debugging
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration key existence check
    // Returns true if key is present in configuration
    bool has(const std::string& key) const;
    
    // Remove configuration key
    // Permanently removes key from configuration
    void remove(const std::string& key);
    
    // Get all configuration keys
    // Returns vector of all keys currently in configuration
    std::vector<std::string> getKeys() const;
    
    // Clear entire configuration
    // Removes all key-value pairs and resets to empty state
    void clear();
    
    // Get configuration summary for logging
    // Returns human-readable string with key-value pairs
    std::string summary() const;
    
    // Validation methods for configuration integrity
    // Returns true if all configured values pass validation
    bool validate() const;
    
    // Get validation error messages
    // Returns concatenated string of all validation errors encountered
    std::string getValidationErrors() const;
    
    // Add validation schema for configuration keys
    // Schema defines expected types, constraints, and descriptions
    void addSchema(const ConfigSchema& schema);
    
    // Clear all validation schemas
    void clearSchemas();
    
    // Static parser utilities for configuration file parsing
    // Parse key=value format lines
    static ParseResult parseKeyValue(const std::string& line);
    
    // Parse JSON configuration files
    static ParseResult parseJSON(const std::string& content, Config& config);
    
    // Parse YAML configuration files
    static ParseResult parseYAML(const std::string& content, Config& config);
    
    // Detect configuration file format automatically
    static bool detectFileFormat(const std::string& filepath, std::string& format);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation schemas for configuration keys
    std::vector<ConfigSchema> schemas_;
    
    // Internal helper functions for configuration processing
    static std::string trim(const std::string& str);           // Remove whitespace from ends
    static std::string toLower(const std::string& str);        // Convert string to lowercase
    static std::optional<ConfigValue> parseToConfigValue(const std::string& valueStr);  // Convert string to variant type
    static std::string configValueToString(const ConfigValue& value) const;            // Convert variant type to string
    
    // Error handling and validation
    std::vector<std::string> validationErrors_;             // Accumulates validation errors
    
    void addValidationError(const std::string& error);       // Add error to validation queue
    void clearValidationErrors();                           // Clear all validation errors
    
    // File format detection and parsing
    bool loadFromFileFormat(const std::string& filepath, const std::string& format);
    bool loadFromJSON(const std::string& content);
    bool loadFromYAML(const std::string& content);
    bool loadFromKeyValue(const std::string& content);
    
    // Validation helpers
    bool validateValueAgainstSchema(const ConfigSchema& schema, const ConfigEntry& entry) const;
    bool isValueTypeMatch(const ConfigValue& value, const std::type_info& typeInfo) const;
};

} // namespace nlm
