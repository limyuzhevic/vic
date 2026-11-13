#include "ConfigValidator.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

std::vector<std::string> ConfigValidator::validate(const Config& config) {
    std::vector<std::string> errors;
    
    // Validate against NLM schema
    auto schemaErrors = validateAgainstSchema(config);
    errors.insert(errors.end(), schemaErrors.begin(), schemaErrors.end());
    
    // Validate ranges
    auto rangeErrors = validateAgainstRanges(config);
    errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
    
    // Validate dependencies
    auto dependencyErrors = checkDependencies(config);
    errors.insert(errors.end(), dependencyErrors.begin(), dependencyErrors.end());
    
    return errors;
}

void ConfigValidator::applyDefaults(Config& config) {
    const auto& defaults = NLMDefaultConfig::get();
    
    // Apply default values for missing keys
    auto setIfMissing = [&config](const std::string& key, const auto& value) {
        if (!config.has(key)) {
            config.set(key, value);
        }
    };
    
    // Neural configuration
    setIfMissing("neuron_count", defaults.neural.neuron_count);
    setIfMissing("region_count", defaults.neural.region_count);
    setIfMissing("connection_probability", defaults.neural.connection_probability);
    setIfMissing("mean_excitatory_weight", defaults.neural.mean_excitatory_weight);
    setIfMissing("mean_inhibitory_weight", defaults.neural.mean_inhibitory_weight);
    
    // STDP parameters
    setIfMissing("stdp_ltp_weight", defaults.stdp.ltp_weight);
    setIfMissing("stdp_ltd_weight", defaults.stdp.ltd_weight);
    setIfMissing("stdp_tau", defaults.stdp.tau);
    
    // Structural plasticity
    setIfMissing("synaptogenesis_rate", defaults.structuralPlasticity.synaptogenesis_rate);
    setIfMissing("pruning_rate", defaults.structuralPlasticity.pruning_rate);
    setIfMissing("max_synapses_per_neuron", defaults.structuralPlasticity.max_synapses_per_neuron);
    
    // Development
    setIfMissing("simulation_timestep", defaults.development.timestep);
    setIfMissing("plasticity_modifier", defaults.development.plasticity_modifier);
    setIfMissing("developmental_stage_multiplier", defaults.development.developmental_stage_multiplier);
    
    // Memory
    setIfMissing("working_memory_capacity", static_cast<int64_t>(defaults.memory.working_memory_capacity));
    setIfMissing("max_episodes", static_cast<int64_t>(defaults.memory.max_episodes));
    setIfMissing("consolidation_threshold", defaults.memory.consolidation_threshold);
    setIfMissing("replay_interval", static_cast<int64_t>(defaults.memory.replay_interval));
    setIfMissing("consolidation_interval", static_cast<int64_t>(defaults.memory.consolidation_interval));
    
    // Neuromodulation
    setIfMissing("dopamine_level", defaults.neuromodulation.dopamine_level);
    setIfMissing("curiosity_level", defaults.neuromodulation.curiosity_level);
    setIfMissing("novelty_level", defaults.neuromodulation.novelty_level);
    setIfMissing("prediction_error", defaults.neuromodulation.prediction_error);
    
    // Sensory
    setIfMissing("vision_width", static_cast<int64_t>(defaults.sensory.vision_width));
    setIfMissing("vision_height", static_cast<int64_t>(defaults.sensory.vision_height));
    setIfMissing("audio_sample_rate", static_cast<int64_t>(defaults.sensory.audio_sample_rate));
    setIfMissing("audio_num_samples", static_cast<int64_t>(defaults.sensory.audio_num_samples));
    setIfMissing("max_energy", defaults.sensory.max_energy);
    setIfMissing("energy_decay_rate", defaults.sensory.energy_decay_rate);
    setIfMissing("resource_energy_gain", defaults.sensory.resource_energy_gain);
    
    // World
    setIfMissing("world_width", defaults.world.width);
    setIfMissing("world_height", defaults.world.height);
    setIfMissing("world_max_energy", defaults.world.max_energy);
    setIfMissing("world_energy_decay_rate", defaults.world.energy_decay_rate);
    setIfMissing("world_resource_energy_gain", defaults.world.resource_energy_gain);
    
    // Random seed
    setIfMissing("random_seed", defaults.random_seed);
    
    // Checkpointing
    setIfMissing("checkpoint_dir", defaults.checkpoint.directory);
    setIfMissing("max_checkpoints", static_cast<int64_t>(defaults.checkpoint.max_checkpoints));
    setIfMissing("max_history", static_cast<int64_t>(defaults.checkpoint.max_history));
    setIfMissing("auto_save", defaults.checkpoint.auto_save);
}

std::string ConfigValidator::getConfigDocumentation(const std::string& key) {
    static const std::map<std::string, std::string> documentation = {
        // Neural configuration
        {"neuron_count", "Number of neurons in the brain (default: 1000)"},
        {"region_count", "Number of neural regions (default: 1)"},
        {"connection_probability", "Probability of synapses forming between neurons (default: 0.1)"},
        {"mean_excitatory_weight", "Mean weight of excitatory synapses (default: 0.2)"},
        {"mean_inhibitory_weight", "Mean weight of inhibitory synapses (default: -0.1)"},
        
        // STDP parameters
        {"stdp_ltp_weight", "STDP long-term potentiation weight (default: 0.01)"},
        {"stdp_ltd_weight", "STDP long-term depression weight (default: 0.012)"},
        {"stdp_tau", "STDP time constant (default: 20.0)"},
        
        // Structural plasticity
        {"synaptogenesis_rate", "Rate of new synapse formation (default: 0.0001)"},
        {"pruning_rate", "Rate of synapse removal (default: 0.00001)"},
        {"max_synapses_per_neuron", "Maximum number of synapses per neuron (default: 25)"},
        
        // Development
        {"simulation_timestep", "Simulation timestep in seconds (default: 0.001)"},
        {"plasticity_modifier", "Modifier for plasticity rate (default: 1.0)"},
        {"developmental_stage_multiplier", "Multiplier for developmental stage effects (default: 1.0)"},
        
        // Memory
        {"working_memory_capacity", "Capacity of working memory in neurons (default: 100)"},
        {"max_episodes", "Maximum number of episodic memories (default: 1000)"},
        {"consolidation_threshold", "Threshold for memory consolidation (default: 0.3)"},
        {"replay_interval", "Interval for memory replay (default: 100 steps)"},
        {"consolidation_interval", "Interval for memory consolidation (default: 1000 steps)"},
        
        // Neuromodulation
        {"dopamine_level", "Dopamine neuromodulation level (default: 0.5)"},
        {"curiosity_level", "Curiosity neuromodulation level (default: 0.5)"},
        {"novelty_level", "Novelty neuromodulation level (default: 0.5)"},
        {"prediction_error", "Prediction error level (default: 0.0)"},
        
        // Sensory
        {"vision_width", "Vision sensor width in pixels (default: 16)"},
        {"vision_height", "Vision sensor height in pixels (default: 16)"},
        {"audio_sample_rate", "Audio sample rate (default: 44100)"},
        {"audio_num_samples", "Number of audio samples (default: 1024)"},
        {"max_energy", "Maximum energy level (default: 100.0)"},
        {"energy_decay_rate", "Energy decay rate per timestep (default: 0.01)"},
        {"resource_energy_gain", "Energy gain from resources (default: 30.0)"},
        
        // World
        {"world_width", "World width in units (default: 20.0)"},
        {"world_height", "World height in units (default: 20.0)"},
        {"world_max_energy", "World maximum energy (default: 100.0)"},
        {"world_energy_decay_rate", "World energy decay rate (default: 0.01)"},
        {"world_resource_energy_gain", "World resource energy gain (default: 30.0)"},
        
        // Random seed
        {"random_seed", "Random seed for reproducible simulations (default: 42)"},
        
        // Checkpointing
        {"checkpoint_dir", "Directory for checkpoint files (default: ./checkpoints)"},
        {"max_checkpoints", "Maximum number of checkpoints to keep (default: 10000)"},
        {"max_history", "Maximum checkpoint history (default: 5)"},
        {"auto_save", "Enable automatic checkpoint saving (default: true)"}
    };
    
    if (key.empty()) {
        std::stringstream ss;
        ss << "NLM Configuration Documentation:\n\n";
        for (const auto& pair : documentation) {
            ss << pair.first << ": " << pair.second << "\n";
        }
        return ss.str();
    }
    
    auto it = documentation.find(key);
    if (it != documentation.end()) {
        return it->second;
    }
    
    return "No documentation available for configuration key: " + key;
}

std::vector<std::string> ConfigValidator::validateKey(const std::string& key, const ConfigValue& value) {
    std::vector<std::string> errors;
    
    // Validate based on key name
    if (key == "neuron_count" || key == "region_count" || key == "working_memory_capacity" || 
        key == "max_episodes" || key == "replay_interval" || key == "consolidation_interval" ||
        key == "vision_width" || key == "vision_height" || key == "audio_sample_rate" || 
        key == "audio_num_samples" || key == "max_checkpoints" || key == "max_history") {
        int64_t intValue;
        if (std::holds_alternative<int>(value)) {
            intValue = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            intValue = std::get<int64_t>(value);
        } else {
            errors.push_back("Key '" + key + "' must be an integer");
            return errors;
        }
        
        if (intValue < 0) {
            errors.push_back("Key '" + key + "' must be non-negative");
        }
    } 
    else if (key == "connection_probability" || key == "mean_excitatory_weight" ||
             key == "mean_inhibitory_weight" || key == "stdp_ltp_weight" ||
             key == "stdp_ltd_weight" || key == "stdp_tau" || key == "synaptogenesis_rate" ||
             key == "pruning_rate" || key == "max_synapses_per_neuron" ||
             key == "plasticity_modifier" || key == "developmental_stage_multiplier" ||
             key == "consolidation_threshold" || key == "dopamine_level" ||
             key == "curiosity_level" || key == "novelty_level" || key == "prediction_error" ||
             key == "energy_decay_rate" || key == "resource_energy_gain" ||
             key == "world_max_energy" || key == "world_energy_decay_rate" ||
             key == "world_resource_energy_gain") {
        double doubleValue;
        if (std::holds_alternative<double>(value)) {
            doubleValue = std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            doubleValue = static_cast<double>(std::get<float>(value));
        } else {
            errors.push_back("Key '" + key + "' must be a number");
            return errors;
        }
        
        if (std::isnan(doubleValue)) {
            errors.push_back("Key '" + key + "' cannot be NaN");
        }
    }
    
    else if (key == "simulation_timestep") {
        double timestep;
        if (std::holds_alternative<double>(value)) {
            timestep = std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            timestep = static_cast<double>(std::get<float>(value));
        } else {
            errors.push_back("Key 'simulation_timestep' must be a number");
            return errors;
        }
        
        if (timestep <= 0.0 || timestep > 1.0) {
            errors.push_back("Key 'simulation_timestep' must be between 0 and 1.0");
        }
    }
    
    else if (key == "random_seed") {
        int64_t seed;
        if (std::holds_alternative<int>(value)) {
            seed = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            seed = std::get<int64_t>(value);
        } else {
            errors.push_back("Key 'random_seed' must be an integer");
            return errors;
        }
        
        if (seed < 0) {
            errors.push_back("Key 'random_seed' must be non-negative");
        }
    }
    
    else if (key == "auto_save") {
        if (!std::holds_alternative<bool>(value)) {
            errors.push_back("Key 'auto_save' must be a boolean");
        }
    }
    
    else if (key == "checkpoint_dir") {
        if (!std::holds_alternative<std::string>(value)) {
            errors.push_back("Key 'checkpoint_dir' must be a string");
        }
    }
    
    return errors;
}

std::map<std::string, std::string> ConfigValidator::getAllConfigKeys() {
    return {
        // Neural configuration
        {"neuron_count", "Number of neurons in the brain (default: 1000)"},
        {"region_count", "Number of neural regions (default: 1)"},
        {"connection_probability", "Probability of synapses forming between neurons (default: 0.1)"},
        {"mean_excitatory_weight", "Mean weight of excitatory synapses (default: 0.2)"},
        {"mean_inhibitory_weight", "Mean weight of inhibitory synapses (default: -0.1)"},
        
        // STDP parameters
        {"stdp_ltp_weight", "STDP long-term potentiation weight (default: 0.01)"},
        {"stdp_ltd_weight", "STDP long-term depression weight (default: 0.012)"},
        {"stdp_tau", "STDP time constant (default: 20.0)"},
        
        // Structural plasticity
        {"synaptogenesis_rate", "Rate of new synapse formation (default: 0.0001)"},
        {"pruning_rate", "Rate of synapse removal (default: 0.00001)"},
        {"max_synapses_per_neuron", "Maximum number of synapses per neuron (default: 25)"},
        
        // Development
        {"simulation_timestep", "Simulation timestep in seconds (default: 0.001)"},
        {"plasticity_modifier", "Modifier for plasticity rate (default: 1.0)"},
        {"developmental_stage_multiplier", "Multiplier for developmental stage effects (default: 1.0)"},
        
        // Memory
        {"working_memory_capacity", "Capacity of working memory in neurons (default: 100)"},
        {"max_episodes", "Maximum number of episodic memories (default: 1000)"},
        {"consolidation_threshold", "Threshold for memory consolidation (default: 0.3)"},
        {"replay_interval", "Interval for memory replay (default: 100 steps)"},
        {"consolidation_interval", "Interval for memory consolidation (default: 1000 steps)"},
        
        // Neuromodulation
        {"dopamine_level", "Dopamine neuromodulation level (default: 0.5)"},
        {"curiosity_level", "Curiosity neuromodulation level (default: 0.5)"},
        {"novelty_level", "Novelty neuromodulation level (default: 0.5)"},
        {"prediction_error", "Prediction error level (default: 0.0)"},
        
        // Sensory
        {"vision_width", "Vision sensor width in pixels (default: 16)"},
        {"vision_height", "Vision sensor height in pixels (default: 16)"},
        {"audio_sample_rate", "Audio sample rate (default: 44100)"},
        {"audio_num_samples", "Number of audio samples (default: 1024)"},
        {"max_energy", "Maximum energy level (default: 100.0)"},
        {"energy_decay_rate", "Energy decay rate per timestep (default: 0.01)"},
        {"resource_energy_gain", "Energy gain from resources (default: 30.0)"},
        
        // World
        {"world_width", "World width in units (default: 20.0)"},
        {"world_height", "World height in units (default: 20.0)"},
        {"world_max_energy", "World maximum energy (default: 100.0)"},
        {"world_energy_decay_rate", "World energy decay rate (default: 0.01)"},
        {"world_resource_energy_gain", "World resource energy gain (default: 30.0)"},
        
        // Random seed
        {"random_seed", "Random seed for reproducible simulations (default: 42)"},
        
        // Checkpointing
        {"checkpoint_dir", "Directory for checkpoint files (default: ./checkpoints)"},
        {"max_checkpoints", "Maximum number of checkpoints to keep (default: 10000)"},
        {"max_history", "Maximum checkpoint history (default: 5)"},
        {"auto_save", "Enable automatic checkpoint saving (default: true)"}
    };
}

std::vector<std::string> ConfigValidator::validateAgainstSchema(const Config& config) {
    std::vector<std::string> errors;
    
    // Check for invalid keys
    const auto& validKeys = getAllConfigKeys();
    const auto& configKeys = config.getKeys();
    
    for (const auto& key : configKeys) {
        if (validKeys.find(key) == validKeys.end()) {
            errors.push_back("Unknown configuration key: " + key);
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::validateAgainstRanges(const Config& config) {
    std::vector<std::string> errors;
    
    // Validate ranges for each key
    const auto& configKeys = config.getKeys();
    
    for (const auto& key : configKeys) {
        std::vector<std::string> keyErrors = validateKey(key, config.get(key));
        errors.insert(errors.end(), keyErrors.begin(), keyErrors.end());
    }
    
    return errors;
}

std::vector<std::string> ConfigValidator::checkDependencies(const Config& config) {
    std::vector<std::string> errors;
    
    // Check for dependency violations
    
    // World keys should be prefixed for clarity
    if (config.has("width") && !config.has("world_width")) {
        errors.push_back("Key 'width' is deprecated, use 'world_width' instead");
    }
    
    if (config.has("height") && !config.has("world_height")) {
        errors.push_back("Key 'height' is deprecated, use 'world_height' instead");
    }
    
    if (config.has("maxEnergy") && !config.has("max_energy")) {
        errors.push_back("Key 'maxEnergy' is deprecated, use 'max_energy' instead");
    }
    
    return errors;
}

} // namespace nlm
