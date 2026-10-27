// ============================================================================
// NLM Configuration System
// ============================================================================
//
// Enhanced configuration system for NLM (Neural Learning Machine) with support
// for multiple file formats (JSON, YAML, and plain text) and comprehensive
// validation. The system maintains backward compatibility with existing
// key=value format while adding modern JSON/YAML support.
//
// Key Features:
// - Multi-format support: JSON (.json), YAML (.yaml/.yml), and plain text (.cfg)
// - Backward compatibility: Existing key=value format still works
// - Configuration validation: Schema validation and type checking
// - Error handling: Detailed error messages with context
// - CLI integration: Enhanced command-line interface with help/version flags
// - Automatic backup: Optional backup creation during file saves
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

namespace nlm {

// ============================================================================
// Configuration Value Types
// ============================================================================
///
/// Configuration values can be of various types:
/// - Basic types: int, int64_t, double, bool, string
/// - Collection types: vector<int>, vector<double>, vector<string>
///
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

// ============================================================================
// Configuration File Format Enumeration
// ============================================================================
///
/// Supported configuration file formats
enum class ConfigFileFormat {
    PlainText, ///< Legacy key=value format (.cfg)
    JSON,      ///< JSON format (.json, .jsonc)
    YAML       ///< YAML format (.yaml, .yml)
};

// ============================================================================
// Configuration Source Enumeration
// ============================================================================
///
/// Tracks how configuration values were loaded
enum class ConfigSource {
    Default,     ///< Built-in default values
    File,        ///< Loaded from configuration file
    CommandLine, ///< Override from command line arguments
    Runtime      ///< Set programmatically at runtime
};

// ============================================================================
// Configuration Entry Structure
// ============================================================================
///
/// Represents a single configuration key-value pair with metadata
struct ConfigEntry {
    std::string key;      ///< Configuration key identifier
    ConfigValue value;    ///< Configuration value (variant type)
    ConfigSource source;  ///< How the value was loaded
    std::string description; ///< Human-readable description of the parameter
    
    // Constructors
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// ============================================================================
// Main Configuration Class
// ============================================================================
///
/// Central configuration management class for NLM.
/// Supports multiple input formats, validation, and persistent storage.
class Config {
public:
    // ============================================================================
    // Construction and Destruction
    // ============================================================================
    
    ///
    /// Default constructor
    Config();
    
    ///
    /// Virtual destructor
    ~Config();
    
    ///
    /// Copy constructor (deleted)
    Config(const Config&) = delete;
    
    ///
    /// Copy assignment operator (deleted)
    Config& operator=(const Config&) = delete;
    
    ///
    /// Move constructor
    Config(Config&&) noexcept;
    
    ///
    /// Move assignment operator
    Config& operator=(Config&&) noexcept;
    
    // ============================================================================
    // File Loading Methods
    // ============================================================================
    
    ///
    /// Load configuration from a file with automatic format detection.
    /// Supported formats: JSON (.json, .jsonc), YAML (.yaml, .yml), and plain text (.cfg).
    /// Maintains backward compatibility with existing key=value format.
    ///
    /// @param filepath Path to the configuration file
    /// @return true if file was loaded successfully, false otherwise
    bool loadFromFile(const std::string& filepath);
    
    ///
    /// Load configuration from plain text key=value format.
    /// Used internally for backward compatibility and YAML support.
    ///
    /// @param filepath Path to the plain text configuration file
    /// @return true if file was loaded successfully, false otherwise
    bool loadFromPlainText(const std::string& filepath);
    
    ///
    /// Load configuration from JSON format.
    /// Parses JSON key-value pairs and loads them into the configuration.
    ///
    /// @param filepath Path to the JSON configuration file
    /// @return true if file was loaded successfully, false otherwise
    bool loadFromJSON(const std::string& filepath);
    
    ///
    /// Load configuration from YAML format.
    /// Uses the same simple parser as plain text (YAML is similar to key: value).
    ///
    /// @param filepath Path to the YAML configuration file
    /// @return true if file was loaded successfully, false otherwise
    bool loadFromYAML(const std::string& filepath);
    
    // ============================================================================
    // Command Line Interface
    // ============================================================================
    
    ///
    /// Load configuration from command line arguments.
    /// Supports both --key=value and -key value formats.
    ///
    /// @param argc Number of command line arguments
    /// @param argv Array of command line argument strings
    /// @return true if arguments were processed successfully
    bool loadFromArgs(int argc, char** argv);
    
    // ============================================================================
    // File Saving Methods
    // ============================================================================
    
    ///
    /// Save configuration to a file with automatic format detection based on extension.
    /// Supported output formats: JSON (.json), YAML (.yaml), and plain text (.cfg).
    ///
    /// @param filepath Path where the configuration will be saved
    /// @return true if save was successful, false otherwise
    bool saveToFile(const std::string& filepath) const;
    
    ///
    /// Save configuration as plain text (key=value format).
    /// Most compatible format and preserves backward compatibility.
    ///
    /// @param filepath Path where the configuration will be saved
    /// @return true if save was successful, false otherwise
    bool saveAsPlainText(const std::string& filepath) const;
    
    ///
    /// Save configuration as JSON format.
    /// Produces compact, machine-readable configuration files.
    ///
    /// @param filepath Path where the configuration will be saved
    /// @return true if save was successful, false otherwise
    bool saveAsJSON(const std::string& filepath) const;
    
    ///
    /// Save configuration as YAML format.
    /// Produces human-readable configuration files similar to plain text.
    ///
    /// @param filepath Path where the configuration will be saved
    /// @return true if save was successful, false otherwise
    bool saveAsYAML(const std::string& filepath) const;
    
    ///
    /// Save configuration with automatic backup creation.
    /// Creates a timestamped backup before overwriting existing files.
    ///
    /// @param filepath Path where the configuration will be saved
    /// @return true if save was successful, false otherwise
    bool saveToFileWithBackup(const std::string& filepath) const;
    
    // ============================================================================
    // Value Access Methods
    // ============================================================================
    
    ///
    /// Get a configuration value by key with type safety.
    ///
    /// @tparam T Expected type (int, int64_t, double, bool, or std::string)
    /// @param key Configuration key to retrieve
    /// @return Optional containing the value if found, std::nullopt otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    ///
    /// Get a configuration value by key with default value.
    /// Returns the default if the key is not found or type doesn't match.
    ///
    /// @tparam T Expected type
    /// @param key Configuration key to retrieve
    /// @param defaultValue Default value to return if key not found
    /// @return Value from configuration or default if not found
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // ============================================================================
    // Value Setting Methods
    // ============================================================================
    
    ///
    /// Set a configuration value by key with explicit source.
    ///
    /// @param key Configuration key to set
    /// @param value Configuration value
    /// @param source How the value was set (Default, File, CommandLine, Runtime)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    ///
    /// Set a configuration string value.
    ///
    /// @param key Configuration key
    /// @param value String value
    /// @param source Source of the value
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    ///
    /// Set a configuration integer value.
    ///
    /// @param key Configuration key
    /// @param value Integer value
    /// @param source Source of the value
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    ///
    /// Set a configuration double value.
    ///
    /// @param key Configuration key
    /// @param value Double value
    /// @param source Source of the value
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    ///
    /// Set a configuration boolean value.
    ///
    /// @param key Configuration key
    /// @param value Boolean value
    /// @param source Source of the value
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // ============================================================================
    // Query Methods
    // ============================================================================
    
    ///
    /// Check if a configuration key exists.
    ///
    /// @param key Configuration key to check
    /// @return true if key exists, false otherwise
    bool has(const std::string& key) const;
    
    ///
    /// Remove a configuration key.
    ///
    /// @param key Configuration key to remove
    void remove(const std::string& key);
    
    ///
    /// Get all configuration keys.
    ///
    /// @return Vector of all configuration keys
    std::vector<std::string> getKeys() const;
    
    ///
    /// Clear all configuration entries.
    void clear();
    
    // ============================================================================
    // Configuration Management Methods
    // ============================================================================
    
    ///
    /// Get a string summary of the current configuration.
    ///
    /// @return String containing formatted configuration summary
    std::string summary() const;
    
    ///
    /// Validate the entire configuration against schema.
    ///
    /// @return true if configuration is valid, false otherwise
    bool validateConfiguration() const;
    
    ///
    /// Validate and report configuration errors.
    ///
    /// @param filepath Path to configuration file for context
    /// @return String containing validation errors, empty if valid
    std::string validateAndReport(const std::string& filepath) const;
    
    ///
    /// Validate a single configuration entry.
    ///
    /// @param entry Configuration entry to validate
    /// @return Error message if invalid, empty string if valid
    std::string validateEntry(const ConfigEntry& entry) const;
    
private:
    // ============================================================================
    // Internal Implementation
    // ============================================================================
    
    ///
    /// Private implementation struct to hide implementation details.
    struct Impl;
    
    ///
    /// Smart pointer to implementation details.
    std::unique_ptr<Impl> pImpl;
    
    // ============================================================================
    // Helper Functions
    // ============================================================================
    
    ///
    /// Trim whitespace from both ends of a string.
    ///
    /// @param str String to trim
    /// @return Trimmed string
    static std::string trim(const std::string& str);
    
    ///
    /// Convert string to lowercase.
    ///
    /// @param str String to convert
    /// @return Lowercase version of the string
    static std::string toLower(const std::string& str);
    
    ///
    /// Parse JSON value string into appropriate ConfigValue type.
    ///
    /// @param str JSON string to parse
    /// @return Parsed numeric value
    static double parseJSONNumber(const std::string& str);
    
    ///
    /// Parse JSON boolean string.
    ///
    /// @param str JSON boolean string ("true" or "false")
    /// @return Parsed boolean value
    static bool parseJSONBool(const std::string& str);
    
    ///
    /// Parse JSON string, removing surrounding quotes.
    ///
    /// @param str JSON string (with quotes)
    /// @return Unquoted string
    static std::string parseJSONString(const std::string& str);
    
    ///
    /// Escape JSON special characters in a string.
    ///
    /// @param str String to escape
    /// @return JSON-escaped string
    static std::string escapeJSON(const std::string& str);
    
    ///
    /// Parse a JSON key-value pair line.
    ///
    /// @param line JSON line (e.g., "  \"key\": value")
    /// @param value Output parameter for parsed value
    /// @param outKey Output parameter for parsed key
    /// @return true if parsing succeeded, false otherwise
    static bool parseJSONValue(const std::string& line, ConfigValue& value, std::string& outKey);
    
    ///
    /// Log configuration error with context.
    ///
    /// @param message Error message
    /// @param context Context information
    static void logConfigError(const std::string& message, const std::string& context = "");
    
    ///
    /// Get default configuration schema for validation.
    ///
    /// @return Map of keys to their descriptions and default values
    static const std::map<std::string, std::pair<std::string, ConfigValue>> getDefaultSchema();
};

} // namespace nlm
