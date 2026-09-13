// NLM Configuration Constants
// Unified configuration management system for NLM

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <any>

namespace nlm {

// Configuration keys for all NLM components
namespace ConfigKeys {
    // Brain configuration
    const std::string BRAIN = "brain";
    const std::string NEURON_COUNT = "brain.neuron_count";
    const std::string REGION_COUNT = "brain.region_count";
    const std::string CONNECTION_PROBABILITY = "brain.connection_probability";
    const std::string INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
    const std::string INITIAL_WEIGHT_STD = "brain.initial_weight_std";
    const std::string V_THRESH = "brain.v_thresh";
    const std::string V_REST = "brain.v_rest";
    const std::string V_RESET = "brain.v_reset";
    const std::string TAU_MEM = "brain.tau_mem";
    const std::string TAU_REF = "brain.tau_ref";
    const std::string SIMULATION_TIMESTEP = "brain.simulation_timestep";
    const std::string RANDOM_SEED = "brain.random_seed";

    // Plasticity configuration
    const std::string PLASTICITY = "plasticity";
    const std::string STDP_ENABLE = "plasticity.stdp.enable";
    const std::string STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    const std::string STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
    const std::string STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
    const std::string HEBBIAN_ENABLE = "plasticity.hebbian.enable";
    const std::string STRUCTURAL_ENABLE = "plasticity.structural.enable";
    const std::string SYNAPTOGENESIS_RATE = "plasticity.structural.synaptogenesis_rate";
    const std::string PRUNING_RATE = "plasticity.structural.pruning_rate";

    // Neuromodulation configuration
    const std::string NEUROMOD = "neuromod";
    const std::string DOPAMINE_SCALE = "neuromod.dopamine.scale";
    const std::string CURiosity_ENABLE = "neuromod.curiosity.enable";
    const std::string NOVELTY_ENABLE = "neuromod.novelty.enable";

    // Development configuration
    const std::string DEVELOPMENT = "development";
    const std::string CRITICAL_PERIOD_START = "development.critical_period_start";
    const std::string MATURATION_RATE = "development.maturation_rate";
    const std::string AGING_RATE = "development.aging_rate";

    // Memory configuration
    const std::string MEMORY = "memory";
    const std::string WORKING_MEMORY_CAPACITY = "memory.working_memory_capacity";
    const std::string EPISODIC_MEMORY_MAX_SIZE = "memory.episodic_memory_max_size";

    // Sensory configuration
    const std::string SENSORY = "sensory";
    const std::string VISION_WIDTH = "sensory.vision_width";
    const std::string VISION_HEIGHT = "sensory.vision_height";
    const std::string AUDIO_SAMPLE_RATE = "sensory.audio_sample_rate";

    // World configuration
    const std::string WORLD = "world";
    const std::string ENVIRONMENT_WIDTH = "world.environment_width";
    const std::string ENVIRONMENT_HEIGHT = "world.environment_height";
    const std::string MAX_ENERGY = "world.max_energy";
    const std::string ENERGY_DECAY_RATE = "world.energy_decay_rate";

    // Agent configuration
    const std::string AGENT = "agent";
    const std::string REWARD_MODULATION = "agent.reward_modulation";
    const std::string STRUCTURAL_PLASTICITY = "agent.structural_plasticity";
    const std::string DEVELOPMENT = "agent.development";
    const std::string CURIOSITY = "agent.curiosity";

    // Experiment configuration
    const std::string EXPERIMENT = "experiment";
    const std::string NUM_TRIALS = "experiment.num_trials";
    const std::string MAX_STEPS = "experiment.max_steps";

    // Visualization configuration
    const std::string VISUALIZATION = "visualization";
    const std::string ENABLE_VISUALIZATION = "visualization.enable";
    const std::string VISUALIZATION_REFRESH_RATE = "visualization.refresh_rate";
}

// Default configuration values
namespace Defaults {
    // Brain defaults
    constexpr size_t BRAIN_NEURON_COUNT = 1000;
    constexpr size_t BRAIN_REGION_COUNT = 2;
    constexpr float BRAIN_CONNECTION_PROBABILITY = 0.05f;
    constexpr float BRAIN_INITIAL_WEIGHT_MEAN = 0.5f;
    constexpr float BRAIN_INITIAL_WEIGHT_STD = 0.1f;
    constexpr float BRAIN_V_THRESH = -50.0f;
    constexpr float BRAIN_V_REST = -70.0f;
    constexpr float BRAIN_V_RESET = -75.0f;
    constexpr float BRAIN_TAU_MEM = 20.0f;
    constexpr float BRAIN_TAU_REF = 2.0f;
    constexpr float BRAIN_SIMULATION_TIMESTEP = 0.001f;
    constexpr uint64_t BRAIN_RANDOM_SEED = 42;

    // Plasticity defaults
    constexpr bool PLASTICITY_STDP_ENABLE = true;
    constexpr float PLASTICITY_STDP_LEARNING_RATE = 0.001f;
    constexpr float PLASTICITY_STDP_TAU_PLUS = 20.0f;
    constexpr float PLASTICITY_STDP_TAU_MINUS = 20.0f;
    constexpr bool PLASTICITY_HEBBIAN_ENABLE = true;
    constexpr bool PLASTICITY_STRUCTURAL_ENABLE = true;
    constexpr float PLASTICITY_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float PLASTICITY_PRUNING_RATE = 0.00001f;

    // Neuromodulation defaults
    constexpr float NEUROMOD_DOPAMINE_SCALE = 1.0f;
    constexpr bool NEUROMOD_CURIOUSITY_ENABLE = true;
    constexpr bool NEUROMOD_NOVELTY_ENABLE = true;

    // Development defaults
    constexpr float DEVELOPMENT_CRITICAL_PERIOD_START = 100.0f;
    constexpr float DEVELOPMENT_MATURATION_RATE = 0.01f;
    constexpr float DEVELOPMENT_AGING_RATE = 0.005f;

    // Memory defaults
    constexpr size_t MEMORY_WORKING_MEMORY_CAPACITY = 100;
    constexpr size_t MEMORY_EPISODIC_MEMORY_MAX_SIZE = 1000;

    // Sensory defaults
    constexpr size_t SENSORY_VISION_WIDTH = 16;
    constexpr size_t SENSORY_VISION_HEIGHT = 16;
    constexpr size_t SENSORY_AUDIO_SAMPLE_RATE = 44100;

    // World defaults
    constexpr float WORLD_ENVIRONMENT_WIDTH = 20.0f;
    constexpr float WORLD_ENVIRONMENT_HEIGHT = 20.0f;
    constexpr float WORLD_MAX_ENERGY = 100.0f;
    constexpr float WORLD_ENERGY_DECAY_RATE = 0.01f;

    // Agent defaults
    constexpr bool AGENT_REWARD_MODULATION = true;
    constexpr bool AGENT_STRUCTURAL_PLASTICITY = true;
    constexpr bool AGENT_DEVELOPMENT = true;
    constexpr bool AGENT_CURIOSITY = true;

    // Experiment defaults
    constexpr size_t EXPERIMENT_NUM_TRIALS = 50;
    constexpr size_t EXPERIMENT_MAX_STEPS = 1000;

    // Visualization defaults
    constexpr bool VISUALIZATION_ENABLE = false;
    constexpr float VISUALIZATION_REFRESH_RATE = 30.0f;
}

// Configuration validation
struct ConfigValidation {
    static bool validateNeuronCount(size_t count) {
        return count > 0 && count <= 100000;
    }

    static bool validateRegionCount(size_t count) {
        return count > 0 && count <= 100;
    }

    static bool validateConnectionProbability(float prob) {
        return prob >= 0.0f && prob <= 1.0f;
    }

    static bool validateSynapseDensity(float density) {
        return density >= 0.0f && density <= 1.0f;
    }

    static bool validateTimestep(float timestep) {
        return timestep > 0.0f && timestep <= 1.0f;
    }

    static bool validateEnergy(float energy) {
        return energy >= 0.0f && energy <= 1000.0f;
    }

    static bool validateTemperature(float temp) {
        return temp >= -50.0f && temp <= 100.0f;
    }
};

// Helper functions for configuration
class ConfigHelper {
public:
    // Get configuration value with type safety
    template<typename T>
    static T get(const std::string& key, const std::unordered_map<std::string, std::any>& config,
                 const T& defaultValue) {
        auto it = config.find(key);
        if (it == config.end()) {
            return defaultValue;
        }

        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return defaultValue;
        }
    }

    // Set configuration value with type safety
    template<typename T>
    static void set(std::unordered_map<std::string, std::any>& config, const std::string& key, const T& value) {
        config[key] = value;
    }

    // Validate configuration value
    template<typename T>
    static bool validate(const std::string& key, const T& value) {
        if (key == ConfigKeys::NEURON_COUNT) {
            return ConfigValidation::validateNeuronCount(value);
        } else if (key == ConfigKeys::REGION_COUNT) {
            return ConfigValidation::validateRegionCount(value);
        } else if (key == ConfigKeys::CONNECTION_PROBABILITY) {
            return ConfigValidation::validateConnectionProbability(value);
        } else if (key == ConfigKeys::SIMULATION_TIMESTEP) {
            return ConfigValidation::validateTimestep(value);
        } else if (key == ConfigKeys::MAX_ENERGY) {
            return ConfigValidation::validateEnergy(value);
        } else if (key == ConfigKeys::V_THRESH || key == ConfigKeys::V_REST || key == ConfigKeys::V_RESET) {
            return ConfigValidation::validateTemperature(value);
        }

        return true; // Default to valid for unknown keys
    }

    // Print configuration summary
    static void printConfig(const std::unordered_map<std::string, std::any>& config) {
        std::cout << "=== NLM Configuration Summary ===" << std::endl;
        for (const auto& pair : config) {
            std::cout << pair.first << " = ";
            printValue(pair.second);
            std::cout << std::endl;
        }
    }

private:
    static void printValue(const std::any& value) {
        if (std::any_cast<size_t>(&value) != nullptr) {
            std::cout << std::any_cast<size_t>(value);
        } else if (std::any_cast<float>(&value) != nullptr) {
            std::cout << std::any_cast<float>(value);
        } else if (std::any_cast<bool>(&value) != nullptr) {
            std::cout << (std::any_cast<bool>(value) ? "true" : "false");
        } else if (std::any_cast<std::string>(&value) != nullptr) {
            std::cout << "\"" << std::any_cast<std::string>(value) << "\"";
        } else {
            std::cout << "[unknown]";
        }
    }
};

// Configuration presets for common scenarios
class ConfigPresets {
public:
    // Get configuration for a specific phase
    static std::unordered_map<std::string, std::any> getPhaseConfig(int phase) {
        std::unordered_map<std::string, std::any> config;

        switch (phase) {
            case 2:
                // Phase 2: Basic neural computation
                setDefaults(config);
                config[ConfigKeys::NEURON_COUNT] = 500;
                config[ConfigKeys::STDP_ENABLE] = true;
                config[ConfigKeys::HEBBIAN_ENABLE] = true;
                config[ConfigKeys::STRUCTURAL_ENABLE] = true;
                break;

            case 3:
                // Phase 3: World interaction
                setDefaults(config);
                config[ConfigKeys::NEURON_COUNT] = 500;
                config[ConfigKeys::WORLD_ENVIRONMENT_WIDTH] = 20.0f;
                config[ConfigKeys::WORLD_ENVIRONMENT_HEIGHT] = 20.0f;
                config[ConfigKeys::SENSORY_VISION_WIDTH] = 16;
                config[ConfigKeys::SENSORY_VISION_HEIGHT] = 16;
                config[ConfigKeys::AGENT_REWARD_MODULATION] = true;
                config[ConfigKeys::AGENT_CURIOSITY] = true;
                break;

            case 4:
                // Phase 4: Cognitive capabilities
                setDefaults(config);
                config[ConfigKeys::NEURON_COUNT] = 1000;
                config[ConfigKeys::MEMORY_WORKING_MEMORY_CAPACITY] = 100;
                config[ConfigKeys::MEMORY_EPISODIC_MEMORY_MAX_SIZE] = 500;
                config[ConfigKeys::EXPERIMENT_NUM_TRIALS] = 50;
                break;

            case 6:
                // Phase 6: Integration
                setDefaults(config);
                config[ConfigKeys::NEURON_COUNT] = 1000;
                config[ConfigKeys::WORLD_ENVIRONMENT_WIDTH] = 30.0f;
                config[ConfigKeys::WORLD_ENVIRONMENT_HEIGHT] = 30.0f;
                config[ConfigKeys::EXPERIMENT_MAX_STEPS] = 5000;
                config[ConfigKeys::VISUALIZATION_ENABLE] = true;
                break;

            default:
                setDefaults(config);
                break;
        }

        return config;
    }

private:
    static void setDefaults(std::unordered_map<std::string, std::any>& config) {
        // Brain defaults
        config[ConfigKeys::NEURON_COUNT] = Defaults::BRAIN_NEURON_COUNT;
        config[ConfigKeys::REGION_COUNT] = Defaults::BRAIN_REGION_COUNT;
        config[ConfigKeys::CONNECTION_PROBABILITY] = Defaults::BRAIN_CONNECTION_PROBABILITY;
        config[ConfigKeys::INITIAL_WEIGHT_MEAN] = Defaults::BRAIN_INITIAL_WEIGHT_MEAN;
        config[ConfigKeys::INITIAL_WEIGHT_STD] = Defaults::BRAIN_INITIAL_WEIGHT_STD;
        config[ConfigKeys::V_THRESH] = Defaults::BRAIN_V_THRESH;
        config[ConfigKeys::V_REST] = Defaults::BRAIN_V_REST;
        config[ConfigKeys::V_RESET] = Defaults::BRAIN_V_RESET;
        config[ConfigKeys::TAU_MEM] = Defaults::BRAIN_TAU_MEM;
        config[ConfigKeys::TAU_REF] = Defaults::BRAIN_TAU_REF;
        config[ConfigKeys::SIMULATION_TIMESTEP] = Defaults::BRAIN_SIMULATION_TIMESTEP;
        config[ConfigKeys::RANDOM_SEED] = Defaults::BRAIN_RANDOM_SEED;

        // Plasticity defaults
        config[ConfigKeys::STDP_ENABLE] = Defaults::PLASTICITY_STDP_ENABLE;
        config[ConfigKeys::STDP_LEARNING_RATE] = Defaults::PLASTICITY_STDP_LEARNING_RATE;
        config[ConfigKeys::STDP_TAU_PLUS] = Defaults::PLASTICITY_STDP_TAU_PLUS;
        config[ConfigKeys::STDP_TAU_MINUS] = Defaults::PLASTICITY_STDP_TAU_MINUS;
        config[ConfigKeys::HEBBIAN_ENABLE] = Defaults::PLASTICITY_HEBBIAN_ENABLE;
        config[ConfigKeys::STRUCTURAL_ENABLE] = Defaults::PLASTICITY_STRUCTURAL_ENABLE;
        config[ConfigKeys::SYNAPTOGENESIS_RATE] = Defaults::PLASTICITY_SYNAPTOGENESIS_RATE;
        config[ConfigKeys::PRUNING_RATE] = Defaults::PLASTICITY_PRUNING_RATE;

        // Neuromodulation defaults
        config[ConfigKeys::DOPAMINE_SCALE] = Defaults::NEUROMOD_DOPAMINE_SCALE;
        config[ConfigKeys::CURIOUSITY_ENABLE] = Defaults::NEUROMOD_CURIOUSITY_ENABLE;
        config[ConfigKeys::NOVELTY_ENABLE] = Defaults::NEUROMOD_NOVELTY_ENABLE;

        // Development defaults
        config[ConfigKeys::CRITICAL_PERIOD_START] = Defaults::DEVELOPMENT_CRITICAL_PERIOD_START;
        config[ConfigKeys::MATURATION_RATE] = Defaults::DEVELOPMENT_MATURATION_RATE;
        config[ConfigKeys::AGING_RATE] = Defaults::DEVELOPMENT_AGING_RATE;

        // Memory defaults
        config[ConfigKeys::WORKING_MEMORY_CAPACITY] = Defaults::MEMORY_WORKING_MEMORY_CAPACITY;
        config[ConfigKeys::EPISODIC_MEMORY_MAX_SIZE] = Defaults::MEMORY_EPISODIC_MEMORY_MAX_SIZE;

        // Sensory defaults
        config[ConfigKeys::VISION_WIDTH] = Defaults::SENSORY_VISION_WIDTH;
        config[ConfigKeys::VISION_HEIGHT] = Defaults::SENSORY_VISION_HEIGHT;
        config[ConfigKeys::AUDIO_SAMPLE_RATE] = Defaults::SENSORY_AUDIO_SAMPLE_RATE;

        // World defaults
        config[ConfigKeys::ENVIRONMENT_WIDTH] = Defaults::WORLD_ENVIRONMENT_WIDTH;
        config[ConfigKeys::ENVIRONMENT_HEIGHT] = Defaults::WORLD_ENVIRONMENT_HEIGHT;
        config[ConfigKeys::MAX_ENERGY] = Defaults::WORLD_MAX_ENERGY;
        config[ConfigKeys::ENERGY_DECAY_RATE] = Defaults::WORLD_ENERGY_DECAY_RATE;

        // Agent defaults
        config[ConfigKeys::REWARD_MODULATION] = Defaults::AGENT_REWARD_MODULATION;
        config[ConfigKeys::STRUCTURAL_PLASTICITY] = Defaults::AGENT_STRUCTURAL_PLASTICITY;
        config[ConfigKeys::DEVELOPMENT] = Defaults::AGENT_DEVELOPMENT;
        config[ConfigKeys::CURIOSITY] = Defaults::AGENT_CURIOSITY;

        // Experiment defaults
        config[ConfigKeys::NUM_TRIALS] = Defaults::EXPERIMENT_NUM_TRIALS;
        config[ConfigKeys::MAX_STEPS] = Defaults::EXPERIMENT_MAX_STEPS;

        // Visualization defaults
        config[ConfigKeys::ENABLE_VISUALIZATION] = Defaults::VISUALIZATION_ENABLE;
        config[ConfigKeys::VISUALIZATION_REFRESH_RATE] = Defaults::VISUALIZATION_REFRESH_RATE;
    }
};

} // namespace nlm