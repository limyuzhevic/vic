#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Configuration validation error types
enum class ConfigValidationError {
    None = 0,
    InvalidKey,
    InvalidType,
    OutOfRange,
    MissingRequired,
    DuplicateEntry
};

// Validation error with context
struct ValidationError {
    ConfigValidationError type;
    std::string key;
    std::string message;
    ValidationError() : type(ConfigValidationError::None), key(), message() {}
    ValidationError(ConfigValidationError t, const std::string& k, const std::string& m)
        : type(t), key(k), message(m) {}
};

// Configuration value types
using ConfigValue = std::variant<
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<std::string>
>;

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

// Get configuration value type as string
inline std::string getConfigValueTypeName(const ConfigValue& value) {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t>) return "int64";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return "int64_vector";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "double_vector";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "string_vector";
        else return "unknown";
    }, value);
}

// Helper function to get value from ConfigValue
inline bool getValueFromVariant(const ConfigValue& value, auto& output) {
    return std::visit([&](const auto& val) -> bool {
        output = val;
        return true;
    }, value);
}

// Type-safe getter implementations
inline bool getInt64(const ConfigValue& value, int64_t& output) {
    if (const auto* ptr = std::get_if<int64_t>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

inline bool getDouble(const ConfigValue& value, double& output) {
    if (const auto* ptr = std::get_if<double>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

inline bool getFloat(const ConfigValue& value, float& output) {
    if (const auto* ptr = std::get_if<double>(&value)) {
        output = static_cast<float>(*ptr);
        return true;
    }
    return false;
}

inline bool getBool(const ConfigValue& value, bool& output) {
    if (const auto* ptr = std::get_if<bool>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

inline bool getString(const ConfigValue& value, std::string& output) {
    if (const auto* ptr = std::get_if<std::string>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

// Vector type getters
inline bool getInt64Vector(const ConfigValue& value, std::vector<int64_t>& output) {
    if (const auto* ptr = std::get_if<std::vector<int64_t>>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

inline bool getDoubleVector(const ConfigValue& value, std::vector<double>& output) {
    if (const auto* ptr = std::get_if<std::vector<double>>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
}

inline bool getStringVector(const ConfigValue& value, std::vector<std::string>& output) {
    if (const auto* ptr = std::get_if<std::vector<std::string>>(&value)) {
        output = *ptr;
        return true;
    }
    return false;
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(const std::vector<std::string>& args);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Type-safe get methods with proper validation
    bool getInt(const std::string& key, int64_t& value) const;
    bool getDouble(const std::string& key, double& value) const;
    bool getFloat(const std::string& key, float& value) const;
    bool getBool(const std::string& key, bool& value) const;
    bool getString(const std::string& key, std::string& value) const;
    bool getStringVector(const std::string& key, std::vector<std::string>& value) const;
    bool getIntVector(const std::string& key, std::vector<int64_t>& value) const;
    bool getDoubleVector(const std::string& key, std::vector<double>& value) const;
    
    // Convenience template methods with type safety
    template<typename T>
    bool get(const std::string& key, T& value) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Enhanced set methods with validation
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Legacy compatibility methods
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Configuration validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Get all configuration entries
    std::vector<ConfigEntry> getAllEntries() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static std::string getTypeName(const ConfigValue& value);
};

} // namespace nlm
