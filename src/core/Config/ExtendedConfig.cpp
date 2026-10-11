// ExtendedConfig.cpp - Extended configuration implementation for NLM with JSON support

#include "core/Config/ExtendedConfig.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <regex>

namespace nlm {

// Extended configuration key mappings for JSON support
static const std::unordered_map<std::string, std::string> JSON_TO_EXTENDED_KEY_MAP = {
    {"development_stage", "development_stage"},
    {"plasticity_mode", "plasticity_mode"},
    {"stdp_ltp_weight", "stdp_ltp_weight"},
    {"stdp_ltd_weight", "stdp_ltd_weight"},
    {"hebbian_learning_rate", "hebbian_learning_rate"},
    {"structural_plasticity_rate", "structural_plasticity_rate"},
    {"memory_capacity", "memory_capacity"},
    {"memory_retention_rate", "memory_retention_rate"},
    {"working_memory_size", "working_memory_size"},
    {"neuromodulation_enabled", "neuromodulation_enabled"},
    {"curiosity_weight", "curiosity_weight"},
    {"reward_weight", "reward_weight"},
    {"prediction_error_weight", "prediction_error_weight"},
    {"checkpoint_interval", "checkpoint_interval"},
    {"auto_save", "auto_save"},
    {"error_recovery_enabled", "error_recovery_enabled"},
    {"error_recovery_attempts", "error_recovery_attempts"},
    {"error_recovery_threshold", "error_recovery_threshold"},
    {"performance_optimization_enabled", "performance_optimization_enabled"},
    {"optimization_strategy", "optimization_strategy"},
    {"optimization_parameters", "optimization_parameters"},
    {"validation_enabled", "validation_enabled"},
    {"verification_enabled", "verification_enabled"},
    {"debug_mode_enabled", "debug_mode_enabled"},
    {"testing_mode_enabled", "testing_mode_enabled"},
    {"error_injection_type", "error_injection_type"},
    {"error_injection_rate", "error_injection_rate"}
};

// Reverse mapping for ExtendedConfig to JSON
static const std::unordered_map<std::string, std::string> EXTENDED_TO_JSON_KEY_MAP = {
    {"development_stage", "development_stage"},
    {"plasticity_mode", "plasticity_mode"},
    {"stdp_ltp_weight", "stdp_ltp_weight"},
    {"stdp_ltd_weight", "stdp_ltd_weight"},
    {"hebbian_learning_rate", "hebbian_learning_rate"},
    {"structural_plasticity_rate", "structural_plasticity_rate"},
    {"memory_capacity", "memory_capacity"},
    {"memory_retention_rate", "memory_retention_rate"},
    {"working_memory_size", "working_memory_size"},
    {"neuromodulation_enabled", "neuromodulation_enabled"},
    {"curiosity_weight", "curiosity_weight"},
    {"reward_weight", "reward_weight"},
    {"prediction_error_weight", "prediction_error_weight"},
    {"checkpoint_interval", "checkpoint_interval"},
    {"auto_save", "auto_save"},
    {"error_recovery_enabled", "error_recovery_enabled"},
    {"error_recovery_attempts", "error_recovery_attempts"},
    {"error_recovery_threshold", "error_recovery_threshold"},
    {"performance_optimization_enabled", "performance_optimization_enabled"},
    {"optimization_strategy", "optimization_strategy"},
    {"optimization_parameters", "optimization_parameters"},
    {"validation_enabled", "validation_enabled"},
    {"verification_enabled", "verification_enabled"},
    {"debug_mode_enabled", "debug_mode_enabled"},
    {"testing_mode_enabled", "testing_mode_enabled"},
    {"error_injection_type", "error_injection_type"},
    {"error_injection_rate", "error_injection_rate"}
};

namespace nlm {

ExtendedConfig::ExtendedConfig()
    : developmentStage(1),
      plasticityMode("mixed"),
      stdpLTPWeight(0.02f),
      stdpLTDWeight(0.015f),
      hebbianLearningRate(0.01f),
      structuralPlasticityRate(0.0001f),
      memoryCapacity(1000),
      memoryRetentionRate(0.99f),
      workingMemorySize(100),
      neuromodulationEnabled(false),
      curiosityWeight(0.5f),
      rewardWeight(0.3f),
      predictionErrorWeight(0.2f),
      checkpointInterval(1000),
      autoSaveEnabled(false),
      errorRecoveryEnabled(true),
      errorRecoveryAttempts(3),
      errorRecoveryThreshold(0.95f),
      performanceOptimizationEnabled(false),
      optimizationParameters(0),
      validationEnabled(true),
      verificationEnabled(true),
      debugModeEnabled(false),
      testingModeEnabled(false),
      errorInjectionType("none"),
      errorInjectionRate(0.0f) {
    config = std::make_shared<Config>();
    initializeDefaults();
}

ExtendedConfig::~ExtendedConfig() = default;

bool ExtendedConfig::initializeDefaults() {
    NLM_LOG_INFO("Initializing extended configuration defaults...");
    
    // Initialize all configuration categories
    initializeDefaultPlasticityConfig();
    initializeDefaultMemoryConfig();
    initializeDefaultNeuromodulationConfig();
    initializeDefaultDevelopmentConfig();
    initializeDefaultCheckpointConfig();
    initializeDefaultErrorHandlingConfig();
    initializeDefaultPerformanceConfig();
    initializeDefaultValidationConfig();
    initializeDefaultDebugConfig();
    
    NLM_LOG_INFO("Extended configuration defaults initialized");
    return true;
}

bool ExtendedConfig::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading extended configuration from: " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Cannot open extended configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            // Skip empty lines and comments
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Map extended configuration keys to internal variables
                if (key == "development_stage") {
                    developmentStage = std::stoll(value);
                } else if (key == "plasticity_mode") {
                    plasticityMode = value;
                } else if (key == "stdp_ltp_weight") {
                    stdpLTPWeight = std::stof(value);
                } else if (key == "stdp_ltd_weight") {
                    stdpLTDWeight = std::stof(value);
                } else if (key == "hebbian_learning_rate") {
                    hebbianLearningRate = std::stof(value);
                } else if (key == "structural_plasticity_rate") {
                    structuralPlasticityRate = std::stof(value);
                } else if (key == "memory_capacity") {
                    memoryCapacity = std::stoll(value);
                } else if (key == "memory_retention_rate") {
                    memoryRetentionRate = std::stof(value);
                } else if (key == "working_memory_size") {
                    workingMemorySize = std::stoll(value);
                } else if (key == "neuromodulation_enabled") {
                    neuromodulationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "curiosity_weight") {
                    curiosityWeight = std::stof(value);
                } else if (key == "reward_weight") {
                    rewardWeight = std::stof(value);
                } else if (key == "prediction_error_weight") {
                    predictionErrorWeight = std::stof(value);
                } else if (key == "checkpoint_interval") {
                    checkpointInterval = std::stoll(value);
                } else if (key == "auto_save") {
                    autoSaveEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "error_recovery_enabled") {
                    errorRecoveryEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "error_recovery_attempts") {
                    errorRecoveryAttempts = std::stoll(value);
                } else if (key == "error_recovery_threshold") {
                    errorRecoveryThreshold = std::stof(value);
                } else if (key == "performance_optimization_enabled") {
                    performanceOptimizationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "optimization_strategy") {
                    optimizationStrategy = value;
                } else if (key == "optimization_parameters") {
                    optimizationParameters = std::stoll(value);
                } else if (key == "validation_enabled") {
                    validationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "verification_enabled") {
                    verificationEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "debug_mode_enabled") {
                    debugModeEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "testing_mode_enabled") {
                    testingModeEnabled = (value == "true" || value == "1" || value == "yes");
                } else if (key == "error_injection_type") {
                    errorInjectionType = value;
                } else if (key == "error_injection_rate") {
                    errorInjectionRate = std::stof(value);
                }
            } else {
                NLM_LOG_WARNING("Invalid extended config format at line " + std::to_string(lineNumber) + ": " + line);
            }
        }
        
        NLM_LOG_INFO("Extended configuration loaded successfully from: " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading extended configuration: " + std::string(e.what()));
        return false;
    }
}

float ExtendedConfig::getFloat(const std::string& key, float defaultValue) const {
    // For extended configuration, we check our internal variables first
    // Then delegate to the base config
    if (key == "development_stage") {
        return static_cast<float>(developmentStage);
    } else if (key == "plasticity_mode") {
        // Return as float (not ideal, but for compatibility)
        if (plasticityMode == "stdp") return 1.0f;
        else if (plasticityMode == "hebbian") return 2.0f;
        else if (plasticityMode == "structural") return 3.0f;
        else return 4.0f; // mixed
    } else if (key == "stdp_ltp_weight") {
        return stdpLTPWeight;
    } else if (key == "stdp_ltd_weight") {
        return stdpLTDWeight;
    } else if (key == "hebbian_learning_rate") {
        return hebbianLearningRate;
    } else if (key == "structural_plasticity_rate") {
        return structuralPlasticityRate;
    } else if (key == "memory_capacity") {
        return static_cast<float>(memoryCapacity);
    } else if (key == "memory_retention_rate") {
        return memoryRetentionRate;
    } else if (key == "working_memory_size") {
        return static_cast<float>(workingMemorySize);
    } else if (key == "curiosity_weight") {
        return curiosityWeight;
    } else if (key == "reward_weight") {
        return rewardWeight;
    } else if (key == "prediction_error_weight") {
        return predictionErrorWeight;
    } else if (key == "checkpoint_interval") {
        return static_cast<float>(checkpointInterval);
    } else if (key == "error_recovery_attempts") {
        return static_cast<float>(errorRecoveryAttempts);
    } else if (key == "error_recovery_threshold") {
        return errorRecoveryThreshold;
    } else if (key == "optimization_parameters") {
        return static_cast<float>(optimizationParameters);
    } else if (key == "error_injection_rate") {
        return errorInjectionRate;
    }
    
    // Otherwise delegate to base config
    auto val = config->get<float>(key);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t ExtendedConfig::getInt64(const std::string& key, int64_t defaultValue) const {
    if (key == "development_stage") {
        return developmentStage;
    } else if (key == "memory_capacity") {
        return memoryCapacity;
    } else if (key == "working_memory_size") {
        return workingMemorySize;
    } else if (key == "checkpoint_interval") {
        return checkpointInterval;
    } else if (key == "error_recovery_attempts") {
        return errorRecoveryAttempts;
    } else if (key == "optimization_parameters") {
        return optimizationParameters;
    }
    
    auto val = config->get<int64_t>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool ExtendedConfig::getBool(const std::string& key, bool defaultValue) const {
    if (key == "neuromodulation_enabled") {
        return neuromodulationEnabled;
    } else if (key == "auto_save") {
        return autoSaveEnabled;
    } else if (key == "error_recovery_enabled") {
        return errorRecoveryEnabled;
    } else if (key == "performance_optimization_enabled") {
        return performanceOptimizationEnabled;
    } else if (key == "validation_enabled") {
        return validationEnabled;
    } else if (key == "verification_enabled") {
        return verificationEnabled;
    } else if (key == "debug_mode_enabled") {
        return debugModeEnabled;
    } else if (key == "testing_mode_enabled") {
        return testingModeEnabled;
    }
    
    auto val = config->get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string ExtendedConfig::getString(const std::string& key, const std::string& defaultValue) const {
    if (key == "plasticity_mode") {
        return plasticityMode;
    } else if (key == "optimization_strategy") {
        return optimizationStrategy;
    } else if (key == "error_injection_type") {
        return errorInjectionType;
    }
    
    auto val = config->get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::vector<float> ExtendedConfig::getVector(const std::string& key, const std::vector<float>& defaultValue) const {
    auto val = config->get<std::vector<float>>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string ExtendedConfig::getSummary() const {
    std::ostringstream oss;
    oss << "Extended Configuration Summary\n";
    oss << "=============================\n\n";
    
    oss << "Development Stage:\n";
    oss << "  Stage: " << developmentStage << " (" << 
        (developmentStageNames.count(developmentStage) ? developmentStageNames.at(developmentStage) : "Unknown") << ")\n";
    oss << "  Features: ";
    if (developmentStageFeatures.count(developmentStage)) {
        for (const auto& feature : developmentStageFeatures.at(developmentStage)) {
            oss << feature << " ";
        }
    }
    oss << "\n\n";
    
    oss << "Plasticity Configuration:\n";
    oss << "  Mode: " << plasticityMode << "\n";
    oss << "  STDP LTP weight: " << stdpLTPWeight << "\n";
    oss << "  STDP LTD weight: " << stdpLTDWeight << "\n";
    oss << "  Hebbian learning rate: " << hebbianLearningRate << "\n";
    oss << "  Structural plasticity rate: " << structuralPlasticityRate << "\n\n";
    
    oss << "Memory System Configuration:\n";
    oss << "  Capacity: " << memoryCapacity << "\n";
    oss << "  Retention rate: " << memoryRetentionRate << "\n";
    oss << "  Working memory size: " << workingMemorySize << "\n\n";
    
    oss << "Neuromodulation Configuration:\n";
    oss << "  Enabled: " << (neuromodulationEnabled ? "true" : "false") << "\n";
    oss << "  Curiosity weight: " << curiosityWeight << "\n";
    oss << "  Reward weight: " << rewardWeight << "\n";
    oss << "  Prediction error weight: " << predictionErrorWeight << "\n\n";
    
    oss << "Checkpoint Configuration:\n";
    oss << "  Interval: " << checkpointInterval << "\n";
    oss << "  Auto-save: " << (autoSaveEnabled ? "enabled" : "disabled") << "\n";
    oss << "  Directory: " << checkpointDirectory << "\n\n";
    
    oss << "Error Handling Configuration:\n";
    oss << "  Recovery enabled: " << (errorRecoveryEnabled ? "true" : "false") << "\n";
    oss << "  Recovery attempts: " << errorRecoveryAttempts << "\n";
    oss << "  Recovery threshold: " << errorRecoveryThreshold << "\n\n";
    
    oss << "Performance Configuration:\n";
    oss << "  Optimization enabled: " << (performanceOptimizationEnabled ? "true" : "false") << "\n";
    oss << "  Strategy: " << optimizationStrategy << "\n";
    oss << "  Parameters: " << optimizationParameters << "\n\n";
    
    oss << "Validation Configuration:\n";
    oss << "  Validation enabled: " << (validationEnabled ? "true" : "false") << "\n";
    oss << "  Verification enabled: " << (verificationEnabled ? "true" : "false") << "\n";
    oss << "  Rules count: " << validationRules.size() << "\n\n";
    
    oss << "Debug Configuration:\n";
    oss << "  Debug mode enabled: " << (debugModeEnabled ? "true" : "false") << "\n";
    oss << "  Testing mode enabled: " << (testingModeEnabled ? "true" : "false") << "\n";
    oss << "  Error injection type: " << errorInjectionType << "\n";
    oss << "  Error injection rate: " << errorInjectionRate << "\n";
    
    return oss.str();
}

void ExtendedConfig::initializeDefaultPlasticityConfig() {
    NLM_LOG_INFO("Initializing default plasticity configuration...");
    
    // Set plasticity mode based on development stage
    if (developmentStage <= 2) {
        plasticityMode = "hebbian";
    } else if (developmentStage <= 4) {
        plasticityMode = "stdp";
    } else {
        plasticityMode = "mixed";
    }
    
    // Set plasticity rule parameters
    if (plasticityMode == "stdp") {
        stdpLTPWeight = 0.02f;
        stdpLTDWeight = 0.015f;
    } else if (plasticityMode == "hebbian") {
        stdpLTPWeight = 0.0f;
        stdpLTDWeight = 0.0f;
        hebbianLearningRate = 0.01f;
    } else if (plasticityMode == "structural") {
        stdpLTPWeight = 0.0f;
        stdpLTDWeight = 0.0f;
        structuralPlasticityRate = 0.0001f;
    } else { // mixed
        stdpLTPWeight = 0.015f;
        stdpLTDWeight = 0.0125f;
        hebbianLearningRate = 0.008f;
        structuralPlasticityRate = 0.00005f;
    }
    
    NLM_LOG_INFO("Plasticity configuration initialized: mode=" + plasticityMode);
}

void ExtendedConfig::initializeDefaultMemoryConfig() {
    NLM_LOG_INFO("Initializing default memory configuration...");
    
    // Set memory capacity based on development stage
    if (developmentStage <= 2) {
        memoryCapacity = 500;
        workingMemorySize = 50;
    } else if (developmentStage <= 4) {
        memoryCapacity = 1000;
        workingMemorySize = 100;
    } else {
        memoryCapacity = 2000;
        workingMemorySize = 200;
    }
    
    memoryRetentionRate = 0.95f + (developmentStage * 0.01f); // Better retention in later stages
    
    NLM_LOG_INFO("Memory configuration initialized: capacity=" + std::to_string(memoryCapacity) + ", retention=" + std::to_string(memoryRetentionRate));
}

void ExtendedConfig::initializeDefaultNeuromodulationConfig() {
    NLM_LOG_INFO("Initializing default neuromodulation configuration...");
    
    // Enable neuromodulation based on development stage
    neuromodulationEnabled = (developmentStage >= 2);
    
    // Set neuromodulator weights based on stage
    if (developmentStage <= 2) {
        curiosityWeight = 0.6f;
        rewardWeight = 0.2f;
        predictionErrorWeight = 0.2f;
    } else if (developmentStage <= 4) {
        curiosityWeight = 0.4f;
        rewardWeight = 0.4f;
        predictionErrorWeight = 0.2f;
    } else {
        curiosityWeight = 0.3f;
        rewardWeight = 0.3f;
        predictionErrorWeight = 0.4f;
    }
    
    NLM_LOG_INFO("Neuromodulation configuration initialized: enabled=" + std::string(neuromodulationEnabled ? "true" : "false") + ", curiosity=" + std::to_string(curiosityWeight));
}

void ExtendedConfig::initializeDefaultDevelopmentConfig() {
    NLM_LOG_INFO("Initializing default development stage configuration...");
    
    // Set development stage names and features
    developmentStageNames[1] = "Sensory"; developmentStageFeatures[1] = {"Sensory processing", "Basic reflexes"};
    developmentStageNames[2] = "Motor"; developmentStageFeatures[2] = {"Movement coordination", "Basic learning"};
    developmentStageNames[3] = "Cognitive"; developmentStageFeatures[3] = {"Concept formation", "Memory consolidation"};
    developmentStageNames[4] = "Social"; developmentStageFeatures[4] = {"Social interaction", "Cooperation"};
    developmentStageNames[5] = "Abstract"; developmentStageFeatures[5] = {"Abstract reasoning", "Problem solving"};
    developmentStageNames[6] = "Metacognitive"; developmentStageFeatures[6] = {"Self-awareness", "Meta-learning"};
    
    NLM_LOG_INFO("Development configuration initialized: stage=" + std::to_string(developmentStage) + " (" + developmentStageNames[developmentStage] + ")");
}

void ExtendedConfig::initializeDefaultCheckpointConfig() {
    NLM_LOG_INFO("Initializing default checkpoint configuration...");
    
    // Set checkpoint interval based on development stage and profile
    if (developmentStage <= 2) {
        checkpointInterval = 500;
    } else if (developmentStage <= 4) {
        checkpointInterval = 1000;
    } else {
        checkpointInterval = 2000;
    }
    
    // Enable auto-save for higher development stages
    autoSaveEnabled = (developmentStage >= 3);
    checkpointDirectory = "checkpoints";
    
    NLM_LOG_INFO("Checkpoint configuration initialized: interval=" + std::to_string(checkpointInterval) + ", auto-save=" + std::string(autoSaveEnabled ? "true" : "false"));
}

void ExtendedConfig::initializeDefaultErrorHandlingConfig() {
    NLM_LOG_INFO("Initializing default error handling configuration...");
    
    // Enable error recovery
    errorRecoveryEnabled = true;
    
    // Set recovery attempts based on development stage
    if (developmentStage <= 2) {
        errorRecoveryAttempts = 2;
        errorRecoveryThreshold = 0.9f;
    } else if (developmentStage <= 4) {
        errorRecoveryAttempts = 3;
        errorRecoveryThreshold = 0.95f;
    } else {
        errorRecoveryAttempts = 5;
        errorRecoveryThreshold = 0.98f;
    }
    
    NLM_LOG_INFO("Error handling configuration initialized: recovery=" + std::string(errorRecoveryEnabled ? "true" : "false") + ", attempts=" + std::to_string(errorRecoveryAttempts));
}

void ExtendedConfig::initializeDefaultPerformanceConfig() {
    NLM_LOG_INFO("Initializing default performance configuration...");
    
    // Enable performance optimization for higher development stages
    performanceOptimizationEnabled = (developmentStage >= 4);
    
    // Set optimization strategy
    if (developmentStage <= 2) {
        optimizationStrategy = "basic";
        optimizationParameters = 0;
    } else if (developmentStage <= 4) {
        optimizationStrategy = "balanced";
        optimizationParameters = 1;
    } else {
        optimizationStrategy = "advanced";
        optimizationParameters = 2;
    }
    
    NLM_LOG_INFO("Performance configuration initialized: optimization=" + std::string(performanceOptimizationEnabled ? "true" : "false") + ", strategy=" + optimizationStrategy);
}

void ExtendedConfig::initializeDefaultValidationConfig() {
    NLM_LOG_INFO("Initializing default validation configuration...");
    
    // Enable validation and verification
    validationEnabled = true;
    verificationEnabled = true;
    
    // Set validation rules based on development stage
    if (developmentStage <= 2) {
        validationRules = {"basic_connectivity", "sensory_processing"};
    } else if (developmentStage <= 4) {
        validationRules = {"basic_connectivity", "sensory_processing", "memory_consolidation", "learning_detection"};
    } else {
        validationRules = {"basic_connectivity", "sensory_processing", "memory_consolidation", "learning_detection", "social_interaction", "abstract_reasoning", "meta_learning"};
    }
    
    NLM_LOG_INFO("Validation configuration initialized: validation=" + std::string(validationEnabled ? "true" : "false") + ", rules=" + std::to_string(validationRules.size()));
}

void ExtendedConfig::initializeDefaultDebugConfig() {
    NLM_LOG_INFO("Initializing default debug configuration...");
    
    // Enable debug mode for testing
    debugModeEnabled = (developmentStage >= 5) || testingModeEnabled;
    testingModeEnabled = false; // User can enable this via command line
    
    // Set error injection type
    if (debugModeEnabled) {
        errorInjectionType = "neuron";
        errorInjectionRate = 0.01f;
    } else {
        errorInjectionType = "none";
        errorInjectionRate = 0.0f;
    }
    
    NLM_LOG_INFO("Debug configuration initialized: debug-mode=" + std::string(debugModeEnabled ? "true" : "false") + ", error-injection=" + errorInjectionType);
}

} // namespace nlm