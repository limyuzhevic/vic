#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// Simplified configuration value types for common use cases
// This is the new simplified API - use this instead of the complex ConfigValue variant
struct SimpleConfigValue {
    // Store any basic type in a type-safe way
    enum class Type {
        Int,
        Int64,
        Double,
        Bool,
        String
    };
    
    Type type;
    union Value {
        int intVal;
        int64_t int64Val;
        double doubleVal;
        bool boolVal;
        std::string stringVal;
    };
    
    SimpleConfigValue() : type(Type::Int), intVal(0) {}
    SimpleConfigValue(int val) : type(Type::Int), intVal(val) {}
    SimpleConfigValue(int64_t val) : type(Type::Int64), int64Val(val) {}
    SimpleConfigValue(double val) : type(Type::Double), doubleVal(val) {}
    SimpleConfigValue(bool val) : type(Type::Bool), boolVal(val) {}
    SimpleConfigValue(const std::string& val) : type(Type::String), stringVal(val) {}
    
    // Type checking methods
    bool isInt() const { return type == Type::Int; }
    bool isInt64() const { return type == Type::Int64; }
    bool isDouble() const { return type == Type::Double; }
    bool isBool() const { return type == Type::Bool; }
    bool isString() const { return type == Type::String; }
    
    // Type conversion methods
    int asInt() const { return (type == Type::Int) ? Value::intVal : 0; }
    int64_t asInt64() const { return (type == Type::Int64) ? Value::int64Val : 0; }
    double asDouble() const { return (type == Type::Double) ? Value::doubleVal : 0.0; }
    bool asBool() const { return (type == Type::Bool) ? Value::boolVal : false; }
    std::string asString() const { return (type == Type::String) ? Value::stringVal : ""; }
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry for simple config
struct SimpleConfigEntry {
    std::string key;
    SimpleConfigValue value;
    ConfigSource source;
    std::string description;
    
    SimpleConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    SimpleConfigEntry(const std::string& k, const SimpleConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Simplified configuration class with easy-to-use API
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
    
    // Set values using simple types (new simplified API)
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values with simple API (new convenience methods)
    bool getBool(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Configuration factory methods (new convenient methods)
    static Config createBrainConfig();
    static Config createAdvancedConfig();
    static Config createDevelopmentConfig();
    
    // Configuration validation
    bool validate() const;
    
    // Merge two configurations
    void merge(const Config& other);
    
    // Backward compatibility - original complex API (keep for advanced users)
    using ConfigValue = std::variant<int, int64_t, double, bool, std::string>;
    
    // Original complex API methods (keep for backward compatibility)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper for simple config value conversion
    static SimpleConfigValue convertToSimple(const ConfigValue& value);
    static ConfigValue convertToComplex(const SimpleConfigValue& value);
};

// Convenience factory functions
inline Config createDefaultConfig() {
    return Config::createBrainConfig();
}

inline Config createBrainConfig() {
    return Config::createBrainConfig();
}

inline Config createAdvancedConfig() {
    return Config::createAdvancedConfig();
}

inline Config createDevelopmentConfig() {
    return Config::createDevelopmentConfig();
}

} // namespace nlm
