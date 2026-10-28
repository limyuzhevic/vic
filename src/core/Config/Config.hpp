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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from simple key=value file (backward compatibility)
    bool loadFromKeyValue(const std::string& filepath);
    
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
    
    // Advanced features for power users
    
    // Get configuration source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key was modified from file
    bool isFileModified(const std::string& key) const;
    
    // Get all keys from a specific source
    std::vector<std::string> getKeysFromSource(ConfigSource source) const;
    
    // Validate configuration against schema
    bool validate(const std::string& schemaFile = "") const;
    
    // Get configuration as JSON string
    std::string toJSON() const;
    
    // Get configuration as YAML string
    std::string toYAML() const;
    
    // Load from environment variables (prefix)
    void loadFromEnv(const std::string& prefix = "NLM_");
    
    // Merge another configuration
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Export configuration to simple key=value format
    void exportKeyValue(std::ostream& output) const;
    
    // Import configuration from simple key=value format
    bool importKeyValue(std::istream& input, ConfigSource source = ConfigSource::File);
    
    // Configuration templates for common patterns
    static Config getDefaultConfig();
    static Config getProductionConfig();
    static Config getDebugConfig();
    
private:
    // Direct implementation instead of Pimpl for better performance and simplicity
    std::vector<ConfigEntry> entries_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing helpers
    void flattenJSON(const nlohmann::json& json, const std::string& prefix, ConfigSource source);
    void flattenYAML(const YAML::Node& node, const std::string& prefix, ConfigSource source);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Validation helpers
    bool validateSchema(const std::string& schema) const;
    void validateValueTypes() const;
    
    // Environment variable helpers
    void processEnvVariable(const std::string& name, const std::string& value);
    
    // JSON/YAML export helpers
    void toJSONImpl(nlohmann::json& json) const;
    void toYAMLImpl(YAML::Emitter& emitter) const;
};

} // namespace nlm
