#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>

namespace nlohmann {
    class json;
}

namespace nlm {

// Forward declarations
class Config;
class ConfigEnhanced;
class ConfigJSON;

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

// JSON configuration parser
class ConfigJSON {
public:
    static bool parseJSONFile(const std::string& filepath, std::shared_ptr<Config> config);
    static bool parseJSONString(const std::string& jsonString, std::shared_ptr<Config> config);
    static std::string generateJSON(const Config& config);
    
private:
    static void parseJSONConfig(const nlohmann::json& jsonData, std::shared_ptr<Config> config);
};

// Enhanced configuration with JSON support
class ConfigEnhanced : public Config {
public:
    bool loadFromJSON(const std::string& filepath);
    bool loadFromJSONString(const std::string& jsonString);
    std::string saveToJSON() const;
    bool validateJSON(const std::string& jsonString) const;
    bool saveToJSONFile(const std::string& filepath) const;
    bool loadFromJSONArgs(int argc, char** argv);
    bool validateConfig() const;
    bool operator==(const ConfigEnhanced& other) const;
    bool operator!=(const ConfigEnhanced& other) const;
    void merge(const ConfigEnhanced& other);
    std::string getConfigSummary() const;
    
private:
    friend class ConfigJSON;
    const std::vector<ConfigEntry>& getEntries() const;
    void addEntry(const ConfigEntry& entry);
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
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Load from JSON (enhanced interface)
    bool loadFromJSON(const std::string& filepath);
    bool loadFromJSONString(const std::string& jsonString);
    
    // Save to JSON (enhanced interface)
    std::string saveToJSON() const;
    bool saveToJSONFile(const std::string& filepath) const;
    
    // Validate JSON
    bool validateJSON(const std::string& jsonString) const;
    
    // Load from JSON command line arguments
    bool loadFromJSONArgs(int argc, char** argv);
    
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
    
    // Validate configuration
    bool validateConfig() const;
    
    // Configuration merge
    void merge(const Config& other);
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Access to internal entries for enhanced functionality
    const std::vector<ConfigEntry>& getEntries() const;
    void addEntry(const ConfigEntry& entry);
    
    friend class ConfigEnhanced;
    friend class ConfigJSON;
};

// Global configuration management
std::shared_ptr<ConfigEnhanced> getGlobalConfig();
void initializeConfig(const std::string& configFile);
void reloadConfig(const std::string& configFile);
bool validateConfig();

} // namespace nlm
