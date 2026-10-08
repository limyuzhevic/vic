#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <regex>
#include <cctype>
#include <stack>
#include <tuple>
#include <any>
#include <fstream>
#include <stdexcept>

namespace nlm {

// Forward declarations
class ConfigParser;
class Config;

// Configuration value types for flexible configuration storage
//
// ConfigValue uses std::variant to support multiple data types:
// - Integral types: int, int64_t
// - Floating-point types: double
// - Boolean type: bool
// - String type: std::string
// - Array types: std::vector<int>, std::vector<double>, std::vector<std::string>
//
// This enables heterogeneous configuration storage while maintaining type safety
// for configuration values through the get<T>() methods.
//
// Used for:
// - Storing configuration parameters
// - Serializing/deserializing configuration
// - Type-safe value access via Config::get<T>()
//
// @see Config::get(), Config::set(), Config::getNested()
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

// Configuration source enumeration
//
// Indicates the origin of a configuration value, used for:
// - Configuration precedence determination
// - Tracking configuration provenance
// - Merging configurations from different sources
// - Validation and error reporting
//
// Sources are processed in this order (later sources override earlier ones):
// 1. Default: Built-in defaults
// 2. File: Configuration files (JSON, YAML, etc.)
// 3. CommandLine: Command-line arguments
// 4. Environment: Environment variables
// 5. Runtime: Programmatic configuration changes
//
// @see Config::loadFromDefaults(), Config::loadFromFile()
enum class ConfigSource {
    Default,      // Built-in default configuration
    File,         // Configuration file (JSON/YAML/toml)
    CommandLine,  // Command-line arguments
    Environment,  // Environment variables
    Runtime      // Programmatic runtime configuration
};

// Configuration entry for storing key-value pairs with metadata
//
// Represents a single configuration parameter with:
// - Key: Unique identifier for the configuration parameter
// - Value: Configuration value (type determined by ConfigValue)
// - Source: Origin of the configuration value
// - Description: Human-readable explanation of the parameter
//
// Used internally by ConfigParser and exposed through Config::get(),
// Config::set(), and other Config methods.
//
// @note Configuration entries are used to track:
//       - Configuration provenance (where values came from)
//       - Type safety (through ConfigValue variant)
//       - Validation rules
//       - Priority for overriding
struct ConfigEntry {
    std::string key;                // Configuration parameter key
    ConfigValue value;              // Configuration value
    ConfigSource source;            // Configuration source
    std::string description;        // Human-readable description
    
    // Default constructor creates empty entry
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description() {}
    
    // Constructor with all parameters
    //
    // @param k Configuration key
    // @param v Configuration value
    // @param s Configuration source
    // @param desc Description of configuration parameter
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration Parser Class
//
// Comprehensive configuration management system supporting multiple input formats:
// - JSON files (.json)
// - YAML files (.yaml, .yml)
// - Python-style format (.pyconf)
// - Custom format (basic key=value syntax)
//
// The parser handles configuration from multiple sources and merges them
// according to precedence rules (later sources override earlier ones).
//
// Key features:
// - Multi-format support for flexibility
// - Error recovery and detailed error reporting
// - Type-safe value conversion
// - Validation support
// - Comprehensive configuration tracking
//
// @note ConfigParser is used by Config::loadFromFile() and other load methods
class ConfigParser {
public:
// Configuration parsing error enumeration
//
// Detailed error reporting for configuration parsing operations
enum class ParseError {
    Success,        // Parsing completed successfully
    FileNotFound,   // Configuration file not found
    ParseError,     // General parsing error (syntax, format, etc.)
    InvalidFormat,  // Unsupported or malformed file format
    TypeMismatch,   // Value type doesn't match expected type
    ValidationError // Value fails validation rules
};
    
// Result structure for configuration parsing operations
//
// Contains the result of parsing configuration from any source, including:
// - Parsed configuration data
// - Error status and messages
// - Error details for debugging
//
// @note ParseResult is used by all parse methods in ConfigParser
struct ParseResult {
    ParseError error;           // Parsing error status
    std::string message;        // Human-readable error message
    std::unordered_map<std::string, ConfigValue> config;  // Parsed configuration
};
    
// Default constructor
//
// @note Creates empty parser with no initial configuration
ConfigParser() = default;

    // Destructor
//
// @note Automatic cleanup of resources
    ~ConfigParser() = default;

    // Parse configuration file based on file extension
//
// @param filepath Path to configuration file
//
// @return ParseResult Structure containing:
//         - error status (Success if file parsed successfully)
//         - message (detailed error information if error)
//         - config (parsed configuration if successful)
//
// @note Supported file formats:
//       - JSON (.json)
//       - YAML (.yaml, .yml)
//       - Python-style (.pyconf)
//       - Custom format (.conf)
//
// @see parseContent() for parsing content directly
    ParseResult parseFile(const std::string& filepath);

    // Parse configuration content directly
//
// @param content Configuration content as string
// @param extension File extension for format detection (e.g., ".json", ".yaml")
//
// @return ParseResult Same as parseFile() but without file I/O
//
// @note Useful for:
//       - Configuration from database or network
//       - Configuration string manipulation
//       - Testing and debugging
//       - Dynamic configuration
//
// @see parseFile() for file-based parsing
    ParseResult parseContent(const std::string& content, const std::string& extension);

    // Parse configuration from command-line arguments
//
// @param argc Argument count (typically from main())
// @param argv Argument vector (typically from main())
//
// @return ParseResult Parsed configuration with command-line values
//
// @note Command-line parsing format:
//       - --key=value
//       - --key value
//       - --flag (boolean flags set to true)
//
// @note Command-line values have highest precedence after files
//       and environment variables in the overall configuration
    ParseResult parseCommandLine(int argc, char** argv);

    // Parse configuration from environment variables
//
// @param envVars Map of environment variable names to values
//
// @return ParseResult Parsed configuration from environment variables
//
// @note Environment variable naming convention:
//       - NLM_SECTION_KEY=value (for nested keys)
//       - NLM_KEY=value (for top-level keys)
//
// @note Environment variables are processed in alphabetical order
//       and can override file configuration values
    ParseResult parseEnvironment(const std::unordered_map<std::string, std::string>& envVars);

    // Convert parsed configuration to ConfigEntry format
//
// @param config Parsed configuration (from parseFile/parseContent/etc.)
// @param source Configuration source for tracking provenance
//
// @return std::vector<ConfigEntry> Configuration entries suitable for Config::set()
//
// @note This is an internal method used by the Config class to populate
//       its configuration from parsed sources
    static std::vector<ConfigEntry> convertToConfigEntries(const std::unordered_map<std::string, ConfigValue>& config,
                                                         ConfigSource source);
    
private:
    std::unordered_map<std::string, ConfigValue> mergeConfigs(const std::vector<std::unordered_map<std::string, ConfigValue>>& configs);
    
    // Configuration file extensions that are valid
    static const std::vector<std::string> JSON_EXTENSIONS;
    static const std::vector<std::string> YAML_EXTENSIONS;
    
    // Configuration value type helpers
    template<typename T>
    static bool convertToType(const ConfigValue& value, T& result);
    
    template<typename T>
    static ConfigValue convertFromType(const T& value);
};

// Value conversion utilities
class ConfigValueConverter {
public:
    template<typename T>
    static T convert(const ConfigValue& value);
    
    template<typename T>
    static bool tryConvert(const ConfigValue& value, T& result);
    
    // Check if value can be converted to specific type
    template<typename T>
    static bool isConvertible(const ConfigValue& value);
    
    // Get value type name
    static std::string getTypeName(const ConfigValue& value);
    
private:
    template<typename T>
    static bool vectorToVariant(const std::vector<T>& vec, ConfigValue& result);
    
    template<typename T>
    static bool variantToVector(const ConfigValue& value, std::vector<T>& result);
};

// Configuration validation
class ConfigValidator {
public:
    struct ValidationRule {
        std::string key;
        std::string type; // "int", "double", "string", "bool", "array"
        std::optional<std::string> pattern;
        std::optional<double> min;
        std::optional<double> max;
        bool required;
    };
    
    using ValidationRules = std::vector<ValidationRule>;
    
    ConfigValidator() = default;
    ~ConfigValidator() = default;
    
    // Add validation rule
    void addRule(const std::string& key, const std::string& type, bool required = false);
    void addRangeRule(const std::string& key, double min, double max);
    void addPatternRule(const std::string& key, const std::string& pattern);
    
    // Validate configuration entries
    bool validate(const std::vector<ConfigEntry>& entries) const;
    
    // Get validation errors
    std::vector<std::string> getErrors() const;
    
private:
    ValidationRules rules;
    mutable std::vector<std::string> errors;
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment();
    
    // Load from default configuration
    bool loadFromDefaults();
    
    // Load from multiple sources with inheritance
    bool loadWithInheritance(const std::vector<std::string>& filePaths);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type-safe conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get values using dot notation for nested access
    template<typename T>
    std::optional<T> getNested(const std::string& keyPath) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    bool hasNested(const std::string& keyPath) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeNested(const std::string& keyPath);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys with dot notation
    std::vector<std::string> getAllKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fileEntries;
        size_t cliEntries;
        size_t envEntries;
        size_t runtimeEntries;
        std::unordered_map<std::string, size_t> typeCounts;
    };
    
    ConfigStats getStats() const;
    
    // Validate configuration
    bool validate(const ConfigValidator::ValidationRules& rules) const;
    
    // Set validation rules
    void setValidator(const ConfigValidator& validator);
    
    // Get the underlying parser instance
    ConfigParser& getParser() { return parser; }
    const ConfigParser& getParser() const { return parser; }
    
    // Enable/disable logging
    void setLogging(bool enabled);
    bool isLogging() const;
    
    // Get last error messages
    std::vector<std::string> getLastErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ConfigParser parser;
    ConfigValidator validator;
    bool loggingEnabled;
    mutable std::vector<std::string> lastErrors;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Parse nested key paths
    std::optional<ConfigEntry> findNestedEntry(const std::string& keyPath) const;
    
    // Log helper methods
    void logInfo(const std::string& message) const;
    void logError(const std::string& message) const;
    void addError(const std::string& error);
};

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Nested get instantiations
namespace {
template std::optional<int> Config::getNested<int>(const std::string&) const;
template std::optional<double> Config::getNested<double>(const std::string&) const;
template std::optional<bool> Config::getNested<bool>(const std::string&) const;
template std::optional<std::string> Config::getNested<std::string>(const std::string&) const;
}

} // namespace nlm
