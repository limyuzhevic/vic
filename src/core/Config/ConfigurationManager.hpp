#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <functional>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include "../core/Config/Config.hpp"

namespace nlm {

// Custom exception types for configuration errors
class ConfigValidationError : public std::runtime_error {
public:
    enum class ErrorType {
        INVALID_TYPE,
        MISSING_REQUIRED,
        OUT_OF_RANGE,
        DUPLICATE_KEY,
        INVALID_FORMAT,
        FILE_NOT_FOUND,
        PARSE_ERROR
    };

    ConfigValidationError(ErrorType type, const std::string& key, 
                         const std::string& message) 
        : std::runtime_error("Config error: " + message + " for key: " + key)
        , errorType(type), key(key), message(message) {}

    ErrorType getErrorType() const { return errorType; }
    const std::string& getKey() const { return key; }
    const std::string& getMessage() const { return message; }

private:
    ErrorType errorType;
    std::string key;
    std::string message;
};

class ConfigSchema {
public:
    struct Field {
        std::string name;
        std::string type; // "int", "double", "bool", "string", "vec"
        std::string defaultValue;
        std::string description;
        bool required;
        std::string validator; // Regex pattern, range, etc.
        std::vector<std::string> allowedValues;
        
        Field() : required(false) {}
        Field(const std::string& n, const std::string& t, const std::string& desc = "", 
              bool req = false, const std::string& val = "")
            : name(n), type(t), defaultValue(), description(desc), required(req), 
              validator(val) {}
    };

    ConfigSchema() = default;
    ~ConfigSchema() = default;
    
    void addField(const Field& field);
    bool validate(const Config& config) const;
    std::string getValidationError(const Config& config) const;
    const std::vector<Field>& getFields() const { return fields_; }
    const std::unordered_map<std::string, Field>& getFieldMap() const { return fieldMap_; }
    
    static ConfigSchema createNeuralSimulationSchema();
    static ConfigSchema createDefaultSchema();
    
private:
    std::vector<Field> fields_;
    std::unordered_map<std::string, Field> fieldMap_;
    
    std::string validateField(const Field& field, const std::string& value) const;
    bool validateType(const std::string& expectedType, const std::string& value) const;
};

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // Load configurations from multiple sources with priority
    bool loadConfiguration(const std::string& filepath = "configs/default.cfg");
    bool loadFromEnvironment();
    bool loadFromCommandLine(int argc, char** argv);
    
    // Get values with type safety and validation
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    template<typename T>
    std::optional<T> getOptional(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, int64_t value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Configuration validation
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const { return validationErrors_; }
    void clearValidationErrors() { validationErrors_.clear(); }
    
    // Configuration management
    bool has(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    std::string getSummary() const;
    void clear();
    
    // File operations
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema) { schema_ = schema; }
    const ConfigSchema& getSchema() const { return schema_; }
    
    // Configuration profiles
    bool loadProfile(const std::string& profileName);
    bool saveProfile(const std::string& profileName);
    std::vector<std::string> getAvailableProfiles() const;
    
    // Runtime configuration
    void enableDynamicUpdates(bool enabled = true);
    void setUpdateCallback(std::function<void(const std::string&, const ConfigValue&)> callback);
    
    static ConfigurationManager& getInstance();
    static void resetInstance();
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
    
    ConfigSchema schema_;
    std::vector<ConfigValidationError> validationErrors_;
    bool dynamicUpdatesEnabled_;
    std::function<void(const std::string&, const ConfigValue&)> updateCallback_;
    mutable std::mutex mutex_;
    
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool validateCurrent() const;
    ConfigValue convertValue(const std::string& type, const std::string& value) const;
    std::string serializeToJSON() const;
    std::string serializeToYAML() const;
};

// Convenience functions for common configuration operations
inline ConfigurationManager& Config() { return ConfigurationManager::getInstance(); }

// Configuration conversion utilities
namespace ConfigUtils {
    std::string toString(const ConfigValue& value);
    bool parseBool(const std::string& str);
    double parseDouble(const std::string& str);
    int64_t parseInt(const std::string& str);
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    bool isEmpty(const std::string& str);
}}

// Template implementations
namespace nlm {

template<typename T>
T ConfigurationManager::get(const std::string& key, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pImpl && pImpl->config.has(key)) {
        auto value = pImpl->config.getOptional<T>(key);
        if (value.has_value()) {
            return *value;
        }
    }
    return defaultValue;
}

template<typename T>
std::optional<T> ConfigurationManager::getOptional(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (pImpl && pImpl->config.has(key)) {
        return pImpl->config.getOptional<T>(key);
    }
    return std::nullopt;
}
}

// Initialize default schema when the header is included
inline namespace {
    const ConfigSchema& getDefaultConfigSchema() {
        static ConfigSchema schema = ConfigSchema::createDefaultSchema();
        return schema;
    }
}
