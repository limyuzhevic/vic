#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

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
    
    // Helper to convert to JSON
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["key"] = key;
        j["source"] = static_cast<int>(source);
        j["description"] = description;
        
        // Convert value based on type
        std::visit([&j](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j["value"] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j["value"] = arg;
            }
        }, value);
        
        return j;
    }
    
    // Helper to convert from JSON
    static ConfigEntry fromJson(const nlohmann::json& j) {
        ConfigEntry entry;
        if (j.contains("key")) entry.key = j["key"].get<std::string>();
        if (j.contains("source")) entry.source = static_cast<ConfigSource>(j["source"].get<int>());
        if (j.contains("description")) entry.description = j["description"].get<std::string>();
        if (j.contains("value")) {
            const auto& valueJson = j["value"];
            // Determine type from JSON and convert
            if (valueJson.is_number_integer()) {
                // Check if it fits in int or int64_t
                if (valueJson.get<int64_t>() <= std::numeric_limits<int>::max() &&
                    valueJson.get<int64_t>() >= std::numeric_limits<int>::min()) {
                    entry.value = valueJson.get<int>();
                } else {
                    entry.value = valueJson.get<int64_t>();
                }
            } else if (valueJson.is_number_float()) {
                entry.value = valueJson.get<double>();
            } else if (valueJson.is_boolean()) {
                entry.value = valueJson.get<bool>();
            } else if (valueJson.is_string()) {
                entry.value = valueJson.get<std::string>();
            } else if (valueJson.is_array()) {
                // Try to determine element type
                if (!valueJson.empty()) {
                    if (valueJson[0].is_number_integer()) {
                        entry.value = valueJson.get<std::vector<int>>();
                    } else if (valueJson[0].is_number_float()) {
                        entry.value = valueJson.get<std::vector<double>>();
                    } else if (valueJson[0].is_string()) {
                        entry.value = valueJson.get<std::vector<std::string>>();
                    }
                }
            }
        }
        return entry;
    }
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
    bool loadFromJson(const std::string& jsonString);
    
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
    
    // Get all entries as JSON array
    nlohmann::json exportToJson() const;
    
    // Load from JSON array
    bool importFromJson(const nlohmann::json& jsonData);
    
    // Configuration validation
    std::vector<std::string> validate() const;
    
    // Apply configuration with validation
    bool apply(const std::string& filepath);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON parsing helpers
    static ConfigValue jsonToConfigValue(const nlohmann::json& j);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    
    // Validation helpers
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
};

} // namespace nlm
