#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <variant>
#include <optional>
#include <algorithm>
#include <chrono>
#include <ctime>
#include "Logger/Logger.hpp"

namespace nlm {

// Configuration types for better type safety
enum class ConfigType {
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    DOUBLE,
    VECTOR_FLOAT
};

// Configuration value that can hold different types safely
class ConfigValue {
public:
    ConfigValue() = default;
    
    // Constructors for different types
    ConfigValue(int64_t value) : type_(ConfigType::INTEGER), intValue_(value) {}
    ConfigValue(float value) : type_(ConfigType::FLOAT), floatValue_(value) {}
    ConfigValue(double value) : type_(ConfigType::DOUBLE), doubleValue_(value) {}
    ConfigValue(const std::string& value) : type_(ConfigType::STRING), stringValue_(value) {}
    ConfigValue(bool value) : type_(ConfigType::BOOLEAN), boolValue_(value) {}
    ConfigValue(const std::vector<float>& value) : type_(ConfigType::VECTOR_FLOAT), vectorFloatValue_(value) {}
    
    // Getters with type checking
    bool isInt() const { return type_ == ConfigType::INTEGER; }
    bool isFloat() const { return type_ == ConfigType::FLOAT; }
    bool isDouble() const { return type_ == ConfigType::DOUBLE; }
    bool isString() const { return type_ == ConfigType::STRING; }
    bool isBool() const { return type_ == ConfigType::BOOLEAN; }
    bool isVectorFloat() const { return type_ == ConfigType::VECTOR_FLOAT; }
    
    // Type-safe value access
    int64_t asInt() const { return intValue_; }
    float asFloat() const { return floatValue_; }
    double asDouble() const { return doubleValue_; }
    const std::string& asString() const { return stringValue_; }
    bool asBool() const { return boolValue_; }
    const std::vector<float>& asVectorFloat() const { return vectorFloatValue_; }
    
    // Validation
    bool isValid() const {
        switch (type_) {
            case ConfigType::INTEGER: return true;
            case ConfigType::FLOAT: return !std::isnan(floatValue_) && !std::isinf(floatValue_);
            case ConfigType::DOUBLE: return !std::isnan(doubleValue_) && !std::isinf(doubleValue_);
            case ConfigType::STRING: return true;
            case ConfigType::BOOLEAN: return true;
            case ConfigType::VECTOR_FLOAT: 
                for (float v : vectorFloatValue_) {
                    if (std::isnan(v) || std::isinf(v)) return false;
                }
                return true;
            default: return false;
        }
    }
    
    // Get type name for debugging
    std::string typeName() const {
        switch (type_) {
            case ConfigType::INTEGER: return "INTEGER";
            case ConfigType::FLOAT: return "FLOAT";
            case ConfigType::DOUBLE: return "DOUBLE";
            case ConfigType::STRING: return "STRING";
            case ConfigType::BOOLEAN: return "BOOLEAN";
            case ConfigType::VECTOR_FLOAT: return "VECTOR_FLOAT";
            default: return "UNKNOWN";
        }
    }
    
private:
    ConfigType type_;
    union {
        int64_t intValue_;
        float floatValue_;
        double doubleValue_;
        std::string stringValue_;
        bool boolValue_;
        std::vector<float> vectorFloatValue_;
    };
};

// Configuration source tracking
enum class ConfigSource {
    DEFAULT,
    CONFIG_FILE,
    COMMAND_LINE,
    PROGRAMMATIC
};

// Configuration entry with source tracking
struct ConfigEntry {
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : source(ConfigSource::DEFAULT), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const ConfigValue& val, ConfigSource src, const std::string& desc = "")
        : value(val), source(src), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced configuration system with validation and change tracking
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration loading
    bool loadFromFile(const std::string& filename);
    bool loadFromArgs(int argc, char** argv);
    
    // Configuration setting with validation
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    bool set(const std::string& key, float value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    bool set(const std::string& key, const std::vector<float>& value, ConfigSource source = ConfigSource::PROGRAMMATIC);
    
    // Enhanced getter with type checking and defaults
    bool exists(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Type-safe accessors with validation and defaults
    int64_t getOr(const std::string& key, int64_t defaultValue) const;
    float getOr(const std::string& key, float defaultValue) const;
    double getOr(const std::string& key, double defaultValue) const;
    const std::string& getOr(const std::string& key, const std::string& defaultValue) const;
    bool getOr(const std::string& key, bool defaultValue) const;
    const std::vector<float>& getOr(const std::string& key, const std::vector<float>& defaultValue) const;
    
    // Type-specific accessors with validation
    int64_t getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    double getDouble(const std::string& key) const;
    const std::string& getString(const std::string& key) const;
    bool getBool(const std::string& key) const;
    const std::vector<float>& getVectorFloat(const std::string& key) const;
    
    // Configuration validation and consistency checks
    bool validate() const;
    std::string validateReport() const;
    
    // Configuration introspection
    std::vector<std::string> getAllKeys() const;
    std::vector<std::string> getKeysOfType(ConfigType type) const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Configuration management
    void clear();
    bool remove(const std::string& key);
    bool hasChanged(const std::string& key) const;
    
    // Configuration export/import
    std::string toJSON() const;
    bool fromJSON(const std::string& json);
    
    // Statistics and monitoring
    double getValidationScore() const;
    std::chrono::system_clock::time_point getLastModified() const;
    
    // Logging
    void logConfiguration() const;
    void logConfigurationChanges() const;
    
    // Convenience methods for common configuration patterns
    void configureForPhase(int phase);
    void configureForPerformance();
    void configureForLearning();
    void configureForDevelopment();
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Test interface compatibility
    bool has(const std::string& key) const;
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Legacy compatibility methods
    bool isSet(const std::string& key) const { return has(key); }
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const;
    int64_t getIntOr(const std::string& key, int64_t defaultValue) const;
    float getFloatOr(const std::string& key, float defaultValue) const;

private:
    // Internal storage
    std::unordered_map<std::string, ConfigEntry> config_;
    
    // Validation and type checking
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateType(const std::string& key, ConfigType expectedType) const;
    
    // Key normalization
    std::string normalizeKey(const std::string& key) const;
    
    // Default values
    static std::unordered_map<std::string, ConfigValue> getDefaultValues();
    
    // Command line argument parsing
    bool parseCommandLineArgs(int argc, char** argv);
    bool parseConfigLine(const std::string& line);
    
    // Configuration file parsing
    bool parseConfigFile(const std::string& filename);
    bool parseJSONConfig(const std::string& json);
    
    // Statistics
    mutable size_t validationCount_;
    mutable size_t errorCount_;
    mutable std::chrono::system_clock::time_point lastModified_;
    
    // Configuration change tracking
    std::unordered_map<std::string, ConfigValue> previousValues_;
    bool hasConfigurationChanged() const;
};