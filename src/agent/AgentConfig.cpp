#include "AgentConfig.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>

namespace nlm {

bool AgentConfig::validate() const {
    bool isValid = true;
    
    // Validate neuromodulation parameters
    if (rewardModulationStrength < -1.0f || rewardModulationStrength > 1.0f) {
        NLM_LOG_ERROR("rewardModulationStrength must be between -1.0 and 1.0");
        isValid = false;
    }
    
    if (curiosityThreshold < 0.0f || curiosityThreshold > 1.0f) {
        NLM_LOG_ERROR("curiosityThreshold must be between 0.0 and 1.0");
        isValid = false;
    }
    
    if (noveltySensitivity < 0.0f) {
        NLM_LOG_ERROR("noveltySensitivity must be non-negative");
        isValid = false;
    }
    
    if (predictionErrorWeight < 0.0f || predictionErrorWeight > 1.0f) {
        NLM_LOG_ERROR("predictionErrorWeight must be between 0.0 and 1.0");
        isValid = false;
    }
    
    // Validate developmental parameters
    if (developmentalSpeed < 0.0f) {
        NLM_LOG_ERROR("developmentalSpeed must be non-negative");
        isValid = false;
    }
    
    if (plasticityRate < 0.0f) {
        NLM_LOG_ERROR("plasticityRate must be non-negative");
        isValid = false;
    }
    
    // Validate sensory processing parameters
    if (visionSmoothing < 0.0f || visionSmoothing > 1.0f) {
        NLM_LOG_ERROR("visionSmoothing must be between 0.0 and 1.0");
        isValid = false;
    }
    
    if (touchThreshold < 0.0f || touchThreshold > 1.0f) {
        NLM_LOG_ERROR("touchThreshold must be between 0.0 and 1.0");
        isValid = false;
    }
    
    // Validate motor control parameters
    if (motorActivityThreshold < 0.0f || motorActivityThreshold > 1.0f) {
        NLM_LOG_ERROR("motorActivityThreshold must be between 0.0 and 1.0");
        isValid = false;
    }
    
    if (explorationProbability < 0.0f || explorationProbability > 1.0f) {
        NLM_LOG_ERROR("explorationProbability must be between 0.0 and 1.0");
        isValid = false;
    }
    
    // Validate memory parameters
    if (memoryConsolidationThreshold < 0.0f || memoryConsolidationThreshold > 1.0f) {
        NLM_LOG_ERROR("memoryConsolidationThreshold must be between 0.0 and 1.0");
        isValid = false;
    }
    
    return isValid;
}

template<typename T>
T AgentConfig::getParameter(const std::string& name, const T& defaultValue) const {
    auto it = configMap_.find(name);
    if (it != configMap_.end()) {
        // TODO: Implement proper type conversion from string to T
        // For now, return default
        NLM_LOG_WARNING("getParameter: Type conversion not implemented for " + name);
    }
    return defaultValue;
}

template<typename T>
void AgentConfig::setParameter(const std::string& name, const T& value) {
    std::string oldValue;
    auto it = configMap_.find(name);
    if (it != configMap_.end()) {
        oldValue = it->second;
    }
    
    // TODO: Implement proper type conversion from T to string
    std::string newValue = "";
    
    configMap_[name] = newValue;
    
    // Notify callbacks
    for (const auto& callback : callbacks_) {
        callback(name, oldValue, newValue);
    }
}

void AgentConfig::applyPreset(Preset preset) {
    // Reset to defaults
    *this = AgentConfig();
    
    // Apply preset-specific settings
    switch (preset) {
        case Preset::Minimal:
            enableRewardModulation = false;
            enableStructuralPlasticity = false;
            enableDevelopment = false;
            enableCuriosity = false;
            enableLogging = false;
            enableDebugVisualization = false;
            enableProfileMode = false;
            maxMemoryBufferSize = 256;
            break;
            
        case Preset::Balanced:
            // Current defaults represent balanced preset
            break;
            
        case Preset::Advanced:
            enableRewardModulation = true;
            enableStructuralPlasticity = true;
            enableDevelopment = true;
            enableCuriosity = true;
            enableLogging = true;
            enableDebugVisualization = true;
            enableProfileMode = true;
            maxMemoryBufferSize = 4096;
            visionSmoothing = 0.95f;
            break;
            
        case Preset::Custom:
            // User-defined settings remain
            break;
    }
    
    currentPreset = preset;
}

std::string AgentConfig::getConfigSummary() const {
    std::stringstream ss;
    ss << "Agent Configuration Summary:" << std::endl;
    ss << "==========================" << std::endl;
    ss << "Preset: " << (currentPreset == Preset::Minimal ? "Minimal" :
                         currentPreset == Preset::Balanced ? "Balanced" :
                         currentPreset == Preset::Advanced ? "Advanced" : "Custom") << std::endl;
    ss << "Reward Modulation: " << (enableRewardModulation ? "Enabled" : "Disabled") << std::endl;
    ss << "Structural Plasticity: " << (enableStructuralPlasticity ? "Enabled" : "Disabled") << std::endl;
    ss << "Development: " << (enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    ss << "Curiosity: " << (enableCuriosity ? "Enabled" : "Disabled") << std::endl;
    ss << "Memory Replay: " << (enableMemoryReplay ? "Enabled" : "Disabled") << std::endl;
    ss << "Working Memory: " << (enableWorkingMemory ? "Enabled" : "Disabled") << std::endl;
    ss << "Logging: " << (enableLogging ? "Enabled" : "Disabled") << std::endl;
    ss << "Debug Visualization: " << (enableDebugVisualization ? "Enabled" : "Disabled") << std::endl;
    ss << "Performance Metrics: " << (enablePerformanceMetrics ? "Enabled" : "Disabled") << std::endl;
    ss << "Vision Smoothing: " << visionSmoothing << std::endl;
    ss << "Motor Threshold: " << motorActivityThreshold << std::endl;
    ss << "Exploration Probability: " << explorationProbability << std::endl;
    ss << "Memory Buffer Size: " << maxMemoryBufferSize << std::endl;
    
    return ss.str();
}

void AgentConfig::addConfigChangedCallback(const ConfigChangedCallback& callback) {
    callbacks_.push_back(callback);
}

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

AgentConfig& ConfigManager::getConfig() {
    return config_;
}

void ConfigManager::setConfig(const AgentConfig& config) {
    config_ = config;
}

void ConfigManager::resetToDefaults() {
    config_ = AgentConfig();
}

bool ConfigManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Write configuration as simple key-value pairs
    file << "# NLM Agent Configuration" << std::endl;
    file << "# Generated on " << "[current date]" << std::endl;
    file << std::endl;
    
    file << "enableRewardModulation=" << (config_.enableRewardModulation ? "true" : "false") << std::endl;
    file << "enableStructuralPlasticity=" << (config_.enableStructuralPlasticity ? "true" : "false") << std::endl;
    file << "enableDevelopment=" << (config_.enableDevelopment ? "true" : "false") << std::endl;
    file << "enableCuriosity=" << (config_.enableCuriosity ? "true" : "false") << std::endl;
    file << "rewardModulationStrength=" << config_.rewardModulationStrength << std::endl;
    file << "curiosityThreshold=" << config_.curiosityThreshold << std::endl;
    file << "noveltySensitivity=" << config_.noveltySensitivity << std::endl;
    file << "predictionErrorWeight=" << config_.predictionErrorWeight << std::endl;
    file << "developmentalSpeed=" << config_.developmentalSpeed << std::endl;
    file << "plasticityRate=" << config_.plasticityRate << std::endl;
    file << "structuralPlasticityRate=" << config_.structuralPlasticityRate << std::endl;
    file << "pruningRate=" << config_.pruningRate << std::endl;
    file << "visionSmoothing=" << config_.visionSmoothing << std::endl;
    file << "touchThreshold=" << config_.touchThreshold << std::endl;
    file << "internalSignalScale=" << config_.internalSignalScale << std::endl;
    file << "motorActivityThreshold=" << config_.motorActivityThreshold << std::endl;
    file << "explorationProbability=" << config_.explorationProbability << std::endl;
    file << "enableLookActions=" << (config_.enableLookActions ? "true" : "false") << std::endl;
    file << "enableMemoryReplay=" << (config_.enableMemoryReplay ? "true" : "false") << std::endl;
    file << "memoryConsolidationThreshold=" << config_.memoryConsolidationThreshold << std::endl;
    file << "enableWorkingMemory=" << (config_.enableWorkingMemory ? "true" : "false") << std::endl;
    file << "maxMemoryBufferSize=" << config_.maxMemoryBufferSize << std::endl;
    file << "performanceMonitorInterval=" << config_.performanceMonitorInterval << std::endl;
    file << "enablePerformanceMetrics=" << (config_.enablePerformanceMetrics ? "true" : "false") << std::endl;
    file << "enableLogging=" << (config_.enableLogging ? "true" : "false") << std::endl;
    file << "enableDebugVisualization=" << (config_.enableDebugVisualization ? "true" : "false") << std::endl;
    file << "enableProfileMode=" << (config_.enableProfileMode ? "true" : "false") << std::endl;
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filename);
    return true;
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            continue;
        }
        
        std::string key = line.substr(0, equalsPos);
        std::string value = line.substr(equalsPos + 1);
        
        // TODO: Implement proper type conversion from string to config fields
        // For now, we'll just log unknown parameters
        NLM_LOG_WARNING("Unknown configuration parameter: " + key);
    }
    
    file.close();
    NLM_LOG_INFO("Configuration loaded from: " + filename);
    return true;
}

template<typename T>
T ConfigManager::getParameter(const std::string& name, const T& defaultValue) {
    return ConfigManager::getInstance().getConfig().getParameter<T>(name, defaultValue);
}

template<typename T>
void ConfigManager::setParameter(const std::string& name, const T& value) {
    ConfigManager::getInstance().getConfig().setParameter<T>(name, value);
}

} // namespace nlm