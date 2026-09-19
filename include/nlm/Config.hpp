#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

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

// Configuration categories
enum class ConfigCategory {
    General,
    Brain,
    Dynamics,
    Plasticity,
    Development,
    Neuromodulation,
    Memory,
    Cognition,
    Sensory,
    Motor,
    Environment,
    Custom
};

// Type alias for validation function
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration options for a parameter
struct ConfigOptions {
    std::string description;
    ValidationFunction validator;
    ConfigValue defaultValue;
    ConfigCategory category;
    bool required = false;
    
    ConfigOptions() : description(""), validator(nullptr), defaultValue(), category(ConfigCategory::Custom), required(false) {}
    
    ConfigOptions(const std::string& desc, ConfigCategory cat = ConfigCategory::Custom) 
        : description(desc), validator(nullptr), defaultValue(), category(cat), required(false) {}
    
    ConfigOptions withValidator(ValidationFunction val) {
        ConfigOptions opts = *this;
        opts.validator = val;
        return opts;
    }
    
    ConfigOptions withDefault(const ConfigValue& def) {
        ConfigOptions opts = *this;
        opts.defaultValue = def;
        return opts;
    }
    
    ConfigOptions inCategory(ConfigCategory cat) {
        ConfigOptions opts = *this;
        opts.category = cat;
        return opts;
    }
    
    // Method to set required flag
    ConfigOptions& setRequired(bool req) {
        required = req;
        return *this;
    }
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
               const std::string& desc = "", const std::string& cat = "")
        : key(k), value(v), source(s), description(desc), category(cat) {}
};

// Nested configuration node
struct ConfigNode {
    std::string name;
    std::unordered_map<std::string, ConfigValue> values;
    std::unordered_map<std::string, ConfigNode> children;
    ConfigOptions options;
    
    ConfigNode() : name(""), options() {}
    ConfigNode(const std::string& n) : name(n), options() {}
    
    bool hasValue(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    ConfigValue getValue(const std::string& key) const {
        auto it = values.find(key);
        if (it != values.end()) {
            return it->second;
        }
        return ConfigValue(); // Empty variant
    }
    
    void setValue(const std::string& key, const ConfigValue& value) {
        values[key] = value;
    }
    
    bool hasChild(const std::string& name) const {
        return children.find(name) != children.end();
    }
    
    ConfigNode& getChild(const std::string& name) {
        return children[name];
    }
    
    const ConfigNode& getChild(const std::string& name) const {
        return children.at(name);
    }
    
    std::vector<std::string> getChildNames() const {
        std::vector<std::string> names;
        names.reserve(children.size());
        for (const auto& pair : children) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::string> getValueKeys() const {
        std::vector<std::string> keys;
        keys.reserve(values.size());
        for (const auto& pair : values) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string summary(int indent = 0) const;
};

// Config structure helper
namespace config_utils {
    // Convert string to lowercase
    inline std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Trim whitespace
    inline std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Parse simple key=value format for backward compatibility
    inline std::unordered_map<std::string, std::string> parseSimpleIni(const std::string& content) {
        std::unordered_map<std::string, std::string> result;
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                result[key] = value;
            }
        }
        return result;
    }
    
    // Validate integer range
    inline bool validateIntRange(const ConfigValue& value, int min, int max) {
        if (auto val = std::get_if<int>(&value)) {
            return *val >= min && *val <= max;
        }
        return false;
    }
    
    // Validate double range
    inline bool validateDoubleRange(const ConfigValue& value, double min, double max) {
        if (auto val = std::get_if<double>(&value)) {
            return *val >= min && *val <= max;
        }
        return false;
    }
    
    // Validate string length
    inline bool validateStringLength(const ConfigValue& value, size_t minLength, size_t maxLength) {
        if (auto val = std::get_if<std::string>(&value)) {
            return val->length() >= minLength && val->length() <= maxLength;
        }
        return false;
    }
    
    // Validate positive
    inline bool validatePositive(const ConfigValue& value) {
        if (auto val = std::get_if<int>(&value)) {
            return *val > 0;
        } else if (auto val = std::get_if<double>(&value)) {
            return *val > 0.0;
        }
        return false;
    }
}

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
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    
    // Backward compatibility: load from simple key=value file
    bool loadFromSimpleFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML/INI format)
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
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
    
    // Enhanced methods for nested configs
    bool hasNested(const std::string& path) const;
    std::optional<ConfigValue> getNested(const std::string& path) const;
    void setNested(const std::string& path, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    void addEntry(const std::string& key, const ConfigValue& value, const std::string& description = "", 
                  const std::string& category = "", const ConfigOptions& options = ConfigOptions());
    
    std::vector<ConfigEntry> getEntries() const;
    void setEntryDescription(const std::string& key, const std::string& description);
    void setEntryCategory(const std::string& key, const std::string& category);
    
    // Validation
    bool validate(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON parsing helpers
    static void parseJsonToConfig(const nlohmann::json& json, Config& config);
    static void parseYamlToConfig(const YAML::Node& yaml, Config& config);
    static nlohmann::json configToJson(const Config& config);
    static YAML::Node configToYaml(const Config& config);
};

} // namespace nlm
