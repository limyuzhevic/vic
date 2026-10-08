// Validated Configuration and Enhanced Error Handling for NLM
// This file provides enhanced configuration validation and error handling

#pragma once

#include "Config.hpp"
#include "Logger.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// Custom exception types for enhanced error handling
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) 
        : std::runtime_error("Config Validation Error: " + message) {}
};

class NeuronValidationError : public std::runtime_error {
public:
    NeuronValidationError(const std::string& message) 
        : std::runtime_error("Neuron Validation Error: " + message) {}
};

class BrainValidationError : public std::runtime_error {
public:
    BrainValidationError(const std::string& message) 
        : std::runtime_error("Brain Validation Error: " + message) {}
};

// Forward declarations
class ValidatedNeuron;
class ValidatedBrain;

// Enhanced Config class with validation
class ValidatedConfig : public Config {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ValidationResult validate() const;
    
    // Enhanced save with validation
    bool saveToFileValidated(const std::string& filepath) const;
    
    // Enhanced load with validation
    bool loadFromFileValidated(const std::string& filepath);
    
    friend class ValidatedBrain; // For brain configuration access
};

// Enhanced Neuron with validation and error handling
class ValidatedNeuron : public Neuron {
public:
    bool validateState() const;
    bool safeStepLIF(Timestamp currentTime, TimestepDuration dt);
    
    friend class ValidatedBrain; // For brain neuron access
};

// Enhanced Brain with validation and error handling
class ValidatedBrain : public Brain {
public:
    struct BrainHealth {
        bool isHealthy;
        float neuronHealth;
        float synapseHealth;
        float memoryHealth;
        float neuromodulationHealth;
        std::vector<std::string> issues;
    };
    
    BrainHealth assessHealth() const;
    bool safeStep(SimulationStep currentStep, Timestamp currentTime);
    std::unique_ptr<Action> safeProduceAction();
    
    friend class ValidatedConfig; // For config access
};

// Enhanced configuration validation utilities
namespace ConfigValidation {

    // Comprehensive configuration validation
    struct FullValidationResult {
        bool isValid;
        std::vector<std::string> criticalErrors;
        std::vector<std::string> warnings;
        std::vector<std::string> info;
    };
    
    FullValidationResult validateFull(const Config& config);
    
    // Configuration merge utility
    void mergeConfig(Config& target, const Config& source, 
                     const std::unordered_set<std::string>& excludeKeys = {});
    
    // Configuration backup utility
    bool backupConfig(const Config& config, const std::string& backupPath);
    
    // Configuration comparison utility
    std::vector<std::string> findConfigDifferences(const Config& config1, 
                                                  const Config& config2);
}

// Global validation instances
extern ValidatedConfig globalValidatedConfig;
extern ValidatedBrain globalValidatedBrain;

// Global validation functions
inline void validateAndLog(const Config& config, const std::string& context);
inline bool isConfigValid(const Config& config);
inline std::vector<std::string> getConfigIssues(const Config& config);
inline void logBrainHealth(const Brain& brain);
inline bool isBrainHealthy(const Brain& brain);

// Safe wrapper functions for backward compatibility
inline bool safeBrainStep(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
inline std::unique_ptr<Action> safeBrainAction(Brain& brain);
inline bool safeConfigLoad(Config& config, const std::string& filepath);
inline bool safeConfigSave(const Config& config, const std::string& filepath);

// Performance-optimized validation macros
#define VALIDATE_CONFIG_IF_NEEDED(config) \
    do { \
        if (!isConfigValid(config)) { \
            NLM_LOG_ERROR("Configuration validation failed, using defaults"); \
        } \
    } while(0)

#define VALIDATE_BRAIN_IF_NEEDED(brain) \
    do { \
        if (!isBrainHealthy(brain)) { \
            NLM_LOG_WARNING("Brain health check failed"); \
        } \
    } while(0)

} // namespace nlm