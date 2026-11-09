// Enhanced Configuration Manager
// Provides centralized configuration handling with presets, validation, and monitoring

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>
#include <vector>
#include <iostream>

namespace nlm {

// Configuration presets
enum class ConfigPreset {
    QuickTest,
    FullTest,
    Custom,
    Performance,
    Development,
    Production
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type;  // int, double, bool, string, vector<T>
    std::string description;
    bool required;
    std::string defaultValue;
    std::string validationRegex;
    std::vector<std::string> allowedValues;
    float minValue;
    float maxValue;
    bool sensitive;  // Password/API key
    
    ConfigSchema() : required(false), minValue(0.0f), maxValue(1e6f), sensitive(false) {}
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&)>;

// Real-time configuration monitoring
class ConfigMonitor {
public:
    struct ConfigChange {
        std::string key;
        ConfigValue oldValue;
        ConfigValue newValue;
        ConfigSource source;
        std::chrono::steady_clock::time_point timestamp;
        
        ConfigChange(const std::string& k, const ConfigValue& oldVal, 
                    const ConfigValue& newVal, ConfigSource src)
            : key(k), oldValue(oldVal), newValue(newVal), source(src),
              timestamp(std::chrono::steady_clock::now()) {}
    };
    
    using ChangeCallback = std::function<void(const ConfigChange&)>;
    
    void addChangeCallback(ChangeCallback callback);
    void removeChangeCallback(ChangeCallback callback);
    void clearChangeCallbacks();
    
    const std::vector<ConfigChange>& getChanges() const;
    void clearChanges();
    
    void logChange(const std::string& key, const ConfigValue& oldValue, 
                   const ConfigValue& newValue, ConfigSource source);
    
    void startMonitoring(double intervalSeconds = 1.0);
    void stopMonitoring();
    bool isMonitoring() const;
    
    std::string getMonitoringStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Configuration Manager with presets, validation, and monitoring
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Disable copying, enable moving
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    ConfigurationManager(ConfigurationManager&&) noexcept;
    ConfigurationManager& operator=(ConfigurationManager&&) noexcept;
    
    // Load configurations
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Apply presets
    bool applyPreset(ConfigPreset preset);
    bool savePreset(const std::string& name, const ConfigPreset preset);
    
    // Configuration validation
    ConfigValidationResult validate(const ConfigSchema& schema = ConfigSchema()) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get configuration with type safety
    template<typename T>
    T getWithValidation(const std::string& key, const T& defaultValue = T(), 
                       const std::string& errorContext = "") const;
    
    // Set configuration with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, 
                          ConfigSource source = ConfigSource::Runtime,
                          const std::string& errorContext = "");
    
    // Configuration monitoring
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
    // Preset management
    std::vector<std::string> getPresetNames() const;
    bool hasPreset(const std::string& name) const;
    ConfigPreset getPreset(const std::string& name) const;
    
    // Configuration summaries
    std::string getConfigurationSummary() const;
    std::string getPresetSummary(ConfigPreset preset) const;
    
    // Performance and monitoring
    void enableMonitoring(double intervalSeconds = 1.0);
    void disableMonitoring();
    bool isMonitoringEnabled() const;
    std::string getMonitoringStats() const;
    
    // Export/import
    bool exportToFile(const std::string& filepath, ConfigPreset preset = ConfigPreset::Custom) const;
    bool importFromFile(const std::string& filepath);
    
    // Statistics
    size_t getEntryCount() const;
    size_t getChangeCount() const;
    std::chrono::steady_clock::time_point getLastChangeTime() const;
    
    // Help and documentation
    void printHelp() const;
    void printConfigurationSummary() const;
    void printPresetHelp(ConfigPreset preset) const;
    
    // Configuration schemas for common use cases
    static ConfigSchema getBrainSchema();
    static ConfigSchema getEnvironmentSchema();
    static ConfigSchema getPerformanceSchema();
    static ConfigSchema getDebugSchema();
    
    // Convenience methods for common configurations
    bool configureQuickTest(int neuronCount = 500, int steps = 100);
    bool configureFullTest(int neuronCount = 1000, int steps = 1000);
    bool configurePerformance(int neuronCount = 2000, float connectionProb = 0.05f);
    bool configureDevelopment();
    bool configureProduction();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool validatePreset(const ConfigPreset preset) const;
    void setupPresetDefaults(ConfigPreset preset);
    std::string getConfigTypeName(const ConfigValue& value) const;
    bool matchesSchema(const std::string& key, const ConfigValue& value, 
                      const ConfigSchema& schema) const;
};

// Global configuration manager instance
ConfigurationManager& getConfigurationManager();

// Convenience functions
inline bool loadConfiguration(int argc, char** argv) {
    return getConfigurationManager().loadFromArgs(argc, argv);
}

inline bool loadConfigurationFromFile(const std::string& filepath) {
    return getConfigurationManager().loadFromFile(filepath);
}

inline bool applyConfigurationPreset(ConfigPreset preset) {
    return getConfigurationManager().applyPreset(preset);
}

inline void printConfigurationHelp() {
    getConfigurationManager().printHelp();
}

inline bool configureQuickTest(int neuronCount = 500, int steps = 100) {
    return getConfigurationManager().configureQuickTest(neuronCount, steps);
}

inline bool configureFullTest(int neuronCount = 1000, int steps = 1000) {
    return getConfigurationManager().configureFullTest(neuronCount, steps);
}

inline bool configurePerformance(int neuronCount = 2000, float connectionProb = 0.05f) {
    return getConfigurationManager().configurePerformance(neuronCount, connectionProb);
}

inline bool configureDevelopment() {
    return getConfigurationManager().configureDevelopment();
}

inline bool configureProduction() {
    return getConfigurationManager().configureProduction();
}

} // namespace nlm
