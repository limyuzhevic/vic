#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <regex>
#include <chrono>

namespace nlm {

// Forward declarations for parser support
namespace detail {
    struct JsonConfigParser;
    struct YamlConfigParser;
    struct KeyValueConfigParser;
}

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Schema
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::string validationPattern;
    bool required;
    std::string defaultValue;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), type(), 
          validationPattern(), required(false), defaultValue(), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(), 
          validationPattern(), required(false), defaultValue(), 
          timestamp(std::chrono::system_clock::now()) {}
        
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc, const std::string& t,
                const std::string& pattern = "", bool req = false,
                const std::string& def = "")
        : key(k), value(v), source(s), description(desc), type(t), 
          validationPattern(pattern), required(req), defaultValue(def),
          timestamp(std::chrono::system_clock::now()) {}
};

// Schema validation structure
struct ConfigSchema {
    std::map<std::string, std::string> typeMappings;
    std::map<std::string, std::string> validationPatterns;
    std::map<std::string, std::string> defaultValues;
    std::map<std::string, bool> requiredFields;
    std::map<std::string, std::string> descriptions;
    
    void addField(const std::string& key, const std::string& type,
                  const std::string& description = "",
                  const std::string& pattern = "",
                  bool required = false,
                  const std::string& defaultVal = "");
    
    bool validate(const Config& config) const;
    std::string getFieldType(const std::string& key) const;
    std::string getFieldDescription(const std::string& key) const;
    std::string getFieldDefault(const std::string& key) const;
    bool isFieldRequired(const std::string& key) const;
};

// Configuration error types
class ConfigError : public std::runtime_error {
public:
    enum class ErrorType {
        FileNotFound,
        ParseError,
        ValidationError,
        TypeError,
        MissingField,
        InvalidValue,
        IoError,
        UnsupportedFormat
    };
    
    ConfigError(ErrorType type, const std::string& message, const std::string& key = "", 
                const std::string& filepath = "")
        : std::runtime_error(message), errorType(type), errorKey(key), errorFilepath(filepath) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getErrorKey() const { return errorKey; }
    const std::string& getErrorFilepath() const { return errorFilepath; }
    
private:
    ErrorType errorType;
    std::string errorKey;
    std::string errorFilepath;
};

// Parser interface base class
class IConfigParser {
public:
    virtual ~IConfigParser() = default;
    virtual bool canParse(const std::string& content) const = 0;
    virtual bool parse(const std::string& content, std::vector<ConfigEntry>& entries) = 0;
    virtual std::string serialize(const std::vector<ConfigEntry>& entries) const = 0;
    virtual std::string getFormatName() const = 0;
};

// Main configuration class with enhanced functionality
class Config {
public:
    Config();
    explicit Config(const ConfigSchema& schema);
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with auto-detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with explicit format
    bool loadFromFile(const std::string& filepath, const std::string& format);
    
    // Load from string with auto-detection
    bool loadFromString(const std::string& content, const std::string& format = "");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Specialized vector setters
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type conversion utilities
    static std::string toString(const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value);
    
    // Format detection utilities
    static std::string detectFormat(const std::string& content);
    static std::string detectFormatFromExtension(const std::string& filepath);
    
    // Validation utilities
    static bool validateValue(const ConfigValue& value, const std::string& type);
    static bool validatePattern(const std::string& value, const std::string& pattern);
    static std::string toLower(const std::string& str);
    static std::string trim(const std::string& str);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get file modification time
    static std::chrono::system_clock::time_point getFileModificationTime(const std::string& filepath);
    
    // Set schema
    void setSchema(const ConfigSchema& schema);
    
    // Get schema validation result
    std::vector<std::string> validateAgainstSchema() const;
    
    // Merge with another config
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Get format from parser
    std::string getCurrentFormat() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    bool loadWithParser(const std::string& content, const std::string& format);
    bool saveWithFormat(const std::vector<ConfigEntry>& entries, const std::string& format, 
                       std::ostream& output) const;
    std::string getFormatNameFromString(const std::string& format) const;
};

// Exception utilities
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// Parser implementations (forward declarations)
struct JsonConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override;
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override;
    std::string serialize(const std::vector<ConfigEntry>& entries) const override;
    std::string getFormatName() const override { return "JSON"; }
};

struct YamlConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override;
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override;
    std::string serialize(const std::vector<ConfigEntry>& entries) const override;
    std::string getFormatName() const override { return "YAML"; }
};

struct KeyValueConfigParser : public IConfigParser {
    bool canParse(const std::string& content) const override;
    bool parse(const std::string& content, std::vector<ConfigEntry>& entries) override;
    std::string serialize(const std::vector<ConfigEntry>& entries) const override;
    std::string getFormatName() const override { return "KEY_VALUE"; }
};

} // namespace nlm
