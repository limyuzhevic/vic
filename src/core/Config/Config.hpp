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
    
    // Load from command line arguments (advanced parsing)
    bool loadFromArgs(int argc, char** argv);
    
    // Load from file (JSON/YAML/legacy format)
    bool loadFromFile(const std::string& filepath);
    
    // Save to file (JSON format preferred)
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
    
    // Command line argument parsing (alias for loadFromArgs)
    bool parseCommandLine(int argc, char** argv) {
        return loadFromArgs(argc, argv);
    }
    
    static void printHelp(const std::string& programName);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::string& value, const ConfigValue& expectedType);
    static std::string valueToString(const ConfigValue& value);
    static std::string getDataTypeName(const ConfigValue& value);
    
    // Configuration file helpers
    static std::string getFileExtension(const std::string& filename);
    static std::vector<std::string> splitKeyPath(const std::string& key);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string formatConfigValue(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // JSON/YAML parsing helpers
#ifdef HAVE_NLOHMANN_JSON
    bool loadFromJSON(const std::string& filepath);
    bool saveToJSON(const std::string& filepath) const;
    nlohmann::json configValueToJSON(const ConfigValue& value);
    void parseJSONToConfig(const nlohmann::json& jsonData, const std::string& prefix, ConfigSource source);
    ConfigValue jsonValueToConfigValue(const nlohmann::json& jsonVal);
#endif
    
    bool loadFromYAML(const std::string& filepath);
    bool saveToYAML(const std::string& filepath) const;
    
    bool loadFromLegacy(const std::string& filepath);
    bool saveToLegacyFile(const std::string& filepath) const;
};

} // namespace nlm