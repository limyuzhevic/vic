#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    ConfigSource source;
    
    ConfigValidationError(const std::string& k, const std::string& msg, ConfigSource s)
        : key(k), message(msg), source(s) {}
};

// Configuration schema rule
enum class ValidationRule {
    RangeInt,
    RangeDouble,
    RangeFloat,
    RangeBool,
    StringLength,
    StringPattern,
    MinItems,
    MaxItems,
    Required,
    Enum
};

// Schema rule for validation
struct SchemaRule {
    ValidationRule type;
    std::string key;
    std::string description;
    
    // Union for rule parameters
    std::variant<
        std::pair<double, double>,  // Range: min, max
        size_t,                     // StringLength or MinItems or MaxItems
        std::string,                // StringPattern or Enum
        bool                        // Required flag (unused)
    > parameters;
    
    SchemaRule() : type(ValidationRule::Required), key(), description() {}
};

// Configuration help entry
struct ConfigHelpEntry {
    std::string key;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::string example;
    bool isRequired;
    
    ConfigHelpEntry(const std::string& k, const std::string& desc, const std::string& t,
                   const std::string& def = "", const std::string& ex = "",
                   bool req = false)
        : key(k), description(desc), type(t), defaultValue(def), example(ex), isRequired(req) {}
};

// Configuration inheritance
struct ConfigInheritance {
    std::string baseFile;
    std::string overrideFile;
    bool merge;  // true for merge, false for override
    
    ConfigInheritance(const std::string& base, const std::string& override, bool m)
        : baseFile(base), overrideFile(override), merge(m) {}
};

// Configuration checksum
struct ConfigChecksum {
    std::string value;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigChecksum(const std::string& v) : value(v) {
        timestamp = std::chrono::system_clock::now();
    }
};

// File format detection
enum class FileFormat {
    Unknown,
    INI,
    JSON,
    YAML,
    YAML_MIXED_CASE
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
    
    // Enhanced file loading with format detection
    bool loadFromFile(const std::string& filepath);
    bool loadFromFileWithFormat(const std::string& filepath, FileFormat format);
    bool loadFromJSON(const std::string& content);
    bool loadFromYAML(const std::string& content);
    bool loadFromINI(const std::string& content);
    
    // Configuration schema and validation
    bool addSchemaRule(const SchemaRule& rule);
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const;
    
    // Configuration inheritance
    bool loadWithInheritance(const ConfigInheritance& inheritance);
    
    // Enhanced command line parsing
    bool loadFromArgs(int argc, char** argv);
    bool loadFromArgsWithValidation(int argc, char** argv);
    
    // Enhanced serialization
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToINI(const std::string& filepath) const;
    
    // Configuration help system
    static std::vector<ConfigHelpEntry> getConfigurationHelp();
    static std::string getConfigurationExamples();
    static std::string getAvailableOptions();
    
    // Configuration checksum and integrity
    ConfigChecksum computeChecksum() const;
    bool verifyChecksum(const ConfigChecksum& checksum) const;
    bool saveWithBackup(const std::string& filepath) const;
    
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
    
    // Get file format from extension
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // Get configuration help for specific key
    static std::string getKeyHelp(const std::string& key);
    
    // Validate command line arguments
    static bool validateCommandLineArgs(int argc, char** argv);
    
    // Generate usage information
    static std::string generateUsage(const std::string& programName);
    
    // Enhanced command line key validation
    static bool validateCommandLineKey(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    static std::string convertValueToString(const ConfigValue& value) const;
    static std::string computeMD5(const std::string& input) const;
};

// Global configuration instance
Config& getGlobalConfig();

// Utility functions for configuration
namespace config {
    // Parse command line arguments with enhanced support
    bool parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs);
    
    // Generate short and long option names
    std::string generateShortOption(const std::string& longOption);
    
    // Check if argument is a configuration option
    bool isConfigOption(const std::string& arg);
    
    // Get configuration value from parsed arguments
    template<typename T>
    std::optional<T> getFromArgs(const std::map<std::string, std::string>& args, const std::string& key);
}

} // namespace nlm
