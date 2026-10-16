#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

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
    Runtime,
    JSON,
    YAML
};

// Configuration entry with description
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    std::string defaultValue;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          validationRegex(), defaultValue(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& regex = "",
                const std::string& def = "", bool req = false)
        : key(k), value(v), source(s), description(desc), 
          validationRegex(regex), defaultValue(def), required(req) {}
};

// JSON/YAML configuration schema
struct ConfigSchema {
    std::map<std::string, ConfigEntry> entries;
    std::string filename;
    std::string schemaVersion;
    
    bool validate(const std::map<std::string, std::string>& jsonData) const;
    std::map<std::string, std::string> toFlatFormat() const;
    static ConfigSchema fromFile(const std::string& filepath);
    static ConfigSchema fromJSON(const std::string& jsonContent);
    static ConfigSchema fromYAML(const std::string& yamlContent);
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
    
    // Load from file (JSON format preferred, YAML if .yaml, fallback to key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML content string
    bool loadFromYAMLContent(const std::string& yamlContent);
    
    // Load from JSON content string
    bool loadFromJSONContent(const std::string& jsonContent);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (default: key=value format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON file
    bool saveToJSON(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYAML(const std::string& filepath) const;
    
    // Load schema for validation
    bool loadSchema(const std::string& schemaFile);
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type conversion
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with schema validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration entries for schema validation
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Schema management
    bool setSchema(const ConfigSchema& schema);
    const ConfigSchema* getSchema() const;
    
    // Configuration conversion
    std::string toJSON() const;
    std::string toYAML() const;
    static std::string toFlatFormat(const Config& config);
    
    // File operations
    static bool fileExists(const std::string& filepath);
    static std::string getConfigDirectory();
    static std::vector<std::string> findConfigFiles(const std::string& directory);
    
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static int stringToInt(const std::string& str);
    static double stringToDouble(const std::string& str);
    static bool isValidJSON(const std::string& str);
    static bool isValidYAML(const std::string& str);
    
    // Configuration merge
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool mergeFromJSON(const std::string& jsonContent, ConfigSource source = ConfigSource::JSON);
    bool mergeFromYAML(const std::string& yamlContent, ConfigSource source = ConfigSource::YAML);
    
    // Template implementations
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Exception classes
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& message) : ConfigException(message) {}
};

// Configuration conversion functions
extern "C" {
    // C-style API for backward compatibility
    Config* ConfigCreate();
    void ConfigDestroy(Config* config);
    bool ConfigLoadFromFile(Config* config, const char* filepath);
    bool ConfigLoadFromJSON(Config* config, const char* filepath);
    bool ConfigLoadFromYAML(Config* config, const char* filepath);
    bool ConfigSaveToFile(Config* config, const char* filepath);
    bool ConfigSaveToJSON(Config* config, const char* filepath);
    bool ConfigSaveToYAML(Config* config, const char* filepath);
    bool ConfigHas(Config* config, const char* key);
    const char* ConfigGetString(Config* config, const char* key, const char* defaultValue);
    int ConfigGetInt(Config* config, const char* key, int defaultValue);
    double ConfigGetDouble(Config* config, const char* key, double defaultValue);
    bool ConfigGetBool(Config* config, const char* key, bool defaultValue);
    void ConfigSetString(Config* config, const char* key, const char* value);
    void ConfigSetInt(Config* config, const char* key, int value);
    void ConfigSetDouble(Config* config, const char* key, double value);
    void ConfigSetBool(Config* config, const char* key, bool value);
    const char* ConfigSummary(Config* config);
}

} // namespace nlm