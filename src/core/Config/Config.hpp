#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <functional>

// Include nlohmann/json for JSON parsing and serialization
#include "nlohmann/json.hpp"

// Forward declaration for YAML
namespace YAML {
    class Node;
}

namespace nlm {

// Configuration system version
inline constexpr int CONFIG_VERSION = 1;

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public std::runtime_error {
public:
    ConfigFileError(const std::string& message) : std::runtime_error(message) {}
};

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

// Configuration value validator function type
using ConfigValidator = std::function<bool(const std::string&, const ConfigValue&)>;

// Configuration schema for validation
class ConfigSchema {
public:
    using DefaultValueFunc = std::function<ConfigValue()>;
    
    struct ConfigProperty {
        std::string name;
        std::string type;
        std::optional<ConfigValue> defaultValue;
        std::optional<ConfigValidator> validator;
        std::string description;
    };
    
    ConfigSchema() = default;
    
    void addProperty(const ConfigProperty& property) {
        properties_.push_back(property);
    }
    
    bool validate(const ConfigEntry& entry) const {
        auto prop = findProperty(entry.key);
        if (!prop) {
            return false; // Unknown property
        }
        
        // Check if property has a validator
        if (prop->validator && !prop->validator(entry.key, entry.value)) {
            return false;
        }
        
        return true;
    }
    
    std::optional<ConfigProperty> findProperty(const std::string& name) const {
        for (const auto& prop : properties_) {
            if (prop.name == name) {
                return prop;
            }
        }
        return std::nullopt;
    }
    
    const std::vector<ConfigProperty>& getProperties() const { return properties_; }
    
private:
    std::vector<ConfigProperty> properties_;
};

// JSON serialization helper for ConfigValue
inline void to_json(nlohmann::json& j, const ConfigValue& value) {
    std::visit([&j](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        } else {
            throw std::runtime_error("Unsupported ConfigValue type for JSON serialization");
        }
    }, value);
}

// JSON deserialization helper for ConfigValue
inline void from_json(const nlohmann::json& j, ConfigValue& value) {
    if (j.is_number_integer()) {
        if (j.get<int64_t>() <= std::numeric_limits<int>::max()) {
            value = j.get<int>();
        } else {
            value = j.get<int64_t>();
        }
    } else if (j.is_number_float()) {
        value = j.get<double>();
    } else if (j.is_boolean()) {
        value = j.get<bool>();
    } else if (j.is_string()) {
        value = j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            const auto& first = j[0];
            if (first.is_number_integer()) {
                std::vector<int> vec = j.get<std::vector<int>>();
                value = vec;
            } else if (first.is_number_float()) {
                std::vector<double> vec = j.get<std::vector<double>>();
                value = vec;
            } else if (first.is_string()) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                value = vec;
            }
        }
    }
}

// JSON serialization helper for ConfigEntry
inline void to_json(nlohmann::json& j, const ConfigEntry& entry) {
    nlohmann::json entryJson;
    entryJson["key"] = entry.key;
    entryJson["source"] = static_cast<int>(entry.source);
    entryJson["description"] = entry.description;
    
    // Add the actual value
    std::visit([&entryJson](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            entryJson["value"] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            entryJson["value"] = arg;
        }
    }, entry.value);
    
    j.push_back(entryJson);
}

// JSON deserialization helper for ConfigEntry
inline void from_json(const nlohmann::json& j, ConfigEntry& entry) {
    entry.key = j.value("key", "");
    entry.source = static_cast<ConfigSource>(j.value("source", static_cast<int>(ConfigSource::File)));
    entry.description = j.value("description", "");
    
    if (j.contains("value")) {
        entry.value = j["value"].get<ConfigValue>();
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
    
    // Load from file (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
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
    
    // New JSON/YAML methods
    bool loadFromJson(const std::string& jsonString);
    bool saveToJson(std::string& jsonString) const;
    bool loadFromYaml(const std::string& yamlString);
    bool saveToYaml(std::string& yamlString) const;
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    
    // Validate current configuration
    bool validate() const;
    
    // Helper methods for nested keys (e.g., "brain.neuron_count")
    std::optional<ConfigValue> getNested(const std::string& key) const;
    void setNested(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool hasNested(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string normalizeKey(const std::string& key);
    static std::string denormalizeKey(const std::string& key);
    
    // File format detection
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isSimpleConfigFile(const std::string& filepath);
};