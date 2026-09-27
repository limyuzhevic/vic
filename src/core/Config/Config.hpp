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
    
    // Get string representation of the value for file serialization
    std::optional<std::string> getValueString() const {
        std::string result;
        std::visit([&result](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Convert vector of strings to a format that can be easily parsed back
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += "\"" + arg[i] + "\"";
                }
                result += "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(arg[i]);
                }
                result += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(arg[i]);
                }
                result += "]";
            } else {
                // For basic types (int, int64_t, double, bool)
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, double> || std::is_same_v<T, bool>) {
                    result = std::to_string(arg);
                }
            }
        }, value);
        
        return result.empty() ? std::nullopt : std::optional<std::string>(result);
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
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
