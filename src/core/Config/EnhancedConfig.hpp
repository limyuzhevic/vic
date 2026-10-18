// Enhanced Configuration Class with Validation and Duplicate Cleanup
// 
// This enhanced version provides:
// 1. Comprehensive configuration validation
// 2. Automatic duplicate cleanup
// 3. Schema-based validation
// 4. Enhanced error handling and reporting
// 5. Configuration inheritance and scoping
// 6. Change notification patterns
// 7. Type safety and validation
// 8. Comprehensive documentation
// 9. Integration with experiment framework
// 10. Support for advanced configuration operations

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <functional>

namespace nlm {

// Enhanced configuration value types with validation
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

// Enhanced configuration source
enum class ConfigSource {
    Default,      // Default configuration values
    File,         // Loaded from file
    CommandLine,  // Loaded from command line
    Runtime,      // Set at runtime
    Inherited,    // Inherited from parent configuration
    Computed      // Computed value
};

// Configuration validation error types
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message, const std::string& key = "", 
                                  const ConfigSource& source = ConfigSource::Runtime)
        : std::runtime_error(message), key_(key), source_(source) {}
    
    std::string getKey() const { return key_; }
    ConfigSource getSource() const { return source_; }
    
private:
    std::string key_;
    ConfigSource source_;
};

class ConfigTypeError : public ConfigValidationError {
public:
    explicit ConfigTypeError(const std::string& key, const std::string& expectedType, const std::string& actualType)
        : ConfigValidationError("Type mismatch for key '" + key + "': expected " + expectedType + ", got " + actualType, key) {}
};

class ConfigRangeError : public ConfigValidationError {
public:
    explicit ConfigRangeError(const std::string& key, double value, double min, double max)
        : ConfigValidationError("Value for key '" + key + "' out of range: " + std::to_string(value) + 
                               ", must be between " + std::to_string(min) + " and " + std::to_string(max), key) {}
};

class ConfigDependencyError : public ConfigValidationError {
public:
    explicit ConfigDependencyError(const std::string& key, const std::string& missingDependency)
        : ConfigValidationError("Key '" + key + "' depends on missing key '" + missingDependency + "'", key) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::unordered_map<std::string, std::string> typeValidators;  // key -> validation function
    std::unordered_map<std::string, std::pair<double, double>> rangeValidators;  // key -> (min, max)
    std::unordered_set<std::string> requiredKeys;  // keys that must be present
    std::unordered_map<std::string, std::vector<std::string>> dependencies;  // key -> dependent keys
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> customValidators;
    
    // Add type validator
    void addTypeValidator(const std::string& key, const std::string& typeName) {
        typeValidators[key] = typeName;
    }
    
    // Add range validator for numeric types
    void addRangeValidator(const std::string& key, double min, double max) {
        rangeValidators[key] = std::make_pair(min, max);
    }
    
    // Add required key
    void addRequiredKey(const std::string& key) {
        requiredKeys.insert(key);
    }
    
    // Add dependency
    void addDependency(const std::string& key, const std::string& dependsOn) {
        dependencies[key].push_back(dependsOn);
    }
    
    // Add custom validator
    void addCustomValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
        customValidators[key] = validator;
    }
    
    // Validate configuration against schema
    bool validate(const std::unordered_map<std::string, ConfigEntry>& entries) const {
        // Check required keys
        for (const auto& key : requiredKeys) {
            if (entries.find(key) == entries.end()) {
                throw ConfigValidationError("Missing required key: '" + key + "'", key);
            }
        }
        
        // Check type validators
        for (const auto& [key, typeName] : typeValidators) {
            auto it = entries.find(key);
            if (it != entries.end()) {
                if (!validateType(it->second.value, typeName)) {
                    throw ConfigTypeError(key, typeName, getTypeName(it->second.value));
                }
            }
        }
        
        // Check range validators
        for (const auto& [key, range] : rangeValidators) {
            auto it = entries.find(key);
            if (it != entries.end()) {
                if (!validateRange(it->second.value, range.first, range.second)) {
                    throw ConfigRangeError(key, std::get<double>(it->second.value), range.first, range.second);
                }
            }
        }
        
        // Check dependencies
        for (const auto& [key, deps] : dependencies) {
            auto it = entries.find(key);
            if (it != entries.end()) {
                for (const auto& dep : deps) {
                    if (entries.find(dep) == entries.end()) {
                        throw ConfigDependencyError(key, dep);
                    }
                }
            }
        }
        
        // Check custom validators
        for (const auto& [key, validator] : customValidators) {
            auto it = entries.find(key);
            if (it != entries.end()) {
                if (!validator(it->second.value)) {
                    throw ConfigValidationError("Custom validation failed for key '" + key + "'", key);
                }
            }
        }
        
        return true;
    }
    
private:
    bool validateType(const ConfigValue& value, const std::string& typeName) const {
        switch (hash<std::string>{}(typeName)) {
            case hash<std::string>{}("int"):
                return std::holds_alternative<int>(value);
            case hash<std::string>{}("int64_t"):
                return std::holds_alternative<int64_t>(value);
            case hash<std::string>{}("double"):
                return std::holds_alternative<double>(value);
            case hash<std::string>{}("bool"):
                return std::holds_alternative<bool>(value);
            case hash<std::string>{}("string"):
                return std::holds_alternative<std::string>(value);
            case hash<std::string>{}("vector<int>"):
                return std::holds_alternative<std::vector<int>>(value);
            case hash<std::string>{}("vector<double>"):
                return std::holds_alternative<std::vector<double>>(value);
            case hash<std::string>{}("vector<string>"):
                return std::holds_alternative<std::vector<std::string>>(value);
            default:
                return true;  // Unknown type, skip validation
        }
    }
    
    bool validateRange(const ConfigValue& value, double min, double max) const {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        }
        return true;  // Non-numeric type, skip range validation
    }
    
    std::string getTypeName(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
};

// Configuration observer for change notification
class ConfigObserver {
public:
    virtual ~ConfigObserver() = default;
    virtual void onConfigChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) = 0;
    virtual void onConfigAdded(const std::string& key, const ConfigEntry& entry) = 0;
    virtual void onConfigRemoved(const std::string& key) = 0;
};

// Enhanced configuration class
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
    
    // Enhanced set with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration validation
    bool validate(const ConfigSchema& schema = ConfigSchema()) const;
    std::string getValidationErrors(const ConfigSchema& schema = ConfigSchema()) const;
    
    // Duplicate cleanup
    void cleanupDuplicates();
    
    // Configuration inheritance
    void inheritFrom(const Config& parentConfig, ConfigSource source = ConfigSource::Inherited);
    bool hasInheritance() const;
    
    // Observer support
    void addObserver(std::shared_ptr<ConfigObserver> observer);
    void removeObserver(std::shared_ptr<ConfigObserver> observer);
    void notifyObservers(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Configuration scoping
    void setScope(const std::string& scope);
    std::string getScope() const;
    bool hasScope(const std::string& scope) const;
    
    // Configuration export/import
    std::string exportToString() const;
    bool importFromString(const std::string& data);
    
    // Statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::unordered_map<ConfigSource, size_t> getSourceStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    
    // Validation helpers
    bool validateEntry(const ConfigEntry& entry) const;
    bool validateType(const ConfigValue& value, const std::string& typeName) const;
    bool validateRange(const ConfigValue& value, double min, double max) const;
    
    // Duplicate cleanup helpers
    void removeDuplicateEntries();
    void resolveConflicts(const std::vector<ConfigEntry>& entries);
};

// Config observer implementation for logging
class LoggingConfigObserver : public ConfigObserver {
public:
    void onConfigChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) override {
        std::cout << "Config changed: " << key << " = " << configValueToString(newValue) << std::endl;
    }
    
    void onConfigAdded(const std::string& key, const ConfigEntry& entry) override {
        std::cout << "Config added: " << key << " = " << configValueToString(entry.value) << " (source: " 
                  << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    void onConfigRemoved(const std::string& key) override {
        std::cout << "Config removed: " << key << std::endl;
    }
};

// Common configuration schemas
namespace ConfigSchemas {
    // Default NLM configuration schema
    ConfigSchema createDefaultNLMSchema();
    
    // Phase 2 configuration schema
    ConfigSchema createPhase2Schema();
    
    // Enhanced Phase 2 schema with validation
    ConfigSchema createEnhancedPhase2Schema();
    
    // Minimal schema for testing
    ConfigSchema createMinimalSchema();
}

} // namespace nlm