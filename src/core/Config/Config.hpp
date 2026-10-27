#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <ctime>
#include <stdexcept>

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
    EnvironmentVariable
};

// Configuration validation error
struct ConfigValidationError : public std::runtime_error {
    std::string key;
    std::string expectedType;
    std::string actualType;
    std::string errorMessage;
    
    ConfigValidationError(const std::string& k, 
                         const std::string& expected,
                         const std::string& actual,
                         const std::string& msg)
        : std::runtime_error("Configuration error for key \"" + k + "\": " + msg),
          key(k), expectedType(expected), actualType(actual), errorMessage(msg) {}
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string expectedType;
    
    ConfigValidationRule(const std::string& k,
                         std::function<bool(const ConfigValue&)> val,
                         const std::string& msg,
                         const std::string& expected = "")
        : key(k), validator(val), errorMessage(msg), expectedType(expected) {}
};

// Configuration schema
class ConfigSchema {
public:
    struct ConfigEntry {
        std::string key;
        std::type_index type;
        ConfigSource source;
        std::any defaultValue;
        std::string description;
        std::function<bool(const std::any&)> validator;
        bool required;
        
        template<typename T>
        ConfigEntry(const std::string& k, T defaultVal, const std::string& desc = "",
                   std::function<bool(const T&)> val = nullptr, bool req = false)
            : key(k), type(typeid(T)), source(ConfigSource::Default),
              defaultValue(defaultVal), description(desc), 
              validator([val](const std::any& v) {
                  if (val && v.has_value()) {
                      return val(std::any_cast<T>(v));
                  }
                  return !req;  // If not required and no value, it's valid
              }), required(req) {}
    };
    
    ConfigSchema() = default;
    
    template<typename T>
    void add(const std::string& key, T defaultValue, 
             const std::string& description = "",
             std::function<bool(const T&)> validator = nullptr,
             bool required = false) {
        entries.emplace_back(key, defaultValue, description, validator, required);
    }
    
    std::vector<std::string> validate(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& entry : entries) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Required configuration key missing: " + entry.key);
                continue;
            }
            
            if (config.has(entry.key)) {
                auto value = config.getAny(entry.key);
                if (value.has_value()) {
                    // Check type
                    if (!std::holds_alternative<typename std::remove_reference<decltype(entry.defaultValue)>::type>(value.value())) {
                        std::string actualType = getTypeName(value.value());
                        errors.push_back("Invalid type for key \"" + entry.key + "\": expected " + 
                                       entry.expectedType + ", got " + actualType);
                        continue;
                    }
                    
                    // Check custom validator
                    if (entry.validator && !entry.validator(value.value())) {
                        errors.push_back("Validation failed for key \"" + entry.key + "\": " + 
                                       (entry.errorMessage.empty() ? "Invalid value" : entry.errorMessage));
                    }
                }
            }
        }
        
        return errors;
    }
    
    std::string getExpectedType(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) {
                return typeNameFromTypeIndex(entry.type);
            }
        }
        return "unknown";
    }
    
    bool hasKey(const std::string& key) const {
        for (const auto& entry : entries) {
            if (entry.key == key) {
                return true;
            }
        }
        return false;
    }
    
    std::string typeNameFromTypeIndex(const std::type_index& typeIndex) const {
        if (typeIndex == typeid(int)) return "int";
        if (typeIndex == typeid(int64_t)) return "int64_t";
        if (typeIndex == typeid(double)) return "double";
        if (typeIndex == typeid(bool)) return "bool";
        if (typeIndex == typeid(std::string)) return "string";
        if (typeIndex == typeid(std::vector<int>)) return "vector<int>";
        if (typeIndex == typeid(std::vector<double>)) return "vector<double>";
        if (typeIndex == typeid(std::vector<std::string>)) return "vector<string>";
        return "unknown";
    }
    
    std::string getTypeName(const ConfigValue& value) const {
        return std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) return "int";
            if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
            if constexpr (std::is_same_v<T, double>) return "double";
            if constexpr (std::is_same_v<T, bool>) return "bool";
            if constexpr (std::is_same_v<T, std::string>) return "string";
            if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
            if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
            if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
            return "unknown";
        }, value);
    }
    
private:
    std::vector<ConfigEntry> entries;
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
    
    // Load from file (JSON, YAML, TOML, or simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from TOML file
    bool loadFromTOML(const std::string& filepath);
    
    // Load from simple key=value format
    bool loadFromSimpleFormat(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    void loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Type-safe getters with default values
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Legacy set methods for backward compatibility
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
    
    // Get raw ConfigValue
    std::optional<ConfigValue> getAny(const std::string& key) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration against schema
    std::vector<std::string> validate(const ConfigSchema& schema) const;
    
    // Parse string to value
    template<typename T>
    static bool parseStringToValue(const std::string& str, T& outValue);
    
    // Convert value to string
    template<typename T>
    static std::string valueToString(const T& value);
    
    // Get configuration file extension
    static std::string getFileExtension(const std::string& filepath);
    
    // Set schema for validation
    void setSchema(const ConfigSchema& schema);
    
    // Check if configuration has schema
    bool hasSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractStringValue(const ConfigValue& value);
    static std::string extractVectorValue(const ConfigValue& value);
};

// Error types
class ConfigError : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        FILE_NOT_FOUND = 1,
        INVALID_FORMAT = 2,
        PARSING_ERROR = 3,
        VALIDATION_ERROR = 4,
        TYPE_MISMATCH = 5,
        DUPLICATE_KEY = 6
    };
    
    ConfigError(ErrorCode code, const std::string& message)
        : std::runtime_error(message), code_(code) {}
    
    ErrorCode code() const { return code_; }
    
private:
    ErrorCode code_;
};

} // namespace nlm
