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
    Runtime,
    Environment
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;           // Type information for validation
    bool required = false;       // Whether field is required
    std::string validationRegex; // Regex pattern for validation
    std::string defaultValue;    // Default value if not provided
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   type("string"), required(false), defaultValue() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "string",
                bool req = false, const std::string& regex = "",
                const std::string& def = "")
        : key(k), value(v), source(s), description(desc), type(t), 
          required(req), validationRegex(regex), defaultValue(def) {}
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, YAML, and simple key=value format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from YAML file
    bool loadFromYaml(const std::string& filepath, bool validate = true);
    
    // Load from JSON file  
    bool loadFromJson(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file (supports JSON, YAML, and simple key=value format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYaml(const std::string& filepath) const;
    
    // Save to JSON file
    bool saveToJson(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with enhanced metadata
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with enhanced information
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    
    // Get source information
    ConfigSource getSource(const std::string& key) const;
    
    // Check if value has been modified
    bool isModified(const std::string& key) const;
    
    // Get all entries
    std::vector<ConfigEntry> getAllEntries() const { return pImpl->entries; }
    
    // Configuration merge operations
    void merge(const Config& other, ConfigSource source = ConfigSource::CommandLine);
    
    // Configuration diff operations
    std::vector<std::string> diff(const Config& other) const;
    
    // Export/import operations
    std::string exportToJson() const;
    bool importFromJson(const std::string& json);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesRegex(const std::string& value, const std::string& regex);
    static std::string valueToString(const ConfigValue& value);
    static std::string getTypeName(const ConfigValue& value);
    
    // Simple file format parsing (backward compatibility)
    bool parseSimpleFormat(const std::string& content, ConfigSource source, 
                          const std::string& description);
    
    // JSON/YAML parsing helpers (simplified implementations)
    bool parseSimpleJson(const std::string& json, ConfigSource source,
                        const std::string& description);
    bool parseSimpleYaml(const std::string& yaml, ConfigSource source,
                        const std::string& description);
    
    // Validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    
    friend class ConfigFileParser;
    friend class ConfigJsonParser;
    friend class ConfigYamlParser;
};

// Simple file format parser for backward compatibility
class ConfigFileParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries,
                     ConfigSource source, const std::string& description);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
};

// Simplified JSON parser (requires external JSON library if available)
class ConfigJsonParser {
public:
    static bool parse(const std::string& json, std::vector<ConfigEntry>& entries,
                     ConfigSource source, const std::string& description);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
};

// Simplified YAML parser (requires external YAML library if available)
class ConfigYamlParser {
public:
    static bool parse(const std::string& yaml, std::vector<ConfigEntry>& entries,
                     ConfigSource source, const std::string& description);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
};

} // namespace nlm

} // namespace nlm
