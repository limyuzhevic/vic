// Enhanced Configuration System for NLM
// ====================================
// 
// This header defines an enhanced configuration system that provides:
// - Centralized parameter schema with validation
// - Parameter ranges and constraints
// - Magic number consolidation
// - Enhanced error handling
// - Performance optimizations

#pragma once

#include "Config.hpp"
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <limits>

namespace nlm {

// Enhanced configuration parameter definition
struct ConfigParameter {
    std::string key;                    // Parameter name
    std::string description;            // Human-readable description
    ConfigValue defaultValue;           // Default value
    ConfigSource defaultSource;         // Source of default value
    std::string typeName;               // Type name for documentation
    
    // Validation constraints
    std::optional<int64_t> intMin;      // Minimum integer value
    std::optional<int64_t> intMax;      // Maximum integer value
    std::optional<double> doubleMin;    // Minimum double value
    std::optional<double> doubleMax;    // Maximum double value
    std::optional<double> floatMin;     // Minimum float value
    std::optional<double> floatMax;     // Maximum float value
    std::optional<size_t> vectorMinSize; // Minimum vector size
    std::optional<size_t> vectorMaxSize; // Maximum vector size
    std::vector<std::string> validStrings; // Valid string values
    
    // Configuration dependencies
    std::vector<std::string> dependsOn;    // Other parameters this depends on
    std::function<bool(const ConfigValue&, const Config&)> dependencyValidator;
    
    // Custom validation function
    std::function<bool(const ConfigValue&)> customValidator;
    
    ConfigParameter() : defaultSource(ConfigSource::Default) {}
};

// Enhanced Configuration class
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Initialize with predefined schema
    void initializeSchema();
    
    // Load configuration from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromJSON(const std::string& jsonContent);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    std::string toJSON() const;
    
    // Enhanced parameter access with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Enhanced parameter setting with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Parameter information and validation
    bool has(const std::string& key) const;
    std::string getTypeName(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    std::optional<std::string> getValidationError(const std::string& key) const;
    bool validate(const std::string& key) const;
    bool validateAll() const;
    
    // Configuration presets
    bool loadPreset(const std::string& presetName);
    bool savePreset(const std::string& presetName) const;
    std::vector<std::string> getAvailablePresets() const;
    
    // Configuration management
    void remove(const std::string& key);
    void clear();
    void clearValidationCache();
    
    // Configuration summary with validation status
    std::string summary() const;
    std::string validationSummary() const;
    
    // Parameter range queries
    template<typename T>
    std::pair<T, T> getRange(const std::string& key) const;
    
    // Dependency checking
    bool checkDependencies(const std::string& key) const;
    std::vector<std::string> getDependencies(const std::string& key) const;
    
    // Configuration migration
    bool migrateFromLegacy(const Config& legacyConfig);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema management
    std::map<std::string, ConfigParameter> schema_;
    
    // Validation cache
    mutable std::map<std::string, bool> validationCache_;
    mutable std::map<std::string, std::string> validationErrors_;
    
    // Internal helpers
    bool validateValue(const ConfigValue& value, const std::string& key) const;
    std::string validateValueType(const ConfigValue& value, const std::string& expectedType) const;
    bool validateRange(const ConfigValue& value, const ConfigParameter& param) const;
    bool validateDependencies(const std::string& key) const;
    
    // Parameter normalization
    ConfigValue normalizeValue(const ConfigValue& value, const std::string& key) const;
    
    // Error handling
    void addValidationError(const std::string& key, const std::string& error) const;
    
    // Schema initialization helper
    void addParameter(const std::string& key,
                     const std::string& description,
                     const ConfigValue& defaultValue,
                     ConfigSource source = ConfigSource::Default);
};

// Convenience functions
inline EnhancedConfig createDefaultConfig() {
    EnhancedConfig config;
    config.initializeSchema();
    config.loadPreset("default");
    return config;
}

inline EnhancedConfig createSmallConfig() {
    EnhancedConfig config;
    config.initializeSchema();
    config.loadPreset("small");
    return config;
}

inline EnhancedConfig createLargeConfig() {
    EnhancedConfig config;
    config.initializeSchema();
    config.loadPreset("large");
    return config;
}