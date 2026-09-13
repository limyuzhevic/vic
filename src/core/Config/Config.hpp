#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <any>

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

// Parameter validation result
struct ValidationResult {
    bool valid = true;
    std::string errorMessage;
    std::string source;
};

// Type validation function
using ValidationFunc = std::function<ValidationResult(const ConfigValue& value, const std::string& key)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string sourceLabel;  // Human-readable source (e.g., "default.json", "--brain.neuron_count=1000")
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), sourceLabel() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), sourceLabel() {}
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
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;

    // Get values using dot notation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Helper get methods
    std::optional<int64_t> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    
    // Get nested values using dot notation with dot separated path
    template<typename T>
    std::optional<T> getNested(const std::vector<std::string>& path) const;

    // Set values with dot notation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");

    // Set nested values using dot notation with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& sourceLabel = "");

    // Get parameter source information
    ConfigSource getSource(const std::string& key) const;
    std::string getSourceLabel(const std::string& key) const;

    // Validate configuration parameter
    ValidationResult validate(const std::string& key) const;

    // Add validation rule for a parameter
    void addValidation(const std::string& key, ValidationFunc validator);

    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get flattened keys with dot notation
    std::vector<std::string> getFlatKeys() const;
    
    // Get keys with specific prefix
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Get configuration as nested structure (JSON/YAML format)
    std::any getAsNested() const;
    std::any getAsAny() const;

    // Clear all
    void clear();

    // Get configuration summary
    std::string summary() const;

    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;

    // Merge another configuration
    void merge(const Config& other);

    // Get parameter description
    std::string getDescription(const std::string& key) const;
    
    // Set parameter description
    void setDescription(const std::string& key, const std::string& description);

    // Check if parameter is locked (cannot be changed after loading)
    void setLocked(const std::string& key, bool locked);
    bool isLocked(const std::string& key) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::vector<std::string> splitKey(const std::string& key) const;
    std::string joinPath(const std::vector<std::string>& path) const;
    bool wildcardMatch(const std::string& pattern, const std::string& text) const;
    bool parseValue(const std::string& valueStr, ConfigValue& outValue) const;
    std::string sourceToString(ConfigSource source) const;
    std::string sourceToLabel(const std::string& filepath, ConfigSource source, const std::string& arg) const;
    void setNestedValue(nlohmann::json& j, const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm

} // namespace nlm
