#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <limits>
#include <sstream>
#include <iomanip>

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

// JSON/YAML helper for converting ConfigValue to/from JSON
class JsonUtils {
public:
    static nlohmann::json ConfigValueToJson(const ConfigValue& value) {
        return std::visit([](const auto& val) -> nlohmann::json {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return val;
            } else if constexpr (std::is_same_v<T, double>) {
                return val;
            } else if constexpr (std::is_same_v<T, bool>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return nlohmann::json(val);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return nlohmann::json(val);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return nlohmann::json(val);
            }
        }, value);
    }
    
    static ConfigValue JsonToConfigValue(const nlohmann::json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_number_integer()) {
            // Check if it fits in int64_t
            if (j.get<int64_t>() >= std::numeric_limits<int>::min() && 
                j.get<int64_t>() <= std::numeric_limits<int>::max()) {
                return j.get<int>();
            }
            return j.get<int64_t>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_array()) {
            // Determine array type by first element
            if (j.empty()) {
                return std::vector<std::string>{}; // Default to string vector
            }
            if (j[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<std::string>());
                }
                return vec;
            } else if (j[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<int>());
                }
                return vec;
            } else if (j[0].is_number_float()) {
                std::vector<double> vec;
                for (const auto& item : j) {
                    vec.push_back(item.get<double>());
                }
                return vec;
            }
            return std::vector<std::string>{};
        } else if (j.is_null()) {
            return std::string("");
        }
        return std::string("");
    }
    
    static std::string ConfigValueToString(const ConfigValue& value) {
        return std::visit([](const auto& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation for simple decimals
                std::stringstream ss;
                ss << std::fixed << std::setprecision(6) << val;
                return ss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                return val ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << val[i];
                }
                ss << "]";
                return ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << std::fixed << std::setprecision(6) << val[i];
                }
                ss << "]";
                return ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << val[i] << "\"";
                }
                ss << "]";
                return ss.str();
            }
            return "";
        }, value);
    }
};

// Simple YAML parser for basic key-value pairs
class YamlUtils {
public:
    static bool parseYamlLine(const std::string& line, std::string& key, std::string& value) {
        // Simple YAML parser for key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            key = trim(line.substr(0, colonPos));
            value = trim(line.substr(colonPos + 1));
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            return true;
        }
        return false;
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
    
    // Load from file (JSON or YAML format)
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
