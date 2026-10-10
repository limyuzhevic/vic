#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
// 
// These types define the supported value types in the configuration system.
// The variant type allows a single configuration entry to hold different
// kinds of data, providing flexibility for configuration management.
// 
// The choice of types reflects common configuration needs:
// - int, int64_t: Integer parameters (e.g., neuron counts, simulation steps)
// - double: Floating-point parameters (e.g., learning rates, time constants)
// - bool: Boolean flags (e.g., enable/disable features)
// - std::string: String parameters (e.g., file paths, names)
// - std::vector<T>: Array parameters for batch configuration

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
// 
// The configuration source tracks where configuration values originate from,
// enabling override semantics and debugging of configuration values. This is
// essential for reproducible experiments and understanding configuration hierarchies.

enum class ConfigSource {
    Default,      // Default value from configuration schema
    File,         // Loaded from configuration file
    CommandLine,  // Override from command line arguments
    Runtime       // Set at runtime during execution
};

// Configuration entry
// 
// Represents a single configuration entry with metadata. Each configuration
// key has a value, a source (indicating its origin), and a description for
// documentation purposes. This structure enables sophisticated configuration
// management with proper source tracking and documentation.

struct ConfigEntry {
    std::string key;           // Configuration key name
    ConfigValue value;         // Configuration value
    ConfigSource source;       // Source of this configuration value
    std::string description;   // Human-readable description
    
    // Default constructor for empty entries
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Constructor with all fields
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration schema entry
// 
// Defines the expected types and constraints for configuration parameters.
// This schema provides metadata about configuration parameters including
// their expected types, default values, and validation rules. This enables
// comprehensive validation and documentation generation.

struct ConfigSchemaEntry {
    std::string key;                    // Configuration key name
    std::string description;            // Human-readable description
    ConfigValue defaultValue;            // Default value if not specified
    bool required;                       // Whether this parameter is required
    std::function<bool(const std::string&, const ConfigValue&)> validate; // Validation function
    
    ConfigSchemaEntry()
        : key(), description(), defaultValue(), required(false), validate(nullptr) {}
    
    ConfigSchemaEntry(const std::string& k, const std::string& desc, const ConfigValue& def,
                     bool req = false, std::function<bool(const std::string&, const ConfigValue&)> val = nullptr)
        : key(k), description(desc), defaultValue(def), required(req), validate(val) {}
};

// Configuration validation function type
// 
// This type defines the signature for configuration validation functions.
// Validation functions take a configuration key and value, and return true
// if the value is valid, false otherwise. This enables flexible validation
// of configuration parameters with custom validation rules.

using ConfigValidationFunction = std::function<bool(const std::string&, const ConfigValue&)>;

// Main configuration class
// 
// The Config class provides a unified interface for managing simulation
// configuration with support for multiple sources (default, file, command line,
// runtime) and automatic merging of configuration values. This enables
// flexible configuration management while maintaining reproducibility.
// 
// Key features:
// - Type-safe access to configuration values
// - Multiple configuration sources with proper precedence
// - Serialization/deserialization for checkpointing
// - Runtime configuration modification
// - Configuration validation capabilities
// - Schema validation for configuration structure
// 
// The class uses the Pimpl idiom to hide implementation details and improve
// compilation times while maintaining high performance for runtime access.

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving for performance and memory safety
    // 
    // Copy operations are disabled because the Pimpl pattern requires
    // deep copy semantics. Move operations enable efficient transfer of ownership
    // while maintaining memory safety.
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    // 
    // Loads configuration from a JSON file. This provides a structured way to
    // store simulation parameters and allows for complex configuration schemas.
    // 
    // Args:
    //   filepath: Path to the JSON configuration file
    // 
    // Returns:
    //   true if configuration is loaded successfully, false if file cannot be
    //   read, is malformed, or contains invalid values
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    // 
    // Loads configuration from command line arguments, typically in key=value
    // format. This enables quick configuration changes without modifying
    // configuration files and is useful for experimental parameterization.
    // 
    // Args:
    //   argc: Number of command line arguments
    //   argv: Array of command line argument strings
    // 
    // Returns:
    //   true if command line arguments are parsed successfully, false otherwise
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    // 
    // Saves the current configuration to a JSON file for checkpointing,
    // sharing, or documentation purposes. This ensures configuration
    // reproducibility and enables configuration history tracking.
    // 
    // Args:
    //   filepath: Path to the output JSON file
    // 
    // Returns:
    //   true if configuration is saved successfully, false otherwise
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    // 
    // Provides type-safe access to configuration values. If the value cannot
    // be converted to the requested type, returns std::nullopt. This prevents
    // runtime errors from invalid type access and provides clear error handling.
    // 
    // Args:
    //   key: Configuration key to retrieve
    // 
    // Returns:
    //   Optional containing the value if found and type matches, std::nullopt
    //   otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get values with default fallback
    // 
    // Provides access to configuration values with a default value if the
    // key is not present. This is useful for optional configuration parameters
    // and maintaining backward compatibility.
    // 
    // Args:
    //   key: Configuration key to retrieve
    //   defaultValue: Default value to return if key is not found
    // 
    // Returns:
    //   Value from configuration or default if not found
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    // 
    // Set configuration values at runtime. This allows for dynamic
    // configuration changes during simulation, useful for adaptive experiments
    // or runtime tuning. This method validates the configuration before setting.
    // 
    // Args:
    //   key: Configuration key to set
    //   value: Configuration value to set
    //   source: Source of this configuration value
    // 
    // Returns:
    //   true if configuration value was set successfully, false otherwise
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    // 
    // Checks if a configuration key is present. This is useful for conditional
    // configuration loading and for checking if optional parameters are set.
    // 
    // Args:
    //   key: Configuration key to check
    // 
    // Returns:
    //   true if key exists, false otherwise
    bool has(const std::string& key) const;
    
    // Remove key
    // 
    // Removes a configuration key, reverting to default values. This is useful
    // for clearing runtime overrides or removing specific configuration
    // parameters.
    // 
    // Args:
    //   key: Configuration key to remove
    void remove(const std::string& key);
    
    // Get all keys
    // 
    // Returns a list of all configuration keys currently set. This is useful
    // for introspection, documentation generation, or validation.
    // 
    // Returns:
    //   Vector of all configuration key names
    std::vector<std::string> getKeys() const;
    
    // Clear all
    // 
    // Removes all configuration entries, reverting to the default state.
    // This is useful for starting fresh simulations or experimental trials.
    void clear();
    
    // Get configuration summary
    // 
    // Provides a human-readable summary of the current configuration for
    // logging, debugging, or documentation purposes. This includes all
    // configuration keys and their values with source information.
    // 
    // Returns:
    //   String containing configuration summary
    std::string summary() const;
    
    // Configuration validation
    // 
    // Validates the current configuration against schema and validation rules.
    // This method checks all configuration values and returns true if the
    // configuration is valid, false otherwise. Detailed error messages are
    // provided if validation fails.
    // 
    // Returns:
    //   true if configuration is valid, false otherwise
    bool validate() const;
    
    // Get validation error messages
    // 
    // Returns detailed error messages from the last validation attempt.
    // This is useful for debugging configuration issues and providing
    // user feedback about configuration problems.
    // 
    // Returns:
    //   String containing validation error messages
    std::string getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration schema
    std::vector<ConfigSchemaEntry> schema;
    
    // Validation error messages
    mutable std::vector<std::string> validationErrors;
    
    // Internal validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    static bool validatePositive(const std::string& key, const ConfigValue& value);
    static bool validateNonNegative(const std::string& key, const ConfigValue& value);
    static bool validateRange(const std::string& key, const ConfigValue& value, double min, double max);
    static bool validateNeuronCount(const std::string& key, const ConfigValue& value);
    static bool validateRegionCount(const std::string& key, const ConfigValue& value);
    static bool validateConnectionProbability(const std::string& key, const ConfigValue& value);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};