#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>

// Forward declarations
class json;

namespace nlm {

// Forward declarations
class Config;

// Configuration validation callback type
typedef std::function<bool(const std::string&, const ConfigValue&)> ValidationCallback;

// Schema validation
typedef std::function<bool(const std::string&, const std::string&, const ConfigValue&)> SchemaValidationCallback;

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

// Schema validation information
struct ConfigSchema {
    std::string key;
    ConfigValue defaultValue;
    std::string typeName;
    std::string description;
    ValidationCallback validator;
    bool required;
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const ConfigValue& def, const std::string& type, const std::string& desc, bool req = false)
        : key(k), defaultValue(def), typeName(type), description(desc), required(req) {}
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Export to JSON
    std::string exportToJson() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get values with type validation
    template<typename T>
    std::optional<T> get(const std::string& key, std::function<bool(const T&)> validator) const;
    
    // Set values with validation
    bool setValidated(const std::string& key, const ConfigValue& value, 
                     ConfigSource source = ConfigSource::Runtime,
                     const ValidationCallback& validator = nullptr);
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration schema management
    void addSchema(const ConfigSchema& schema);
    bool validateSchema(const std::string& key, const ConfigValue& value) const;
    void validateAll() const;
    
    // Schema validation callback
    void setSchemaValidationCallback(const SchemaValidationCallback& callback);
    
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
    
    // Get configuration statistics
    std::string statistics() const;
    
    // Configuration group management
    void beginGroup(const std::string& group);
    void endGroup();
    std::string getCurrentGroup() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON helper functions
    static std::string jsonValueToString(const ConfigValue& value);
    static ConfigValue stringToJsonValue(const std::string& str, const std::string& typeHint = "");
    
    // File parsing helpers
    bool parseLegacyFileFormat(const std::string& content);
    bool parseJsonFile(const std::string& content);
    bool parseYamlFile(const std::string& content);
    
    // Value saving helpers
    static void saveValueToFile(std::ostream& file, const ConfigValue& value);
    static void saveValueToJson(std::ostream& file, const ConfigValue& value);
    
    // Schema management
    std::vector<ConfigSchema> getSchemas() const;
    std::string getGroupPrefix(const std::string& key) const;
    bool isValidKey(const std::string& key) const;
};

// Global configuration management
class GlobalConfig {
public:
    static GlobalConfig& getInstance();
    static void setInstance(Config* config);
    
    Config* getConfig();
    const Config* getConfig() const;
    
    void loadFromFile(const std::string& filepath);
    void loadFromArgs(int argc, char** argv);
    void saveToFile(const std::string& filepath) const;
    
private:
    GlobalConfig() : config_(nullptr) {}
    Config* config_;
    
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
};

} // namespace nlm
