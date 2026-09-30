#pragma once

#include "EnhancedConfig.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Implementation of enhanced configuration features

// EnhancedConfigurationBuilder implementation
EnhancedConfigurationBuilder& EnhancedConfigurationBuilder::brain(size_t neuronCount, 
                                                           float synapseDensity,
                                                           float vThresh,
                                                           float vRest) {
    config_->set("brain.neuron_count", neuronCount);
    config_->set("brain.synapse_density", synapseDensity);
    config_->set("brain.v_thresh", vThresh);
    config_->set("brain.v_rest", vRest);
    return *this;
}

EnhancedConfigurationBuilder& EnhancedConfigurationBuilder::plasticity(bool enableSTDP,
                                                           float stdpLearningRate,
                                                           bool enableHebbian,
                                                           bool enableStructural) {
    config_->set("plasticity.stdp.enable", enableSTDP);
    config_->set("plasticity.stdp.learning_rate", stdpLearningRate);
    config_->set("plasticity.hebbian.enable", enableHebbian);
    config_->set("plasticity.structural.enable", enableStructural);
    return *this;
}

EnhancedConfigurationBuilder& EnhancedConfigurationBuilder::neuromodulation(bool enableCuriosity,
                                                           bool enableNovelty,
                                                           float dopamineScale) {
    config_->set("neuromod.curiosity.enable", enableCuriosity);
    config_->set("neuromod.novelty.enable", enableNovelty);
    config_->set("neuromod.dopamine.scale", dopamineScale);
    return *this;
}

EnhancedConfigurationBuilder& EnhancedConfigurationBuilder::development(bool enableDevelopment,
                                                           float criticalPeriodStart,
                                                           float criticalPeriodEnd) {
    config_->set("development.enabled", enableDevelopment);
    config_->set("development.critical_period_start", criticalPeriodStart);
    config_->set("development.critical_period_end", criticalPeriodEnd);
    return *this;
}

EnhancedConfigurationBuilder& EnhancedConfigurationBuilder::simulation(double timestep,
                                                           size_t maxSteps) {
    config_->set("simulation.time.step_size", timestep);
    config_->set("simulation.max_steps", maxSteps);
    return *this;
}

// Configuration IO implementation
bool ConfigurationIO::saveConfig(const Config& config, const std::string& filename) {
    return config.saveToFile(filename);
}

bool ConfigurationIO::loadConfig(Config& config, const std::string& filename) {
    return config.loadFromFile(filename);
}

bool ConfigurationIO::exportConfigToJSON(const Config& config, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "{\n";
    
    auto keys = config.getKeys();
    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& key = keys[i];
        file << "  \"" << key << "\": \"" << config.get<std::string>(key) << "\"";
        
        if (i < keys.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    file.close();
    
    return true;
}

bool ConfigurationIO::importConfigFromJSON(Config& config, const std::string& filename) {
    return config.loadFromFile(filename);
}

// ConfigurationComparison implementation
std::vector<std::string> ConfigurationComparison::findDifferences(const Config& config1, const Config& config2) {
    std::vector<std::string> differences;
    
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    // Find keys in config1 but not in config2
    for (const auto& key : keys1) {
        if (std::find(keys2.begin(), keys2.end(), key) == keys2.end()) {
            differences.push_back("Missing in config2: " + key);
        } else if (config1.get<std::string>(key) != config2.get<std::string>(key)) {
            differences.push_back("Different value for key " + key + ": " +
                config1.get<std::string>(key) + " vs " + config2.get<std::string>(key));
        }
    }
    
    // Find keys in config2 but not in config1
    for (const auto& key : keys2) {
        if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
            differences.push_back("Missing in config1: " + key);
        }
    }
    
    return differences;
}

bool ConfigurationComparison::isConfigSubset(const Config& subset, const Config& superset) {
    auto keys = subset.getKeys();
    
    for (const auto& key : keys) {
        if (!superset.has(key) || subset.get<std::string>(key) != superset.get<std::string>(key)) {
            return false;
        }
    }
    
    return true;
}

Config ConfigurationComparison::mergeConfigs(const Config& config1, const Config& config2) {
    Config merged = config1;
    auto keys = config2.getKeys();
    
    for (const auto& key : keys) {
        if (config2.has(key)) {
            merged.set(key, config2.get<std::string>(key));
        }
    }
    
    return merged;
}

// ConfigurationValidator implementation
bool ConfigurationValidator::validateBrainConfig(const Config& config) {
    bool valid = true;
    
    if (!config.has("brain.neuron_count") || 
        config.get<size_t>("brain.neuron_count", 0) == 0) {
        NLM_LOG_ERROR("Brain neuron count must be specified and > 0");
        valid = false;
    }
    
    if (config.has("brain.neuron_count")) {
        size_t neurons = config.get<size_t>("brain.neuron_count", 1000);
        if (neurons > 100000) {
            NLM_LOG_WARNING("High neuron count may impact performance: " + std::to_string(neurons));
        }
    }
    
    if (config.has("brain.v_thresh") && config.has("brain.v_rest")) {
        float thresh = config.get<float>("brain.v_thresh", -50.0f);
        float rest = config.get<float>("brain.v_rest", -70.0f);
        if (thresh >= rest) {
            NLM_LOG_ERROR("Neuron threshold must be less than resting potential");
            valid = false;
        }
    }
    
    return valid;
}

bool ConfigurationValidator::validatePlasticityConfig(const Config& config) {
    if (config.has("plasticity.stdp.enable") && 
        config.get<bool>("plasticity.stdp.enable")) {
        
        if (config.has("plasticity.stdp.learning_rate")) {
            float lr = config.get<float>("plasticity.stdp.learning_rate", 0.001f);
            if (lr < 0.0f || lr > 1.0f) {
                NLM_LOG_ERROR("STDP learning rate must be between 0.0 and 1.0");
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigurationValidator::validateNeuromodulationConfig(const Config& config) {
    if (config.has("neuromod.curiosity.enable") &&
        config.get<bool>("neuromod.curiosity.enable")) {
        
        if (config.has("neuromod.curiosity.base_rate")) {
            float rate = config.get<float>("neuromod.curiosity.base_rate", 0.01f);
            if (rate < 0.0f || rate > 1.0f) {
                NLM_LOG_ERROR("Curiosity base rate must be between 0.0 and 1.0");
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigurationValidator::validateDevelopmentConfig(const Config& config) {
    if (config.has("development.enabled") &&
        config.get<bool>("development.enabled")) {
        
        if (config.has("development.critical_period_start") &&
            config.has("development.critical_period_end")) {
            
            float start = config.get<float>("development.critical_period_start", 10.0f);
            float end = config.get<float>("development.critical_period_end", 100.0f);
            
            if (start < 0.0f || end < 0.0f) {
                NLM_LOG_ERROR("Development periods must be positive");
                return false;
            }
            
            if (start >= end) {
                NLM_LOG_ERROR("Critical period start must be less than end");
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigurationValidator::validateAllConfig(const Config& config) {
    bool valid = true;
    
    valid &= validateBrainConfig(config);
    valid &= validatePlasticityConfig(config);
    valid &= validateNeuromodulationConfig(config);
    valid &= validateDevelopmentConfig(config);
    
    if (!valid) {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return valid;
}

} // namespace nlm
