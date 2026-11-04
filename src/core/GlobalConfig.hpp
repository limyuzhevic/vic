#pragma once

namespace nlm {

// Global configuration management for NLM
// This class provides a centralized way to manage configuration across all subsystems

class GlobalConfig {
public:
    GlobalConfig();
    ~GlobalConfig();
    
    // Disable copying
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    
    // Get global configuration instance
    static GlobalConfig& getInstance();
    
    // Configure all subsystems based on global settings
    void initializeAllSystems();
    
    // Update configuration and reinitialize affected subsystems
    void updateConfig(const std::string& key, const std::string& value);
    
    // Get configuration value
    template<typename T>
    T get(const std::string& key) const;
    
    // Check if configuration is valid
    bool validate() const;
    
    // Get configuration status
    std::string status() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration validation helpers
    bool validateBrainConfig() const;
    bool validatePlasticityConfig() const;
    bool validateMemoryConfig() const;
    bool validateNeuromodulationConfig() const;
    bool validatePredictionConfig() const;
    bool validateCognitionConfig() const;
    bool validateDevelopmentConfig() const;
};

} // namespace nlm