// ExtendedConfig.hpp - Extended configuration options for NLM
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>

#include "core/Config/Config.hpp"

namespace nlm {

/**
 * ExtendedConfig class - Extended configuration options for NLM
 * 
 * @brief Provides comprehensive extended configuration options including:
 *        - Extended plasticity parameters (STDP, Hebbian, structural)
 *        - Memory system configuration
 *        - Neuromodulation settings
 *        - Development stage configuration
 *        - Checkpoint and persistence options
 *        - Error handling and recovery settings
 *        - Performance optimization settings
 * 
 * Features:
 * - Extended plasticity rules (STDP, Hebbian, structural)
 * - Memory system capacity and management
 * - Neuromodulation (curiosity, reward, prediction error)
 * - Development stages (1-6) with stage-specific settings
 * - Checkpoint and persistence options
 * - Error handling and recovery options
 * - Performance optimization settings
 * - Validation and verification options
 * - Debug and testing options
 * 
 * @note Integrates with existing Config system for configuration management
 * @see Config for basic configuration options
 * @see AdvancedArgs for command-line configuration
 * @see CLIEngine for configuration integration
 */
class ExtendedConfig {
public:
    /**
     * @brief ExtendedConfig constructor
     * @details Initializes extended configuration with default settings
     * @post Configuration ready for extended options
     */
    ExtendedConfig();
    
    /**
     * @brief ExtendedConfig destructor
     * @details Cleans up extended configuration resources
     * @post All resources released
     */
    ~ExtendedConfig();
    
    /**
     * @brief Initialize default extended configuration
     * @post Default values set
     * 
     * @details Sets default values for all extended configuration options:
     * - Plasticity rule parameters
     * - Memory system settings
     * - Neuromodulation settings
     * - Development stage settings
     * - Checkpoint settings
     * - Error handling settings
     * - Performance settings
     */
    bool initializeDefaults();
    
    /**
     * @brief Load configuration from file
     * @param filepath Configuration file path
     * @return true if loaded successfully, false otherwise
     * @post Configuration loaded
     * 
     * @details Loads extended configuration from file:
     * - Extended format with additional options
     * - Validation of extended options
     * - Merging with defaults
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Get float configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     * @post Value accessible
     */
    float getFloat(const std::string& key, float defaultValue) const;
    
    /**
     * @brief Get integer configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     * @post Value accessible
     */
    int64_t getInt64(const std::string& key, int64_t defaultValue) const;
    
    /**
     * @brief Get boolean configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     * @post Value accessible
     */
    bool getBool(const std::string& key, bool defaultValue) const;
    
    /**
     * @brief Get string configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     * @post Value accessible
     */
    std::string getString(const std::string& key, const std::string& defaultValue) const;
    
    /**
     * @brief Get vector configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Configuration value or default
     * @post Value accessible
     */
    std::vector<float> getVector(const std::string& key, const std::vector<float>& defaultValue) const;
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key
     * @return true if key exists, false otherwise
     * @post Key existence accessible
     */
    bool has(const std::string& key) const { return config->has(key); }
    
    /**
     * @brief Get configuration summary
     * @return Configuration summary as string
     * @post Summary generated
     * 
     * @details Generates comprehensive configuration summary including:
     * - All extended configuration options
     * - Source information for each option
     * - Validation status
     * - Warnings and errors
     */
    std::string getSummary() const;
    
    /**
     * @brief Set development stage
     * @param stage Development stage (1-6)
     * @post Development stage set
     */
    void setDevelopmentStage(int64_t stage) { developmentStage = stage; }
    
    /**
     * @brief Get development stage
     * @return Development stage
     * @post Development stage accessible
     */
    int64_t getDevelopmentStage() const { return developmentStage; }
    
    /**
     * @brief Set plasticity mode
     * @param mode Plasticity mode (stdp, hebbian, structural, mixed)
     * @post Plasticity mode set
     */
    void setPlasticityMode(const std::string& mode) { plasticityMode = mode; }
    
    /**
     * @brief Get plasticity mode
     * @return Plasticity mode
     * @post Plasticity mode accessible
     */
    const std::string& getPlasticityMode() const { return plasticityMode; }
    
    /**
     * @brief Set neuromodulation enabled
     * @param enabled Neuromodulation enabled flag
     * @post Neuromodulation setting set
     */
    void setNeuromodulationEnabled(bool enabled) { neuromodulationEnabled = enabled; }
    
    /**
     * @brief Check if neuromodulation enabled
     * @return true if neuromodulation enabled, false otherwise
     * @post Neuromodulation status accessible
     */
    bool isNeuromodulationEnabled() const { return neuromodulationEnabled; }

private:
    std::shared_ptr<Config> config;
    
    // Development stage configuration
    int64_t developmentStage;
    
    // Plasticity configuration
    std::string plasticityMode;
    float stdpLTPWeight;
    float stdpLTDWeight;
    float hebbianLearningRate;
    float structuralPlasticityRate;
    
    // Memory system configuration
    int64_t memoryCapacity;
    float memoryRetentionRate;
    int64_t workingMemorySize;
    
    // Neuromodulation configuration
    bool neuromodulationEnabled;
    float curiosityWeight;
    float rewardWeight;
    float predictionErrorWeight;
    
    // Development stage configuration
    std::map<int64_t, std::string> developmentStageNames;
    std::map<int64_t, std::vector<std::string>> developmentStageFeatures;
    
    // Checkpoint and persistence configuration
    int64_t checkpointInterval;
    bool autoSaveEnabled;
    std::string checkpointDirectory;
    
    // Error handling and recovery configuration
    bool errorRecoveryEnabled;
    int64_t errorRecoveryAttempts;
    float errorRecoveryThreshold;
    
    // Performance optimization configuration
    bool performanceOptimizationEnabled;
    std::string optimizationStrategy;
    int64_t optimizationParameters;
    
    // Validation and verification configuration
    bool validationEnabled;
    bool verificationEnabled;
    std::vector<std::string> validationRules;
    
    // Debug and testing configuration
    bool debugModeEnabled;
    bool testingModeEnabled;
    std::string errorInjectionType;
    float errorInjectionRate;
    
    void initializeDefaultPlasticityConfig();
    void initializeDefaultMemoryConfig();
    void initializeDefaultNeuromodulationConfig();
    void initializeDefaultDevelopmentConfig();
    void initializeDefaultCheckpointConfig();
    void initializeDefaultErrorHandlingConfig();
    void initializeDefaultPerformanceConfig();
    void initializeDefaultValidationConfig();
    void initializeDefaultDebugConfig();
    
    // Suppress warnings about unused private members
    ExtendedConfig(const ExtendedConfig&) = delete;
    ExtendedConfig& operator=(const ExtendedConfig&) = delete;
};

} // namespace nlm