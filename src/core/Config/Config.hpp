#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <filesystem>

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
    std::vector<std::string>,
    std::unordered_map<std::string, ConfigValue>  // For JSON objects
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;      // Track which file this came from
    size_t lineNumber;         // For error reporting
    std::string validationError; // Store validation errors
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    filePath(), lineNumber(0), validationError() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), filePath(), lineNumber(0), validationError() {}
};

// Error types
enum class ConfigError {
    Success,
    FileNotFound,
    InvalidFormat,
    ParseError,
    ValidationError,
    TypeMismatch,
    FileWriteError
};

struct ConfigErrorInfo {
    ConfigError error;
    std::string message;
    std::string key;
    std::string filePath;
    size_t lineNumber;
};

// Format detection
enum class ConfigFormat {
    Unknown,
    KeyValue,
    JSON,
    YAML,
    JSONC,
    JSONL
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
    
    // Load from file (with format detection)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::string& prefix = "NLM_", bool skipEmpty = true);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr, const std::string& sourceInfo = "JSON string");
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr, const std::string& sourceInfo = "YAML string");
    
    // Save to file (JSON or key=value format)
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::JSON) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Validation
    std::vector<ConfigErrorInfo> validate() const;
    bool validateParameter(const std::string& key, std::string& error) const;
    
    // Format detection and parsing
    static ConfigFormat detectFormat(const std::string& filepath);
    static ConfigFormat detectFormatFromContent(const std::string& content);
    
    // Export/import
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    bool importFromJSON(const std::string& jsonStr);
    bool importFromYAML(const std::string& yamlStr);
    
    // Configuration merging
    void merge(const Config& other);
    void mergeFromFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigFormat detectFormatFromExtension(const std::string& filepath);
    
    // Private parsing methods
    bool loadFromKeyValue(const std::string& content, const std::string& sourceInfo = "");
    bool loadFromJSONInternal(const std::string& jsonStr, const std::string& sourceInfo = "");
    bool loadFromYAMLInternal(const std::string& yamlStr, const std::string& sourceInfo = "");
};

} // namespace nlm
