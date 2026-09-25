#ifndef NLM_CONFIG_MANAGER_HPP
#define NLM_CONFIG_MANAGER_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <mutex>

namespace nlm {

// Configuration parameter definitions with metadata
struct ConfigParameter {
    std::string name;
    std::string description;
    ConfigValue defaultValue;
    ConfigSource defaultSource;
    bool required;
    std::string validationRegex;
    double minValue;
    double maxValue;
    std::vector<std::string> categories;
    
    ConfigParameter() : defaultSource(ConfigSource::Default), required(false), 
                       minValue(std::numeric_limits<double>::lowest()),
                       maxValue(std::numeric_limits<double>::max()) {}
};

// Configuration categories for organization
enum class ConfigCategory {
    Simulation,
    Network,
    Plasticity,
    Development,
    Neuromodulation,
    Memory,
    Cognition,
    Environment,
    Logging,
    Performance,
    Testing,
    Advanced
};

// Enhanced Configuration Manager with validation, hot-reloading, and validation
class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    // Initialize configuration with parameter definitions
    void initialize();
    
    // Load from multiple sources (priority order: command-line > env > file > default)
    bool loadConfig(const std::string& configFile = "configs/default.cfg");
    
    // Environment variable overrides
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Command line argument parsing
    void loadFromCommandLine(int argc, char** argv);
    
    // Configuration validation
    bool validateConfig() const;
    std::string getValidationErrors() const;
    
    // Configuration parameter management
    template<typename T>
    void registerParameter(const std::string& name, const T& defaultValue,
                          const std::string& description = "",
                          ConfigCategory category = ConfigCategory::Advanced,
                          bool required = false);
    
    // Get configuration values with validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key, const T& defaultValue = T()) const;
    
    // Configuration hot-reloading
    bool reloadConfig();
    bool hasConfigChanged() const;
    
    // Configuration persistence
    bool saveConfig(const std::string& filepath) const;
    bool generateConfigTemplate(const std::string& filepath) const;
    
    // Configuration summary and documentation
    std::string getConfigSummary() const;
    std::string getConfigDocumentation() const;
    std::vector<std::string> getCategories() const;
    std::vector<std::string> getParametersInCategory(ConfigCategory category) const;
    
    // Backward compatibility methods
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto val = config_->getValidated<T>(key, defaultValue);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Config access
    std::shared_ptr<Config> getConfig() const { return config_; }
    
private:
    ConfigManager() : config_(std::make_shared<Config>()), lastLoadTime(std::chrono::steady_clock::now()),
                     configChanged(false), validationErrors(""), initialized(false) {}
    ~ConfigManager() = default;
    
    // Internal helper methods
    std::string normalizeKey(const std::string& key) const;
    bool validateParameterType(const std::string& key, const ConfigValue& value) const;
    bool validateParameterRange(const std::string& key, const ConfigValue& value) const;
    std::string valueToString(const ConfigValue& value) const;
    
    std::shared_ptr<Config> config_;
    std::unordered_map<std::string, ConfigParameter> parameterDefinitions_;
    std::chrono::steady_clock::time_point lastLoadTime;
    bool configChanged;
    mutable std::mutex configMutex_;
    std::string validationErrors;
    bool initialized;
};

// Global convenience functions
inline ConfigManager& Config() { return ConfigManager::getInstance(); }

} // namespace nlm

#endif // NLM_CONFIG_MANAGER_HPP