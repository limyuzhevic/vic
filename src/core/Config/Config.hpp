#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <unordered_set>
#include <functional>
#include <sstream>
#include <iomanip>
#include <regex>
#include <cstdint>

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

// Configuration file formats
enum class ConfigFormat {
    Simple,
    JSON,
    YAML
};

// Configuration validation error types
class ConfigValidationError : public std::runtime_error {
public:
    enum class ErrorType {
        InvalidType,
        MissingRequiredField,
        InvalidValue,
        DuplicateKey,
        FormatNotSupported,
        ParseError
    };
    
    ConfigValidationError(ErrorType type, const std::string& key, const std::string& message)
        : std::runtime_error(message), errorType(type), key(key) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getKey() const { return key; }
    
private:
    ErrorType errorType;
    std::string key;
};

class ConfigFileError : public std::runtime_error {
public:
    enum class ErrorType {
        FileNotFound,
        CannotOpen,
        ParseError,
        WriteError
    };
    
    ConfigFileError(ErrorType type, const std::string& filepath, const std::string& message)
        : std::runtime_error(message), errorType(type), filepath(filepath) {}
    
    ErrorType getErrorType() const { return errorType; }
    const std::string& getFilepath() const { return filepath; }
    
private:
    ErrorType errorType;
    std::string filepath;
};

// Configuration schema definition
struct ConfigSchema {
    std::map<std::string, std::string> typeMap;  // key -> type string
    std::unordered_set<std::string> requiredKeys;
    std::map<std::string, std::string> defaultValues;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    // Validate a single configuration entry
    bool validate(const std::string& key, const ConfigValue& value) const {
        auto it = validators.find(key);
        if (it != validators.end()) {
            return it->second(value);
        }
        return true;  // No validator found, assume valid
    }
    
    // Check if all required keys are present
    bool validateRequired(const std::map<std::string, ConfigValue>& values) const {
        for (const auto& requiredKey : requiredKeys) {
            if (values.find(requiredKey) == values.end()) {
                return false;
            }
        }
        return true;
    }
};

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

// JSON serialization helper classes
namespace json {
    inline std::string toJsonString(const std::string& str) {
        std::ostringstream oss;
        oss << std::quoted(str);
        return oss.str();
    }
    
    inline std::string toJsonArray(const std::vector<int>& arr) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << arr[i];
        }
        oss << "]";
        return oss.str();
    }
    
    inline std::string toJsonArray(const std::vector<double>& arr) {
        std::ostringstream oss;
        oss << std::setprecision(6) << std::fixed;
        oss << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << arr[i];
        }
        oss << "]";
        return oss.str();
    }
    
    inline std::string toJsonArray(const std::vector<std::string>& arr) {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << toJsonString(arr[i]);
        }
        oss << "]";
        return oss.str();
    }
    
    inline std::string configValueToJson(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << std::fixed << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << std::boolalpha << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << toJsonString(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << toJsonArray(arg);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << toJsonArray(arg);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << toJsonArray(arg);
            }
        }, value);
        return oss.str();
    }
}

// YAML serialization helper classes
namespace yaml {
    inline std::string toYamlString(const std::string& str) {
        if (str.find('\n') != std::string::npos || str.find(':') != std::string::npos) {
            std::ostringstream oss;
            oss << "|" << std::endl << str << std::endl;
            return oss.str();
        }
        return "\"" + str + "\"";
    }
    
    inline std::string configValueToYaml(const ConfigValue& value, bool isMultiline = false) {
        std::ostringstream oss;
        std::visit([&oss, isMultiline](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << std::fixed << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << std::boolalpha << arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (isMultiline) {
                    oss << "|" << std::endl << arg << std::endl;
                } else {
                    oss << "\"" << arg << "\"";
                }
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
                    oss << std::setprecision(6) << std::fixed << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            }
        }, value);
        return oss.str();
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
    
    // Load from file (auto-detects JSON, YAML, or simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with explicit format
    bool loadFromFile(const std::string& filepath, ConfigFormat format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::Simple) const;
    
    // Backward compatibility overload
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
    
    // JSON/YAML support
    bool loadFromJson(const std::string& filepath);
    bool saveToJson(const std::string& filepath) const;
    bool loadFromYaml(const std::string& filepath);
    bool saveToYaml(const std::string& filepath) const;
    
    // Format detection and validation
    static ConfigFormat detectFormat(const std::string& filepath);
    static bool isValidJson(const std::string& content);
    static bool isValidYaml(const std::string& content);
    
    // Schema validation
    void setSchema(const ConfigSchema& schema);
    std::vector<ConfigValidationError> validate() const;
    
    // Backward compatibility
    static bool isSimpleFormat(const std::string& content);
    
    // Get file format from filepath
    static ConfigFormat getFormatFromExtension(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing helpers
    static bool parseJson(const std::string& content, std::map<std::string, ConfigValue>& out);
    static bool parseYaml(const std::string& content, std::map<std::string, ConfigValue>& out);
    static bool parseSimpleFormat(const std::string& content, std::map<std::string, ConfigValue>& out);
    
    static std::string serializeToJson(const std::map<std::string, ConfigValue>& data);
    static std::string serializeToYaml(const std::map<std::string, ConfigValue>& data);
    static std::string serializeToSimple(const std::map<std::string, ConfigValue>& data);
    
    // Type conversion helpers
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    
    ConfigSchema schema_;
};
