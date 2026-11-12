// NLM Configuration System - Enhanced Version
// This file provides advanced configuration handling, validation, and utilities
// for the NLM (Neural Learning Machine) demos and applications.

#ifndef NLM_CONFIG_SYSTEM_H
#define NLM_CONFIG_SYSTEM_H

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace nlm {

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Parameter validation rules
struct ParameterValidation {
    std::string name;
    std::string description;
    float minValue;
    float maxValue;
    float defaultValue;
    bool isRequired;
    std::string unit;
    
    ParameterValidation(const std::string& n, const std::string& desc, float min, float max, 
                       float def, bool req = false, const std::string& unit = "")
        : name(n), description(desc), minValue(min), maxValue(max), defaultValue(def),
          isRequired(req), unit(unit) {}
    
    bool validate(float value) const {
        return (value >= minValue && value <= maxValue);
    }
    
    std::string getErrorMessage(float value) const {
        if (value < minValue) {
            return name + " (" + description + ") must be >= " + std::to_string(minValue) + unit;
        } else if (value > maxValue) {
            return name + " (" + description + ") must be <= " + std::to_string(maxValue) + unit;
        }
        return "";
    }
};

// Advanced configuration validator
class ConfigValidator {
private:
    std::unordered_map<std::string, ParameterValidation> validationRules;
    std::unordered_map<std::string, std::string> configDescriptions;
    std::unordered_map<std::string, std::function<bool(float)>> customValidators;
    
public:
    ConfigValidator() {
        initializeValidationRules();
        initializeConfigDescriptions();
    }
    
private:
    void initializeValidationRules() {
        // Brain configuration parameters
        validationRules["neuron_count"] = ParameterValidation(
            "neuron_count", "Number of neurons in the brain", 1, 10000, 1000, true, " neurons"
        );
        
        validationRules["region_count"] = ParameterValidation(
            "region_count", "Number of brain regions", 1, 10, 2, true, " regions"
        );
        
        validationRules["connection_probability"] = ParameterValidation(
            "connection_probability", "Probability of synapse formation", 0.0f, 1.0f, 0.1f, true, ""
        );
        
        validationRules["simulation_timestep"] = ParameterValidation(
            "simulation_timestep", "Time step for simulation", 0.0001f, 0.1f, 0.001f, true, "s"
        );
        
        validationRules["stdp_ltp_weight"] = ParameterValidation(
            "stdp_ltp_weight", "STDP long-term potentiation weight", 0.0f, 1.0f, 0.01f, true, ""
        );
        
        validationRules["stdp_ltd_weight"] = ParameterValidation(
            "stdp_ltd_weight", "STDP long-term depression weight", 0.0f, 1.0f, 0.012f, true, ""
        );
        
        validationRules["stdp_tau"] = ParameterValidation(
            "stdp_tau", "STDP spike timing dependence time constant", 1.0f, 100.0f, 20.0f, true, " ms"
        );
        
        validationRules["synaptogenesis_rate"] = ParameterValidation(
            "synaptogenesis_rate", "Rate of new synapse formation", 0.0f, 0.1f, 0.0001f, false, " /step"
        );
        
        validationRules["pruning_rate"] = ParameterValidation(
            "pruning_rate", "Rate of synapse elimination", 0.0f, 0.1f, 0.00001f, false, " /step"
        );
        
        // World configuration parameters
        validationRules["environment_width"] = ParameterValidation(
            "environment_width", "Width of the environment", 10, 1000, 20, true, " units"
        );
        
        validationRules["environment_height"] = ParameterValidation(
            "environment_height", "Height of the environment", 10, 1000, 20, true, " units"
        );
        
        // Sensory configuration parameters
        validationRules["vision_width"] = ParameterValidation(
            "vision_width", "Width of visual sensor field of view", 8, 64, 16, false, " pixels"
        );
        
        validationRules["vision_height"] = ParameterValidation(
            "vision_height", "Height of visual sensor field of view", 8, 64, 16, false, " pixels"
        );
        
        // Neuromodulation parameters
        validationRules["dopamine_baseline"] = ParameterValidation(
            "dopamine_baseline", "Baseline dopamine level", 0.0f, 2.0f, 0.1f, false, ""
        );
        
        validationRules["curiosity_baseline"] = ParameterValidation(
            "curiosity_baseline", "Baseline curiosity level", 0.0f, 1.0f, 0.3f, false, ""
        );
        
        validationRules["novelty_threshold"] = ParameterValidation(
            "novelty_threshold", "Novelty detection threshold", 0.0f, 1.0f, 0.5f, false, ""
        );
    }
    
    void initializeConfigDescriptions() {
        configDescriptions["neuron_count"] = "Number of neurons in the brain (affects computational capacity)";
        configDescriptions["region_count"] = "Number of brain regions (enables modular processing)";
        configDescriptions["connection_probability"] = "Initial probability of synaptic connections";
        configDescriptions["simulation_timestep"] = "Simulation time step for numerical stability";
        configDescriptions["stdp_ltp_weight"] = "STDP weight for long-term potentiation";
        configDescriptions["stdp_ltd_weight"] = "STDP weight for long-term depression";
        configDescriptions["stdp_tau"] = "STDP spike timing dependence time constant";
        configDescriptions["synaptogenesis_rate"] = "Rate of structural plasticity (synaptogenesis)";
        configDescriptions["pruning_rate"] = "Rate of structural plasticity (synaptic pruning)";
        configDescriptions["environment_width"] = "Width of the simulated world";
        configDescriptions["environment_height"] = "Height of the simulated world";
        configDescriptions["vision_width"] = "Width of visual sensor field of view";
        configDescriptions["vision_height"] = "Height of visual sensor field of view";
        configDescriptions["dopamine_baseline"] = "Baseline dopamine level for motivation";
        configDescriptions["curiosity_baseline"] = "Baseline curiosity level for exploration";
        configDescriptions["novelty_threshold"] = "Threshold for novelty-based learning";
    }
    
public:
    // Validate a configuration value
    bool validateParameter(const std::string& key, float value) {
        auto it = validationRules.find(key);
        if (it == validationRules.end()) {
            // Unknown parameter, allow it but warn
            std::cout << "[CONFIG WARNING] Unknown parameter: " << key << std::endl;
            return true;
        }
        return it->second.validate(value);
    }
    
    // Validate all configuration parameters
    std::vector<std::string> validateAll(const std::shared_ptr<Config>& config) {
        std::vector<std::string> errors;
        
        for (const auto& rule : validationRules) {
            const std::string& key = rule.first;
            if (config->has(key)) {
                auto valueOpt = config->get<double>(key);
                if (valueOpt) {
                    float value = static_cast<float>(*valueOpt);
                    if (!rule.second.validate(value)) {
                        errors.push_back(rule.second.getErrorMessage(value));
                    }
                }
            } else if (rule.second.isRequired) {
                errors.push_back("Missing required parameter: " + key);
            }
        }
        
        // Run custom validators
        for (const auto& customValidator : customValidators) {
            if (config->has(customValidator.first)) {
                auto valueOpt = config->get<double>(customValidator.first);
                if (valueOpt && !customValidator.second(static_cast<float>(*valueOpt))) {
                    errors.push_back("Custom validation failed for parameter: " + customValidator.first);
                }
            }
        }
        
        return errors;
    }
    
    // Add custom validator
    void addCustomValidator(const std::string& key, std::function<bool(float)> validator) {
        customValidators[key] = validator;
    }
    
    // Get configuration description
    std::string getDescription(const std::string& key) const {
        auto it = configDescriptions.find(key);
        return (it != configDescriptions.end()) ? it->second : "No description available";
    }
    
    // Get all parameter names
    std::vector<std::string> getAllParameterNames() const {
        std::vector<std::string> names;
        for (const auto& pair : validationRules) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Print validation report
    void printValidationReport(const std::shared_ptr<Config>& config) const {
        std::cout << "Configuration Validation Report:" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        for (const auto& rule : validationRules) {
            const std::string& key = rule.first;
            if (config->has(key)) {
                auto valueOpt = config->get<double>(key);
                if (valueOpt) {
                    float value = static_cast<float>(*valueOpt);
                    bool isValid = rule.second.validate(value);
                    std::cout << (isValid ? "✓" : "✗") << " " << std::left << std::setw(25) << key << ": "
                              << std::fixed << std::setprecision(4) << value << " - " << rule.second.description << std::endl;
                }
            } else if (rule.second.isRequired) {
                std::cout << "✗ " << std::left << std::setw(25) << key << ": (missing)" 
                          << rule.second.description << std::endl;
            }
        }
    }
};

// Configuration file manager for enhanced file handling
class ConfigFileManager {
private:
    std::string defaultConfigDir;
    std::vector<std::string> configSearchPaths;
    ConfigValidator validator;
    
public:
    ConfigFileManager(const std::string& configDir = "configs") 
        : defaultConfigDir(configDir), configSearchPaths({".", "configs", "./configs", "~/nlm/configs"}) {}
    
    // Load configuration with enhanced error handling and validation
    std::shared_ptr<Config> loadConfig(const std::string& filepath, bool validate = true) {
        auto config = std::make_shared<Config>();
        
        // Try to load from file
        if (!config->loadFromFile(filepath)) {
            std::cerr << "[CONFIG WARNING] Could not load configuration from " << filepath << std::endl;
            std::cerr << "[CONFIG INFO] Using default configuration" << std::endl;
        }
        
        // Apply default values if file loading failed
        if (!config->has("neuron_count")) {
            config->set("neuron_count", 1000);
        }
        if (!config->has("region_count")) {
            config->set("region_count", 2);
        }
        if (!config->has("connection_probability")) {
            config->set("connection_probability", 0.1f);
        }
        if (!config->has("simulation_timestep")) {
            config->set("simulation_timestep", 0.001);
        }
        
        // Validate configuration
        if (validate) {
            std::vector<std::string> errors = validator.validateAll(config);
            if (!errors.empty()) {
                std::cerr << "[CONFIG ERROR] Validation errors:" << std::endl;
                for (const auto& error : errors) {
                    std::cerr << "  - " << error << std::endl;
                }
                throw ConfigValidationError("Configuration validation failed");
            }
        }
        
        return config;
    }
    
    // Load configuration with preset profiles
    std::shared_ptr<Config> loadConfigWithProfile(const std::string& profile) {
        auto config = std::make_shared<Config>();
        
        if (profile == "quick") {
            // Quick test configuration
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            config->set("connection_probability", 0.05);
            config->set("simulation_timestep", 0.001);
            config->set("stdp_ltp_weight", 0.02);
            config->set("stdp_ltd_weight", 0.012);
            std::cout << "[CONFIG PROFILE] Quick test profile loaded" << std::endl;
        } 
        else if (profile == "thorough") {
            // Thorough test configuration
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1);
            config->set("simulation_timestep", 0.001);
            config->set("stdp_ltp_weight", 0.01);
            config->set("stdp_ltd_weight", 0.012);
            config->set("synaptogenesis_rate", 0.0001);
            config->set("pruning_rate", 0.00001);
            std::cout << "[CONFIG PROFILE] Thorough test profile loaded" << std::endl;
        } 
        else if (profile == "ultra") {
            // Ultra comprehensive configuration
            config->set("neuron_count", 2000);
            config->set("region_count", 3);
            config->set("connection_probability", 0.15);
            config->set("simulation_timestep", 0.0005);
            config->set("stdp_ltp_weight", 0.008);
            config->set("stdp_ltd_weight", 0.018);
            config->set("stdp_tau", 25.0);
            config->set("synaptogenesis_rate", 0.0002);
            config->set("pruning_rate", 0.00002);
            config->set("dopamine_baseline", 0.2);
            config->set("curiosity_baseline", 0.5);
            std::cout << "[CONFIG PROFILE] Ultra comprehensive profile loaded" << std::endl;
        } else {
            // Default profile
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1);
            config->set("simulation_timestep", 0.001);
            std::cout << "[CONFIG PROFILE] Default profile loaded" << std::endl;
        }
        
        return config;
    }
    
    // Save configuration to file with validation
    bool saveConfig(const std::shared_ptr<Config>& config, const std::string& filepath) {
        try {
            std::vector<std::string> errors = validator.validateAll(config);
            if (!errors.empty()) {
                std::cerr << "[CONFIG ERROR] Cannot save invalid configuration:" << std::endl;
                for (const auto& error : errors) {
                    std::cerr << "  - " << error << std::endl;
                }
                return false;
            }
            
            if (config->saveToFile(filepath)) {
                std::cout << "[CONFIG SUCCESS] Configuration saved to " << filepath << std::endl;
                return true;
            } else {
                std::cerr << "[CONFIG ERROR] Failed to save configuration to " << filepath << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "[CONFIG ERROR] Exception while saving configuration: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Find configuration file in search paths
    std::string findConfigFile(const std::string& filename) {
        for (const auto& path : configSearchPaths) {
            std::string fullPath = path + "/" + filename;
            // In a real implementation, we would check if file exists
            // For now, we'll just return the first path as an example
            if (path == "." || path == "configs") {
                return fullPath;
            }
        }
        return filename; // Return as-is, letting the caller handle it
    }
}; 

// Configuration preset manager for commonly used configurations
class ConfigPresets {
public:
    static std::shared_ptr<Config> getPreset(const std::string& presetName) {
        auto config = std::make_shared<Config>();
        
        if (presetName == "research") {
            // Research-grade configuration for scientific experiments
            config->set("neuron_count", 2000);
            config->set("region_count", 3);
            config->set("connection_probability", 0.12);
            config->set("random_seed", static_cast<int64_t>(42));
            config->set("simulation_timestep", 0.0005);
            config->set("stdp_ltp_weight", 0.008);
            config->set("stdp_ltd_weight", 0.018);
            config->set("stdp_tau", 25.0);
            config->set("synaptogenesis_rate", 0.0002);
            config->set("pruning_rate", 0.00002);
            config->set("dopamine_baseline", 0.2);
            config->set("curiosity_baseline", 0.5);
            config->set("novelty_threshold", 0.3);
            config->set("environment_width", 50);
            config->set("environment_height", 50);
            config->set("vision_width", 32);
            config->set("vision_height", 32);
        } 
        else if (presetName == "performance") {
            // Performance-optimized configuration
            config->set("neuron_count", 500);
            config->set("region_count", 2);
            config->set("connection_probability", 0.08);
            config->set("random_seed", static_cast<int64_t>(123));
            config->set("simulation_timestep", 0.001);
            config->set("stdp_ltp_weight", 0.01);
            config->set("stdp_ltd_weight", 0.012);
            config->set("synaptogenesis_rate", 0.00005);
            config->set("pruning_rate", 0.000005);
            config->set("dopamine_baseline", 0.1);
        } 
        else if (presetName == "learning") {
            // Learning-focused configuration
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.15);
            config->set("random_seed", static_cast<int64_t>(999));
            config->set("simulation_timestep", 0.001);
            config->set("stdp_ltp_weight", 0.02);
            config->set("stdp_ltd_weight", 0.015);
            config->set("stdp_tau", 30.0);
            config->set("synaptogenesis_rate", 0.0003);
            config->set("pruning_rate", 0.00001);
            config->set("dopamine_baseline", 0.3);
            config->set("curiosity_baseline", 0.7);
            config->set("novelty_threshold", 0.2);
        } 
        else {
            // Default configuration
            config->set("neuron_count", 1000);
            config->set("region_count", 2);
            config->set("connection_probability", 0.1);
            config->set("simulation_timestep", 0.001);
        }
        
        return config;
    }
    
    static std::vector<std::string> getAvailablePresets() {
        return {"research", "performance", "learning", "default"};
    }
    
    static void printAvailablePresets() {
        std::cout << "Available Configuration Presets:" << std::endl;
        std::cout << "  research   - Scientific research configuration with comprehensive settings" << std::endl;
        std::cout << "  performance - Optimized for computational performance" << std::endl;
        std::cout << "  learning   - Focused on learning and adaptation" << std::endl;
        std::cout << "  default    - Balanced general-purpose configuration" << std::endl;
    }
};

// Configuration helper functions
inline void printConfigurationSummary(const std::shared_ptr<Config>& config) {
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    std::cout << "  Neurons: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
    std::cout << "  Regions: " << config->getOr<int64_t>("region_count", 2) << std::endl;
    std::cout << "  Connections: " << config->getOr<float>("connection_probability", 0.1) << std::endl;
    std::cout << "  Time Step: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << std::endl;
}

inline bool isConfigurationValid(const std::shared_ptr<Config>& config) {
    ConfigValidator validator;
    std::vector<std::string> errors = validator.validateAll(config);
    return errors.empty();
}

inline std::string getConfigurationErrorReport(const std::shared_ptr<Config>& config) {
    ConfigValidator validator;
    std::vector<std::string> errors = validator.validateAll(config);
    
    std::stringstream report;
    if (errors.empty()) {
        report << "Configuration is valid." << std::endl;
    } else {
        report << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            report << "  - " << error << std::endl;
        }
    }
    return report.str();
}

} // namespace nlm

#endif // NLM_CONFIG_SYSTEM_H
