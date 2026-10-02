// Enhanced configuration management for NLM
// Provides better CLI options, validation, and utility functions

#pragma once

#include "Config.hpp"
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

class ConfigManager {
public:
    ConfigManager() = default;
    ~ConfigManager() = default;
    
    // Enhanced configuration loading with validation
    bool loadConfig(const std::string& configFile = "configs/default.cfg", 
                   int argc = 0, char** argv = nullptr);
    
    // Command line argument parsing with more options
    bool parseCommandLineArgs(int argc, char** argv);
    
    // Configuration validation
    bool validateConfig() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Get configuration value with type conversion and default
    template<typename T>
    T getConfig(const std::string& key, const T& defaultValue = T{}) const;
    
    // Set configuration value with validation
    template<typename T>
    bool setConfig(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration summary with detailed information
    std::string getConfigSummary() const;
    
    // Configuration file utilities
    bool saveConfig(const std::string& filepath) const;
    bool exportConfig(const std::string& filepath, bool includeComments = true) const;
    
    // Configuration comparison and diffing
    std::string diffConfigs(const ConfigManager& other) const;
    
    // Configuration backup and restore
    bool backupConfig(const std::string& backupDir = "./backups");
    bool restoreConfig(const std::string& backupFile);
    
    // Preset configurations
    bool loadPresetConfig(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Configuration monitoring
    void startConfigMonitoring(double intervalSeconds = 5.0);
    void stopConfigMonitoring();
    std::vector<std::string> getConfigChanges() const;
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t defaultEntries;
        double configLoadTimeMs;
        size_t memoryUsage;
    };
    
    ConfigStats getConfigStats() const;
    
    // Configuration template management
    bool addConfigTemplate(const std::string& name, const std::string& description,
                          const std::unordered_map<std::string, std::string>& defaults);
    bool applyConfigTemplate(const std::string& name);
    std::vector<std::string> getConfigTemplates() const;
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    void addValidationRule(const ValidationRule& rule);
    void clearValidationRules();
    
    // Configuration changes callback
    using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;
    
    void addConfigChangeCallback(const ConfigChangeCallback& callback);
    void removeConfigChangeCallback(const ConfigChangeCallback& callback);
    
    // Get the underlying config object
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Get the underlying config object (mutable)
    std::shared_ptr<Config> getConfig() { return config_; }
    
    // Clear all configuration
    void clearConfig() { if (config_) config_->clear(); }

private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> validationErrors_;
    std::vector<ValidationRule> validationRules_;
    std::vector<ConfigChangeCallback> changeCallbacks_;
    std::vector<std::string> configChanges_;
    ConfigStats stats_;
    std::thread monitoringThread_;
    bool monitoringActive_ = false;
    std::chrono::steady_clock::time_point lastChangeTime_;
    
    // Helper functions
    bool applyConfigTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& defaults);
    bool validateSingleConfigEntry(const ConfigEntry& entry) const;
    static bool isNumeric(const std::string& str);
    static bool isBoolean(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool isScientificNotation(const std::string& str);
    
    // Configuration presets
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> getDefaultPresets();
    
    // Change tracking
    void trackConfigChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Monitoring thread function
    void monitoringThreadFunc();
};

// Convenience functions for common configuration operations
namespace ConfigUtils {
    // Parse command line arguments with enhanced options
    struct CommandLineOptions {
        std::string configFile;
        std::string logLevel;
        bool verbose;
        bool debug;
        bool quiet;
        std::unordered_map<std::string, std::string> customOptions;
        std::vector<std::string> experimentArgs;
    };
    
    CommandLineOptions parseCommandLineArgs(int argc, char** argv);
    
    // Create configuration from command line options
    std::shared_ptr<Config> createConfigFromOptions(const CommandLineOptions& options);
    
    // Validate configuration for specific use cases
    bool validateExperimentConfig(const Config& config, const std::string& experimentType);
    bool validateSimulationConfig(const Config& config);
    bool validateVisualizationConfig(const Config& config);
    
    // Get configuration value with type safety
    template<typename T>
    T getRequiredConfig(const Config& config, const std::string& key);
    
    // Merge configurations
    void mergeConfigs(Config& target, const Config& source, ConfigSource sourcePriority = ConfigSource::File);
    
    // Convert configuration value to string
    std::string configValueToString(const ConfigValue& value);
    
    // Parse configuration string to value
    bool tryParseConfigValue(const std::string& str, ConfigValue& value);
}

// Enhanced configuration file format support
namespace ConfigFormat {
    // JSON configuration format
    bool loadJSONConfig(const std::string& filepath, std::shared_ptr<Config> config);
    bool saveJSONConfig(const std::string& filepath, const Config& config, bool prettyPrint = true);
    
    // YAML configuration format
    bool loadYAMLConfig(const std::string& filepath, std::shared_ptr<Config> config);
    bool saveYAMLConfig(const std::string& filepath, const Config& config);
    
    // TOML configuration format
    bool loadTOMLConfig(const std::string& filepath, std::shared_ptr<Config> config);
    bool saveTOMLConfig(const std::string& filepath, const Config& config);
    
    //INI configuration format
    bool loadINIConfig(const std::string& filepath, std::shared_ptr<Config> config);
    bool saveINIConfig(const std::string& filepath, const Config& config);
}

} // namespace nlm
