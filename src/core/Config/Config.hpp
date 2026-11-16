#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <any>
#include <type_traits>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Custom exception for configuration errors
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error("ConfigError: " + msg) {}
};

// Configuration validation levels
enum class ValidationLevel {
    None,           // No validation
    Basic,          // Basic type validation
    Strict,         // Strict validation with constraints
    Schema         // Full schema validation
};

// Configuration value types (extended)
class ConfigTypes {
public:
    // Basic types
    using Int = int;
    using Int64 = int64_t;
    using Float = double;
    using Bool = bool;
    using String = std::string;
    
    // Collections
    template<typename T>
    using Vector = std::vector<T>;
    using IntVector = Vector<Int>;
    using FloatVector = Vector<Float>;
    using StringVector = Vector<String>;
    
    // Complex types
    using Object = std::map<std::string, std::any>;
    using Array = std::vector<std::any>;
    
    // Special types
    struct Duration {
        double seconds;
        Duration(double s = 0.0) : seconds(s) {}
        
        bool operator==(const Duration& other) const { return seconds == other.seconds; }
        bool operator!=(const Duration& other) const { return !(*this == other); }
    };
    
    struct Percentage {
        float value; // 0.0 to 1.0
        Percentage(float v = 0.0f) : value(std::max(0.0f, std::min(1.0f, v)) {}
        
        bool operator==(const Percentage& other) const { return value == other.value; }
        bool operator!=(const Percentage& other) const { return !(*this == other); }
    };
    
    struct Probability {
        float value; // 0.0 to 1.0
        Probability(float v = 0.0f) : value(std::max(0.0f, std::min(1.0f, v)) {}
        
        bool operator==(const Percentage& other) const { return value == other.value; }
        bool operator!=(const Percentage& other) const { return !(*this == other); }
    };
    
    // Use cases
    using TimeStep = Duration;
    using LearningRate = Probability;
    using ConnectionProbability = Probability;
    using InhibitionStrength = Percentage;
    using NoveltyThreshold = Percentage;
};

// Configuration schema definition
class ConfigSchema {
public:
    struct Field {
        std::string key;
        std::any defaultValue;
        std::string description;
        bool required;
        std::string validationPattern;
        ValidationLevel validationLevel;
        std::vector<std::string> allowedValues;
        std::any minValue;
        std::any maxValue;
        
        Field(const std::string& k, const std::any& defaultVal, const std::string& desc = "",
              bool req = false, ValidationLevel level = ValidationLevel::None)
            : key(k), defaultValue(defaultVal), description(desc), required(req), validationLevel(level) {}
    };
    
    std::vector<Field> fields;
    
    void addField(const std::string& key, const std::any& defaultValue, 
                  const std::string& description = "", bool required = false,
                  ValidationLevel validationLevel = ValidationLevel::None) {
        fields.emplace_back(key, defaultValue, description, required, validationLevel);
    }
    
    // Validate a configuration against the schema
    bool validate(const std::map<std::string, std::any>& config) const {
        for (const auto& field : fields) {
            auto it = config.find(field.key);
            if (it == config.end()) {
                if (field.required) {
                    return false;
                }
                continue;
            }
            
            // Type validation would go here
            // Range validation would go here
            // Pattern validation would go here
        }
        return true;
    }
    
    // Get all field keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& field : fields) {
            keys.push_back(field.key);
        }
        return keys;
    }
};

// Main configuration class (enhanced)
class Config {
public:
    // Configuration sources
    enum class Source {
        Default,
        File,
        CommandLine,
        Runtime,
        Environment
    };
    
    // Configuration entry
    struct Entry {
        std::string key;
        std::any value;
        Source source;
        std::string description;
        mutable bool validated = false;
        
        Entry() : key(), value(), source(Source::Default), description() {}
        
        Entry(const std::string& k, const std::any& v, Source s, const std::string& desc = "")
            : key(k), value(v), source(s), description(desc), validated(false) {}
        
        // Helper methods for common types
        template<typename T>
        T get() const {
            if constexpr (std::is_same_v<T, int>) {
                return std::any_cast<int>(value);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::any_cast<int64_t>(value);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::any_cast<double>(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                return std::any_cast<bool>(value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::any_cast<std::string>(value);
            } else {
                throw ConfigError("Type not supported for Config::get()");
            }
        }
        
        template<typename T>
        std::optional<T> tryGet() const {
            try {
                return get<T>();
            } catch (const std::bad_any_cast&) {
                return std::nullopt;
            }
        }
        
        bool isValid() const { return validated; }
        void markValidated() const { validated = true; }
    };
    
    // Default constructor with schema
    Config(const ConfigSchema& schema = ConfigSchema());
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format) with validation
    bool loadFromFile(const std::string& filepath, ValidationLevel validationLevel = ValidationLevel::Strict);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString, ValidationLevel validationLevel = ValidationLevel::Strict);
    
    // Load from existing Config object
    bool loadFromConfig(const Config& other);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = findEntry(key);
        if (it == entries.end()) {
            return std::nullopt;
        }
        
        try {
            T value = std::any_cast<T>(it->value);
            return value;
        } catch (const std::bad_any_cast&) {
            return std::nullopt;
        }
    }
    
    // Get with default
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto value = get<T>(key);
        if (value) {
            return *value;
        }
        return defaultValue;
    }
    
    // Set values with type safety
    template<typename T>
    void set(const std::string& key, const T& value, Source source = Source::Runtime,
             const std::string& description = "");
    
    // Set with automatic type detection
    void set(const std::string& key, const std::any& value, Source source = Source::Runtime,
             const std::string& description = "");
    
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
    
    // Validate configuration
    bool validate(ValidationLevel level = ValidationLevel::Strict) const;
    
    // Merge configurations
    void merge(const Config& other);
    
    // Clone configuration
    Config clone() const;
    
    // Environment variable support
    static std::map<std::string, std::string> loadEnvironmentVariables(const std::string& prefix = "NLM_");
    
    // Command line argument support
    bool loadFromCommandLine(int argc, char** argv, const std::string& prefix = "--nlm-");
    
    // Get configuration schema
    const ConfigSchema& getSchema() const { return schema; }
    
    // Set validation level
    void setValidationLevel(ValidationLevel level) { validationLevel = level; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    ConfigSchema schema;
    ValidationLevel validationLevel = ValidationLevel::Basic;
    
    // Internal helpers
    Entry* findEntry(const std::string& key);
    const Entry* findEntry(const std::string& key) const;
    
    // JSON conversion
    static std::any jsonToAny(const nlohmann::json& json);
    static nlohmann::json anyToJson(const std::any& value);
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringMatch(const std::string& str, const std::string& pattern);
};

} // namespace nlm
