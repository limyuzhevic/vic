// NLM (熙然) - Neural Learning Machine CLI Configuration Manager
//
// This file implements the configuration management system for the CLI,
// including loading from files, applying command-line overrides, saving to files,
// and applying configuration presets.

#include "cli/ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {
namespace cli {

ConfigManager::ConfigManager() {
    config = std::make_shared<Config>();
    loadPresets();
}

bool ConfigManager::loadFromFile(const std::string& filename) {
    NLM_LOG_INFO("Loading configuration from: " + filename);
    
    if (config->loadFromFile(filename)) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filename);
        return true;
    }
    else {
        NLM_LOG_WARNING("Failed to load configuration from: " + filename);
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filename) const {
    NLM_LOG_INFO("Saving configuration to: " + filename);
    
    if (config->saveToFile(filename)) {
        NLM_LOG_INFO("Successfully saved configuration to: " + filename);
        return true;
    }
    else {
        NLM_LOG_ERROR("Failed to save configuration to: " + filename);
        return false;
    }
}

bool ConfigManager::applyOverrides(const std::map<std::string, std::string>& options) {
    bool success = true;
    
    for (const auto& pair : options) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        NLM_LOG_INFO("Applying override: " + key + " = " + value);
        
        try {
            // Try to parse as integer
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::CommandLine);
            }
            catch (...) {
                // Try to parse as float
                try {
                    float floatValue = std::stof(value);
                    config->set(key, floatValue, ConfigSource::CommandLine);
                }
                catch (...) {
                    // Try to parse as boolean
                    try {
                        bool boolValue;
                        if (value == "true" || value == "1" || value == "yes") {
                            boolValue = true;
                        }
                        else if (value == "false" || value == "0" || value == "no") {
                            boolValue = false;
                        }
                        else {
                            throw std::invalid_argument("Invalid boolean value: " + value);
                        }
                        config->set(key, boolValue, ConfigSource::CommandLine);
                    }
                    catch (...) {
                        // Treat as string
                        config->set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
        catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to apply override " + key + " = " + value + ": " + std::string(e.what()));
            success = false;
        }
    }
    
    return success;
}

bool ConfigManager::validate() const {
    return config->validate().valid;
}

bool ConfigManager::applyPreset(const std::string& presetName) {
    if (presets.find(presetName) == presets.end()) {
        NLM_LOG_ERROR("Unknown preset: " + presetName);
        return false;
    }
    
    NLM_LOG_INFO("Applying preset: " + presetName);
    
    bool success = true;
    const auto& preset = presets.at(presetName);
    
    for (const auto& pair : preset) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        try {
            // Try to parse as integer
            try {
                int64_t intValue = std::stoll(value);
                config->set(key, intValue, ConfigSource::Preset);
            }
            catch (...) {
                // Try to parse as float
                try {
                    float floatValue = std::stof(value);
                    config->set(key, floatValue, ConfigSource::Preset);
                }
                catch (...) {
                    // Try to parse as boolean
                    try {
                        bool boolValue;
                        if (value == "true" || value == "1" || value == "yes") {
                            boolValue = true;
                        }
                        else if (value == "false" || value == "0" || value == "no") {
                            boolValue = false;
                        }
                        else {
                            throw std::invalid_argument("Invalid boolean value: " + value);
                        }
                        config->set(key, boolValue, ConfigSource::Preset);
                    }
                    catch (...) {
                        // Treat as string
                        config->set(key, value, ConfigSource::Preset);
                    }
                }
            }
        }
        catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to apply preset value " + key + " = " + value + ": " + std::string(e.what()));
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully applied preset: " + presetName);
    }
    
    return success;
}

std::vector<std::string> ConfigManager::getAvailablePresets() {
    static std::vector<std::string> presetNames;
    if (presetNames.empty()) {
        ConfigManager temp;
        for (const auto& pair : temp.presets) {
            presetNames.push_back(pair.first);
        }
    }
    return presetNames;
}

void ConfigManager::loadPresets() {
    NLM_LOG_INFO("Loading built-in configuration presets");
    
    // Basic preset
    presets["basic"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "500"},
        {"region_count", "1"},
        {"connection_probability", "0.15"},
        {"plasticity_learning_rate", "0.01"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"plasticity_structural_enable", "false"},
        {"max_simulation_steps", "1000"}
    };
    
    // Plasticity preset
    presets["plasticity"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "1000"},
        {"region_count", "2"},
        {"connection_probability", "0.1"},
        {"plasticity_learning_rate", "0.02"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_stdp_ltp_weight", "0.02"},
        {"plasticity_stdp_ltd_weight", "0.015"},
        {"plasticity_stdp_tau", "20.0"},
        {"plasticity_hebbian_enable", "true"},
        {"plasticity_structural_enable", "true"},
        {"synaptogenesis_rate", "0.0001"},
        {"pruning_rate", "0.00001"},
        {"max_simulation_steps", "5000"}
    };
    
    // Development preset
    presets["development"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "800"},
        {"region_count", "2"},
        {"connection_probability", "0.12"},
        {"plasticity_learning_rate", "0.01"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"plasticity_structural_enable", "true"},
        {"development_synaptogenesis_rate", "0.001"},
        {"development_pruning_rate", "0.0001"},
        {"development_critical_period", "100"},
        {"max_simulation_steps", "10000"}
    };
    
    // Cognitive preset
    presets["cognitive"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "2000"},
        {"region_count", "4"},
        {"connection_probability", "0.08"},
        {"plasticity_learning_rate", "0.005"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"plasticity_structural_enable", "true"},
        {"neuromodulation_curiosity_enable", "true"},
        {"neuromodulation_reward_enable", "true"},
        {"neuromodulation_novelty_enable", "true"},
        {"development_enable", "true"},
        {"memory_working_memory_enable", "true"},
        {"memory_episodic_memory_enable", "true"},
        {"max_simulation_steps", "20000"}
    };
    
    // Social preset
    presets["social"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "1500"},
        {"region_count", "3"},
        {"connection_probability", "0.09"},
        {"plasticity_learning_rate", "0.01"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"neuromodulation_reward_enable", "true"},
        {"neuromodulation_curiosity_enable", "true"},
        {"behavior_social_reward", "true"},
        {"behavior_cooperation", "true"},
        {"behavior_competition", "true"},
        {"social_network_size", "10"},
        {"social_interaction_frequency", "0.5"},
        {"max_simulation_steps", "8000"}
    };
    
    // Memory preset
    presets["memory"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "3000"},
        {"region_count", "2"},
        {"connection_probability", "0.06"},
        {"plasticity_learning_rate", "0.002"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"memory_working_memory_enable", "true"},
        {"memory_episodic_memory_enable", "true"},
        {"memory_consolidation_rate", "0.01"},
        {"memory_forgetting_rate", "0.001"},
        {"memory_reinforcement_threshold", "0.8"},
        {"max_simulation_steps", "50000"}
    };
    
    // Attention preset
    presets["attention"] = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "1200"},
        {"region_count", "3"},
        {"connection_probability", "0.11"},
        {"plasticity_learning_rate", "0.015"},
        {"plasticity_stdp_enable", "true"},
        {"plasticity_hebbian_enable", "true"},
        {"neuromodulation_attention_enable", "true"},
        {"attention_focus_strength", "0.7"},
        {"attention_spread_strength", "0.3"},
        {"attention_boost_factor", "1.5"},
        {"attention_decay_rate", "0.05"},
        {"max_simulation_steps", "6000"}
    };
    
    NLM_LOG_INFO("Loaded " + std::to_string(presets.size()) + " built-in presets");
}

} // namespace cli
} // namespace nlm
