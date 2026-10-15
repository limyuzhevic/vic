// ConfigurationManager.hpp - Enhanced configuration management
#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // Load configuration from multiple sources
    bool loadConfiguration(const std::string& configFile = "configs/default.cfg",
                          const std::unordered_map<std::string, std::string>& cliOverrides = {});
    
    // Validate configuration
    bool validateConfiguration() const;
    
    // Get configuration value with type safety
    template<typename T>
    T getConfigValue(const std::string& key, const T& defaultValue = T()) const;
    
    // Get configuration value with fallback
    template<typename T>
    std::optional<T> getConfigValue(const std::string& key) const;
    
    // Set configuration value
    template<typename T>
    void setConfigValue(const std::string& key, const T& value,
                       ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration source
    ConfigSource getConfigSource(const std::string& key) const;
    
    // Get all keys with their sources
    std::unordered_map<std::string, std::pair<ConfigValue, ConfigSource>> getConfigSummary() const;
    
    // Get configuration file path
    std::string getConfigFilePath() const { return configFilePath_; }
    
    // Access internal config for compatibility
    std::shared_ptr<Config> getInternalConfig() { return config_; }
    
    // Check if configuration was loaded from file
    bool hasFileConfiguration() const { return hasFileConfig_; }
    
    // Get configuration validation errors
    const std::vector<std::string>& getValidationErrors() const { return validationErrors_; }
    
    // Clear configuration
    void clear();
    
    // Load default configuration
    void loadDefaults();
    
    // Load Phase 2 specific defaults
    void loadPhase2Defaults();
    
    // Load Phase 3 specific defaults  
    void loadPhase3Defaults();
    
    // Load Phase 4 specific defaults
    void loadPhase4Defaults();
    
    // Load Phase 5 specific defaults
    void loadPhase5Defaults();
    
    // Load Phase 6 specific defaults
    void loadPhase6Defaults();

private:
    std::unique_ptr<Config> config_;
    std::string configFilePath_;
    bool hasFileConfig_;
    std::vector<std::string> validationErrors_;
    
    // Validation methods
    bool validateGPUConfiguration() const;
    bool validateExperimentConfiguration() const;
    bool validateMemoryConfiguration() const;
    bool validateOutputConfiguration() const;
    
    // Default value setting methods
    void setDefaultValues();
};

} // namespace nlm
