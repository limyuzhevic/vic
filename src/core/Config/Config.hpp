#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
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
    
    // Configuration validation and error handling
    bool validateConfig() const;
    std::string getValidationErrors() const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Configuration parameter metadata and descriptions
    struct ParameterInfo {
        std::string name;
        std::string description;
        ConfigValue defaultValue;
        ConfigSource defaultSource;
        std::vector<std::string> validValues; // For enum/string parameters
        bool required;
        float minValue; // For numeric parameters
        float maxValue; // For numeric parameters
        
        ParameterInfo() : name(), description(), defaultValue(), defaultSource(ConfigSource::Default),
                         required(false), minValue(std::numeric_limits<float>::lowest()),
                         maxValue(std::numeric_limits<float>::max()) {}
        
        ParameterInfo(const std::string& n, const std::string& desc, const ConfigValue& def,
                     ConfigSource src, bool req = false, float min = std::numeric_limits<float>::lowest(),
                     float max = std::numeric_limits<float>::max())
            : name(n), description(desc), defaultValue(def), defaultSource(src), required(req),
              minValue(min), maxValue(max) {}
    };
    
    // Get parameter information
    static const std::vector<ParameterInfo>& getParameterInfos();
    
    // Get parameter info for a specific key
    static ParameterInfo getParameterInfo(const std::string& key);
    
    // Check if configuration has sensible defaults
    bool hasSensibleDefaults() const;
    
    // Suggest improvements for problematic configuration
    std::vector<std::string> getConfigurationSuggestions() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static bool isValidName(const std::string& key);
    static bool isValidValue(const std::string& key, const ConfigValue& value);
    static std::string getTypeName(const ConfigValue& value);
};

} // namespace nlm
