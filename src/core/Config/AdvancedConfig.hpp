#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <unordered_set>
#include "Config.hpp"

namespace nlm {

// Advanced configuration types
using ConfigSchema = std::variant<
    int, int64_t, double, bool, std::string,
    std::vector<int>, std::vector<double>, std::vector<std::string>
>;

using ConfigValidator = std::function<bool(const ConfigValue&)>;
using ConfigModifier = std::function<ConfigValue(const ConfigValue&)>;
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&, const ConfigValue&)>;

// Configuration metadata
struct ConfigMetadata {
    std::string description;
    std::string category;
    std::string unit;
    std::string validationPattern;
    std::string exampleValue;
    bool required;
    bool isReadOnly;
    bool isAdvanced;
    std::vector<std::string> dependencies;
    std::unordered_map<std::string, std::string> tags;
    double minValue;
    double maxValue;
    std::string validationErrorMessage;
    
    ConfigMetadata()
        : description(""), category("general"), unit(""), validationPattern(""), 
          exampleValue(""), required(false), isReadOnly(false), isAdvanced(false),
          minValue(0.0), maxValue(0.0) {}
};

// Environment configuration
struct EnvironmentConfig {
    std::string name;
    std::string description;
    std::unordered_map<std::string, ConfigValue> values;
    bool isProduction;
    bool isDevelopment;
    
    EnvironmentConfig() : isProduction(false), isDevelopment(false) {}
};

// Configuration change tracking
struct ConfigChange {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::chrono::system_clock::time_point timestamp;
    std::string userId;
    
    ConfigChange(const std::string& k, const ConfigValue& oldv, const ConfigValue& newv, ConfigSource s)
        : key(k), oldValue(oldv), newValue(newv), source(s) {
        timestamp = std::chrono::system_clock::now();
        userId = "system";
    }
};

// Advanced configuration class with validation, environments, and change tracking
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Configuration schema management
    void setSchema(const std::string& key, const ConfigSchema& schema, const ConfigMetadata& metadata = ConfigMetadata{});
    bool hasSchema(const std::string& key) const;
    ConfigSchema getSchema(const std::string& key) const;
    ConfigMetadata getMetadata(const std::string& key) const;
    std::vector<std::string> getSchemaKeys() const;
    
    // Enhanced validation
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::string validateAll() const;
    
    // Value conversion with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Environment management
    void addEnvironment(const EnvironmentConfig& env);
    bool setEnvironment(const std::string& envName);
    std::string getCurrentEnvironment() const;
    std::vector<std::string> getEnvironmentNames() const;
    EnvironmentConfig getEnvironment(const std::string& envName) const;
    
    // Configuration with fallback and inheritance
    std::optional<ConfigValue> getWithFallback(const std::string& key, const ConfigValue& fallback) const;
    void mergeConfig(const AdvancedConfig& otherConfig, ConfigSource source = ConfigSource::Runtime);
    
    // Change tracking
    void addChangeCallback(const ConfigChangeCallback& callback);
    std::vector<ConfigChange> getRecentChanges(size_t limit = 100) const;
    size_t getChangeCount() const;
    
    // Validation and enforcement
    void setValidator(const std::string& key, const ConfigValidator& validator);
    void setModifier(const std::string& key, const ConfigModifier& modifier);
    bool isValueValid(const std::string& key, const ConfigValue& value) const;
    ConfigValue applyModifier(const std::string& key, const ConfigValue& value) const;
    
    // Configuration operations
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get and cast operations with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Backward compatibility methods
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::string summary() const;
    
    // Configuration comparison and serialization
    bool operator==(const AdvancedConfig& other) const;
    bool operator!=(const AdvancedConfig& other) const;
    
    // Snapshot and restore
    std::string createSnapshot() const;
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // Export and import
    bool exportToJSON(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    
    // Performance optimization
    void optimizeAccess();
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalKeys;
        size_t validKeys;
        size_t invalidKeys;
        size_t readonlyKeys;
        double memoryUsage;
        double lastAccessTime;
    };
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    ConfigValue normalizeValue(const ConfigValue& value) const;
    bool validateType(const std::string& key, const ConfigValue& value) const;
    bool validateRange(const std::string& key, const ConfigValue& value) const;
    bool validatePattern(const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm
