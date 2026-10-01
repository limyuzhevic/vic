#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <limits>

namespace nlm {

// Custom exception classes for NLM
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class InvalidConfigValueException : public ConfigException {
public:
    InvalidConfigValueException(const std::string& key, const std::string& expectedType, const std::string& actualValue)
        : ConfigException("Invalid configuration value for key '" + key + "': expected " + expectedType + ", got '" + actualValue + "'") {}
};

class MissingConfigValueException : public ConfigException {
public:
    explicit MissingConfigValueException(const std::string& key)
        : ConfigException("Missing required configuration key: '" + key + "'") {}
};

class InvalidConfigFileException : public ConfigException {
public:
    explicit InvalidConfigFileException(const std::string& filepath, const std::string& reason)
        : ConfigException("Invalid configuration file '" + filepath + "': " + reason) {}
};

class InvalidArgumentException : public NLMException {
public:
    explicit InvalidArgumentException(const std::string& message) : NLMException("Invalid argument: " + message) {}
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

// Helper function to get type name as string
inline std::string getTypeName(const ConfigValue& value) {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, value);
}

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool validated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", bool validated = false)
        : key(k), value(v), source(s), description(desc), validated(validated) {}
};

// Configuration validation function template
inline bool validateConfigValue(const std::string& key, const ConfigValue& value, const std::string& expectedType = "") {
    try {
        // Check if the value type matches expected type
        if (!expectedType.empty()) {
            std::string actualType = getTypeName(value);
            if (expectedType != actualType && 
                !(expectedType == "string" && actualType == "string") &&
                !(expectedType == "int" && actualType == "int64_t") &&
                !(expectedType == "int64_t" && actualType == "int")) {
                throw InvalidConfigValueException(key, expectedType, actualType);
            }
        }
        
        // Type-specific validation
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                if (arg < std::numeric_limits<int>::min() || arg > std::numeric_limits<int>::max()) {
                    throw InvalidConfigValueException(key, "int within valid range", std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::isnan(arg) || std::isinf(arg)) {
                    throw InvalidConfigValueException(key, "finite double", std::to_string(arg));
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                // Boolean validation - always passes
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (arg.empty()) {
                    // Empty string might be valid for some configurations
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                for (const auto& item : arg) {
                    if (item < std::numeric_limits<int>::min() || item > std::numeric_limits<int>::max()) {
                        throw InvalidConfigValueException(key, "vector<int> with valid int values", "vector contains out-of-range int");
                    }
                }
            }
        }, value);
        
        return true;
    } catch (const ConfigException&) {
        throw;
    } catch (const std::exception& e) {
        throw InvalidConfigValueException(key, "valid value", e.what());
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
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, bool validate = true);
    
    // Get values (throw exceptions on missing/invalid)
    template<typename T>
    T getRequired(const std::string& key, const std::string& expectedType = "");
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
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
    
    // Get source as string
    static std::string sourceToString(ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isFileReadable(const std::string& filepath);
    static bool isFileWritable(const std::string& filepath);
    static std::string getFileExtension(const std::string& filename);
};

// Exception throwing helper functions
inline void throwIfMissing(const std::string& key, const std::optional<bool>& hasValue) {
    if (!hasValue.has_value() || !hasValue.value()) {
        throw MissingConfigValueException(key);
    }
}

inline void throwIfInvalid(const std::string& key, const std::optional<bool>& isValid) {
    if (!isValid.has_value() || !isValid.value()) {
        throw InvalidConfigValueException(key, "valid value", "validation failed");
    }
}

} // namespace nlm
