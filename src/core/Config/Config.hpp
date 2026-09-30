#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, std::string>  // For nested key-value pairs
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Registry
};

// Advanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;      // Track which file this came from
    int lineNumber;            // Track line number in source file
    bool isSecret;             // For sensitive configuration values
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), filePath(), lineNumber(0), isSecret(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& file = "", int line = 0)
        : key(k), value(v), source(s), description(desc), filePath(file), lineNumber(line), isSecret(false) {}
};

// Validation callback function type
using ValidationCallback = std::function<bool(const ConfigEntry&)>;

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with multiple format support
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromSimplified(const std::string& filepath);
    bool loadFromSimple(const std::string& filepath);
    
    // Enhanced command line argument loading
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Enhanced save functionality
    bool saveToFile(const std::string& filepath, Format format = Format::JSON) const;
    
    // Configuration validation
    bool validate() const;
    void setValidationCallback(ValidationCallback callback);
    
    // Configuration querying with enhanced features
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced configuration operations
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all configuration
    void clear();
    
    // Get configuration summary with enhanced information
    std::string summary() const;
    std::string getSourceSummary(ConfigSource source) const;
    
    // Configuration import/export
    bool importFrom(const Config& other);
    bool exportTo(Config& other) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration merging
    void merge(const Config& other, bool overwrite = true);
    
    // Configuration snapshot and restore
    std::string createSnapshot() const;
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // Enhanced logging and debugging
    std::string validateAgainstSchema(const std::string& schemaFile) const;
    
    // File format enum
    enum class Format {
        SIMPLE,
        JSON,
        YAML,
        INI,
        XML
    };

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    
    // Advanced parsing helpers
    bool parseJSONValue(const std::string& content, size_t& pos, ConfigValue& outValue);
    bool parseYAMLValue(const std::string& content, size_t& pos, ConfigValue& outValue);
    bool parseSimpleValue(const std::string& line, ConfigValue& outValue);
    
    // Validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    bool validateTypes() const;
};
