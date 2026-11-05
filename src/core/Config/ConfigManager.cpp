// Implementation of advanced configuration management for NLM

#include "ConfigManager.hpp"
#include "Config.hpp"
#include "brain/Brain.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Advanced configuration options with descriptions
static const std::unordered_map<std::string, std::pair<std::string, ConfigCategory>> advancedOptions = {
    // Neural configuration
    {"neuron_excitability", "Controls baseline neuron excitability (0.0 to 1.0)", ConfigCategory::Neural}},
    {"neuron_threshold", "Sets neuron firing threshold (0.0 to 1.0)", ConfigCategory::Neural}},
    {"neuron_leak", "Sets neuron leak conductance (0.0 to 1.0)", ConfigCategory::Neural}},
    {"neuron_reset_potential", "Sets neuron reset potential (-1.0 to 1.0)", ConfigCategory::Neural}},
    
    // Plasticity configuration
    {"stdp_learning_rate", "STDP learning rate (0.0 to 0.1)", ConfigCategory::Plasticity}},
    {"stdp_time_constant", "STDP time constant for spike history (1 to 100)", ConfigCategory::Plasticity}},
    {"hebbian_strength", "Hebbian learning strength (0.0 to 1.0)", ConfigCategory::Plasticity}},
    {"structural_plasticity_rate", "Structural plasticity rate (0.0 to 0.001)", ConfigCategory::Plasticity}},
    
    // Memory configuration
    {"working_memory_capacity", "Working memory capacity in neurons (10 to 1000)", ConfigCategory::Memory}},
    {"episodic_memory_limit", "Maximum episodic memory episodes (10 to 10000)", ConfigCategory::Memory}},
    {"associative_memory_strength", "Associative memory strength (0.0 to 1.0)", ConfigCategory::Memory}},
    
    // Neuromodulation configuration
    {"dopamine_baseline", "Baseline dopamine level (0.0 to 1.0)", ConfigCategory::Neuromodulation}},
    {"dopamine_sensitivity", "Dopamine sensitivity to reward (0.0 to 1.0)", ConfigCategory::Neuromodulation}},
    {"curiosity_threshold", "Curiosity threshold for exploration (0.0 to 1.0)", ConfigCategory::Neuromodulation}},
    {"novelty_decay_rate", "Novelty decay rate (0.0 to 1.0)", ConfigCategory::Neuromodulation}},
    
    // Cognition configuration
    {"planner_planning_depth", "Planner planning depth (1 to 10)", ConfigCategory::Cognition}},
    {"concept_formation_threshold", "Concept formation threshold (0.0 to 1.0)", ConfigCategory::Cognition}},
    {"attention_strength", "Attention strength (0.0 to 1.0)", ConfigCategory::Cognition}},
    {"neural_planning_enabled", "Enable neural planning (true/false)", ConfigCategory::Cognition}},
    
    // Development configuration
    {"developmental_plasticity_rate", "Developmental plasticity rate (0.0 to 1.0)", ConfigCategory::Development}},
    {"critical_period_duration", "Critical period duration in steps (0 to 10000)", ConfigCategory::Development}},
    {"maturation_rate", "Maturation rate (0.0 to 1.0)", ConfigCategory::Development}},
    {"development_stages", "Development stages (comma-separated list)", ConfigCategory::Development}},
    
    // Performance configuration
    {"spike_propagation_speed", "Spike propagation speed factor (0.1 to 10.0)", ConfigCategory::Performance}},
    {"plasticity_update_frequency", "Plasticity update frequency in steps (0 to 100)", ConfigCategory::Performance}},
    {"memory_update_frequency", "Memory update frequency in steps (0 to 100)", ConfigCategory::Performance}},
    {"checkpointer_frequency", "Checkpoint frequency in steps (0 to 100000)", ConfigCategory::Performance}},
    
    // Checkpoint configuration
    {"checkpoint_compression", "Checkpoint compression level (0=off, 1=balanced, 2=max)", ConfigCategory::Checkpoint}},
    {"checkpoint_backup_count", "Number of checkpoint backups to keep (0 to 100)", ConfigCategory::Checkpoint}},
    {"checkpoint_auto_save", "Enable automatic checkpoint saving (true/false)", ConfigCategory::Checkpoint}},
    {"checkpoint_integrity_check", "Enable checkpoint integrity verification (true/false)", ConfigCategory::Checkpoint}},
    
    // Reward configuration
    {"reward_computation_method", "Reward computation method (classic/phasic/actor-critic)", ConfigCategory::Reward}},
    {"reward_scale_factor", "Reward scale factor (0.0 to 10.0)", ConfigCategory::Reward}},
    {"reward_decay_rate", "Reward decay rate (0.0 to 1.0)", ConfigCategory::Reward}},
    {"prediction_error_weight", "Prediction error weight (0.0 to 1.0)", ConfigCategory::Reward}},
    
    // Sensory/Motor configuration
    {"vision_sensitivity", "Vision sensory sensitivity (0.0 to 1.0)", ConfigCategory::SensoryMotor}},
    {"touch_sensitivity", "Touch sensory sensitivity (0.0 to 1.0)", ConfigCategory::SensoryMotor}},
    {"proprioception_sensitivity", "Proprioception sensitivity (0.0 to 1.0)", ConfigCategory::SensoryMotor}},
    {"motor_noise_level", "Motor noise level (0.0 to 1.0)", ConfigCategory::SensoryMotor}},
    
    // Environment configuration
    {"environment_complexity", "Environment complexity (0=simple, 1=complex)", ConfigCategory::Environment}},
    {"reward_distribution", "Reward distribution (sparse/dense)", ConfigCategory::Environment}},
    {"task_difficulty", "Task difficulty level (0.0 to 1.0)", ConfigCategory::Environment}},
};

void ConfigManager::initializeDefaults(Config& config) {
    // Neural defaults
    config.set("neuron_excitability", 0.5, ConfigSource::Default);
    config.set("neuron_threshold", 0.5, ConfigSource::Default);
    config.set("neuron_leak", 0.05, ConfigSource::Default);
    config.set("neuron_reset_potential", -0.7, ConfigSource::Default);
    
    // Plasticity defaults
    config.set("stdp_learning_rate", 0.01, ConfigSource::Default);
    config.set("stdp_time_constant", 20.0, ConfigSource::Default);
    config.set("hebbian_strength", 0.5, ConfigSource::Default);
    config.set("structural_plasticity_rate", 0.0001, ConfigSource::Default);
    
    // Memory defaults
    config.set("working_memory_capacity", 100, ConfigSource::Default);
    config.set("episodic_memory_limit", 1000, ConfigSource::Default);
    config.set("associative_memory_strength", 0.3, ConfigSource::Default);
    
    // Neuromodulation defaults
    config.set("dopamine_baseline", 0.1, ConfigSource::Default);
    config.set("dopamine_sensitivity", 0.5, ConfigSource::Default);
    config.set("curiosity_threshold", 0.3, ConfigSource::Default);
    config.set("novelty_decay_rate", 0.05, ConfigSource::Default);
    
    // Cognition defaults
    config.set("planner_planning_depth", 3, ConfigSource::Default);
    config.set("concept_formation_threshold", 0.7, ConfigSource::Default);
    config.set("attention_strength", 0.5, ConfigSource::Default);
    config.set("neural_planning_enabled", true, ConfigSource::Default);
    
    // Development defaults
    config.set("developmental_plasticity_rate", 1.0, ConfigSource::Default);
    config.set("critical_period_duration", 1000, ConfigSource::Default);
    config.set("maturation_rate", 0.5, ConfigSource::Default);
    config.set("development_stages", "Initial,CriticalPeriod,Maturation,Adult", ConfigSource::Default);
    
    // Performance defaults
    config.set("spike_propagation_speed", 1.0, ConfigSource::Default);
    config.set("plasticity_update_frequency", 1, ConfigSource::Default);
    config.set("memory_update_frequency", 10, ConfigSource::Default);
    config.set("checkpointer_frequency", 10000, ConfigSource::Default);
    
    // Checkpoint defaults
    config.set("checkpoint_compression", 1, ConfigSource::Default);
    config.set("checkpoint_backup_count", 10, ConfigSource::Default);
    config.set("checkpoint_auto_save", true, ConfigSource::Default);
    config.set("checkpoint_integrity_check", true, ConfigSource::Default);
    
    // Reward defaults
    config.set("reward_computation_method", "classic", ConfigSource::Default);
    config.set("reward_scale_factor", 1.0, ConfigSource::Default);
    config.set("reward_decay_rate", 0.9, ConfigSource::Default);
    config.set("prediction_error_weight", 0.5, ConfigSource::Default);
    
    // Sensory/Motor defaults
    config.set("vision_sensitivity", 1.0, ConfigSource::Default);
    config.set("touch_sensitivity", 1.0, ConfigSource::Default);
    config.set("proprioception_sensitivity", 1.0, ConfigSource::Default);
    config.set("motor_noise_level", 0.1, ConfigSource::Default);
    
    // Environment defaults
    config.set("environment_complexity", 0.5, ConfigSource::Default);
    config.set("reward_distribution", "sparse", ConfigSource::Default);
    config.set("task_difficulty", 0.5, ConfigSource::Default);
}

bool ConfigManager::validate(const Config& config, std::string& errorMessage) const {
    bool isValid = true;
    
    // Validate ranges for key configuration options
    auto neuronExc = config.getOr<double>("neuron_excitability", 0.5);
    if (neuronExc < 0.0 || neuronExc > 1.0) {
        errorMessage = "neuron_excitability must be between 0.0 and 1.0";
        isValid = false;
    }
    
    auto neuronThresh = config.getOr<double>("neuron_threshold", 0.5);
    if (neuronThresh < 0.0 || neuronThresh > 1.0) {
        errorMessage = "neuron_threshold must be between 0.0 and 1.0";
        isValid = false;
    }
    
    auto stdpRate = config.getOr<double>("stdp_learning_rate", 0.01);
    if (stdpRate < 0.0 || stdpRate > 0.1) {
        errorMessage = "stdp_learning_rate must be between 0.0 and 0.1";
        isValid = false;
    }
    
    auto stdpTimeConst = config.getOr<double>("stdp_time_constant", 20.0);
    if (stdpTimeConst < 1.0 || stdpTimeConst > 100.0) {
        errorMessage = "stdp_time_constant must be between 1.0 and 100.0";
        isValid = false;
    }
    
    auto workingMemCap = config.getOr<int64_t>("working_memory_capacity", 100);
    if (workingMemCap < 10 || workingMemCap > 1000) {
        errorMessage = "working_memory_capacity must be between 10 and 1000";
        isValid = false;
    }
    
    auto episodicMemLimit = config.getOr<int64_t>("episodic_memory_limit", 1000);
    if (episodicMemLimit < 10 || episodicMemLimit > 10000) {
        errorMessage = "episodic_memory_limit must be between 10 and 10000";
        isValid = false;
    }
    
    auto dopamineBase = config.getOr<double>("dopamine_baseline", 0.1);
    if (dopamineBase < 0.0 || dopamineBase > 1.0) {
        errorMessage = "dopamine_baseline must be between 0.0 and 1.0";
        isValid = false;
    }
    
    auto curiosityThreshold = config.getOr<double>("curiosity_threshold", 0.3);
    if (curiosityThreshold < 0.0 || curiosityThreshold > 1.0) {
        errorMessage = "curiosity_threshold must be between 0.0 and 1.0";
        isValid = false;
    }
    
    auto plannerDepth = config.getOr<int64_t>("planner_planning_depth", 3);
    if (plannerDepth < 1 || plannerDepth > 10) {
        errorMessage = "planner_planning_depth must be between 1 and 10";
        isValid = false;
    }
    
    auto checkpointFreq = config.getOr<int64_t>("checkpointer_frequency", 10000);
    if (checkpointFreq < 0 || checkpointFreq > 100000) {
        errorMessage = "checkpointer_frequency must be between 0 and 100000";
        isValid = false;
    }
    
    auto checkpointComp = config.getOr<int64_t>("checkpoint_compression", 1);
    if (checkpointComp < 0 || checkpointComp > 2) {
        errorMessage = "checkpoint_compression must be between 0 and 2";
        isValid = false;
    }
    
    auto rewardScale = config.getOr<double>("reward_scale_factor", 1.0);
    if (rewardScale < 0.0 || rewardScale > 10.0) {
        errorMessage = "reward_scale_factor must be between 0.0 and 10.0";
        isValid = false;
    }
    
    return isValid;
}

std::vector<ConfigEntry> ConfigManager::getByCategory(const Config& config, ConfigCategory category) {
    std::vector<ConfigEntry> result;
    
    for (const auto& entry : config.getKeys()) {
        auto configEntry = config.get<ConfigValue>(entry);
        if (!configEntry) continue;
        
        // Find category for this key
        auto it = advancedOptions.find(entry);
        if (it != advancedOptions.end() && it->second.second == category) {
            result.emplace_back(entry, *configEntry, ConfigSource::Runtime, "", category);
        }
    }
    
    return result;
}

bool ConfigManager::mergeConfig(Config& config, const std::string& filepath) {
    // Create a temporary config with file contents
    Config fileConfig;
    if (!fileConfig.loadFromFile(filepath)) {
        return false;
    }
    
    // Merge: command line/runtime config takes precedence over file config
    for (const auto& key : fileConfig.getKeys()) {
        auto fileValue = fileConfig.get<ConfigValue>(key);
        if (fileValue) {
            // Only set if key doesn't already exist in target config
            if (!config.has(key)) {
                config.set(key, *fileValue, ConfigSource::File);
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getAdvancedOptionNames() const {
    std::vector<std::string> names;
    names.reserve(advancedOptions.size());
    for (const auto& pair : advancedOptions) {
        names.push_back(pair.first);
    }
    std::sort(names.begin(), names.end());
    return names;
}

std::string ConfigManager::getAdvancedOptionHelp(const std::string& option) const {
    auto it = advancedOptions.find(option);
    if (it != advancedOptions.end()) {
        return it->second.first;
    }
    return "Unknown option: " + option;
}

bool ConfigManager::setAdvancedOption(Config& config, const std::string& option, const std::string& value) {
    auto it = advancedOptions.find(option);
    if (it == advancedOptions.end()) {
        return false;
    }
    
    // Parse value based on option type
    // This is a simple implementation - could be enhanced with proper type detection
    try {
        if (value.find('.') != std::string::npos) {
            // Try to parse as double first
            size_t pos;
            double doubleValue = std::stod(value, &pos);
            if (pos == value.size()) {
                config.set(option, doubleValue, ConfigSource::Runtime);
                return true;
            }
        } else if (value == "true" || value == "false") {
            config.set(option, value == "true", ConfigSource::Runtime);
            return true;
        } else {
            // Try to parse as int64_t
            size_t pos;
            int64_t intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                config.set(option, intValue, ConfigSource::Runtime);
                return true;
            }
        }
    } catch (...) {
        // Parsing failed
    }
    
    // If parsing failed, try to set as string
    config.set(option, value, ConfigSource::Runtime);
    return true;
}

} // namespace nlm
