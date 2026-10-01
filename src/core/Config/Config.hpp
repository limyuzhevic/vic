#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <string>

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
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Type checking methods
    bool isString() const {
        return std::holds_alternative<std::string>(value);
    }
    
    bool isNumber() const {
        return std::holds_alternative<int>(value) || 
               std::holds_alternative<int64_t>(value) ||
               std::holds_alternative<double>(value);
    }
    
    bool isBoolean() const {
        return std::holds_alternative<bool>(value);
    }
    
    bool isVector() const {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    // Helper methods for common types
    std::string asString() const {
        if (isString()) {
            return std::get<std::string>(value);
        }
        throw std::runtime_error("ConfigEntry does not contain a string");
    }
    
    int asInt() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<int>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int>(std::get<double>(value));
        }
        throw std::runtime_error("ConfigEntry does not contain an integer");
    }
    
    int64_t asInt64() const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int64_t>(std::get<double>(value));
        }
        throw std::runtime_error("ConfigEntry does not contain a 64-bit integer");
    }
    
    double asDouble() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        throw std::runtime_error("ConfigEntry does not contain a double");
    }
    
    bool asBoolean() const {
        if (isBoolean()) {
            return std::get<bool>(value);
        }
        throw std::runtime_error("ConfigEntry does not contain a boolean");
    }
    
    std::string toString() const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::setprecision(6) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << '"' << arg[i] << '"';
                }
                oss << "]";
            }
        }, value);
        return oss.str();
    }
    
    // Validation helper
    bool validate(const std::string& key) const {
        if (key.empty()) return false;
        
        // Basic validation based on key prefix (common patterns)
        if (key.find("count") != std::string::npos || 
            key.find("num") != std::string::npos ||
            key.find("size") != std::string::npos) {
            if (!isNumber()) {
                return false;
            }
        } else if (key.find("rate") != std::string::npos || 
                   key.find("ratio") != std::string::npos ||
                   key.find("probability") != std::string::npos) {
            if (!isNumber()) {
                return false;
            }
            double val = asDouble();
            if (val < 0.0 || val > 1.0) {
                return false;
            }
        } else if (key.find("weight") != std::string::npos || 
                   key.find("threshold") != std::string::npos) {
            if (!isNumber()) {
                return false;
            }
        } else if (key.find("enabled") != std::string::npos || 
                   key.find("enable") != std::string::npos) {
            if (!isBoolean()) {
                return false;
            }
        }
        return true;
    }
};

// Main configuration class with enhanced functionality
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced loading methods
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& jsonContent);
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced saving methods
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with better type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type detection
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Configuration introspection
    std::string summary() const;
    
    // Enhanced utility methods
    bool validateConfig() const;
    std::string printConfig() const;
    static std::string getDefaultConfigPath();
    static bool isValidKey(const std::string& key);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    
    // Configuration utilities
    std::string generateReport() const;
    
private:
    bool trySetFromString(const std::string& key, const std::string& valueStr);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key, char invalidChar);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
