#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

// Forward declarations for JSON/YAML support
namespace nlohmann {
    class json;
}

namespace YAML {
    class Node;
}

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

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

// Forward declarations for JSON/YAML support
namespace nlohmann {
    class json;
}

namespace YAML {
    class Node;
}

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

// Configuration class
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
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from JSON node
    bool loadFromJSON(const nlohmann::json& jsonNode);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from YAML node
    bool loadFromYAML(const YAML::Node& yamlNode);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // Save to YAML string
    std::string saveToYAML() const;
    
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration validation
    std::vector<std::string> validate() const;
    
    // Get configuration schema
    static std::vector<std::string> getDefaultKeys();
    
    // Merge another configuration
    void merge(const Config& other);
    
    // Apply preset configuration
    bool applyPreset(const std::string& presetName);
    
    // Get current configuration format
    std::string getFormat() const;
    
    // Validate configuration against schema
    bool validateSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML helpers
    static ConfigValue jsonToConfigValue(const nlohmann::json& value);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue yamlToConfigValue(const YAML::Node& node);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    
    // Configuration validation helpers
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value, const std::string& key);
    
    // Preset configurations
    static std::unordered_map<std::string, std::vector<std::pair<std::string, ConfigValue>>> getPresets();
};

} // namespace nlm

} // namespace nlm
