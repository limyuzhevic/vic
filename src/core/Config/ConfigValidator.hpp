// Configuration validation utilities for NLM
#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Default configuration schema for NLM
struct NLMDefaultConfig {
    // Neural configuration
    struct Neural {
        int64_t neuron_count = 1000;
        int64_t region_count = 1;
        float connection_probability = 0.1f;
        float mean_excitatory_weight = 0.2f;
        float mean_inhibitory_weight = -0.1f;
    } neural;

    // STDP parameters
    struct STDP {
        float ltp_weight = 0.01f;
        float ltd_weight = 0.012f;
        float tau = 20.0f;
    } stdp;

    // Structural plasticity
    struct StructuralPlasticity {
        float synaptogenesis_rate = 0.0001f;
        float pruning_rate = 0.00001f;
        float max_synapses_per_neuron = 25.0f;
    } structuralPlasticity;

    // Development parameters
    struct Development {
        double timestep = 0.001;
        float plasticity_modifier = 1.0f;
        float developmental_stage_multiplier = 1.0f;
    } development;

    // Memory parameters
    struct Memory {
        size_t working_memory_capacity = 100;
        size_t max_episodes = 1000;
        float consolidation_threshold = 0.3f;
        size_t replay_interval = 100;
        size_t consolidation_interval = 1000;
    } memory;

    // Neuromodulation
    struct Neuromodulation {
        float dopamine_level = 0.5f;
        float curiosity_level = 0.5f;
        float novelty_level = 0.5f;
        float prediction_error = 0.0f;
    } neuromodulation;

    // Sensory parameters
    struct Sensory {
        size_t vision_width = 16;
        size_t vision_height = 16;
        size_t audio_sample_rate = 44100;
        size_t audio_num_samples = 1024;
        float max_energy = 100.0f;
        float energy_decay_rate = 0.01f;
        float resource_energy_gain = 30.0f;
    } sensory;

    // World parameters
    struct World {
        float width = 20.0f;
        float height = 20.0f;
        float max_energy = 100.0f;
        float energy_decay_rate = 0.01f;
        float resource_energy_gain = 30.0f;
    } world;

    // Random seed
    int64_t random_seed = 42;

    // Checkpointing
    struct Checkpoint {
        std::string directory = "./checkpoints";
        size_t max_checkpoints = 10000;
        size_t max_history = 5;
        bool auto_save = true;
    } checkpoint;

    // Default configuration values
    static const NLMDefaultConfig& get() {
        static NLMDefaultConfig config;
        return config;
    }
};

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Validate configuration against NLM schema
    static std::vector<std::string> validate(const Config& config);
    
    // Apply default values for missing keys
    static void applyDefaults(Config& config);
    
    // Get configuration documentation
    static std::string getConfigDocumentation(const std::string& key = "");
    
    // Validate specific configuration keys
    static std::vector<std::string> validateKey(const std::string& key, const ConfigValue& value);
    
    // Get all known configuration keys with descriptions
    static std::map<std::string, std::string> getAllConfigKeys();

private:
    static std::vector<std::string> validateAgainstSchema(const Config& config);
    static std::vector<std::string> validateAgainstRanges(const Config& config);
    static std::vector<std::string> validateDependencies(const Config& config);
    
    // Type-specific validation
    static std::vector<std::string> validateInt(const std::string& key, int value);
    static std::vector<std::string> validateFloat(const std::string& key, float value);
    static std::vector<std::string> validateDouble(const std::string& key, double value);
    static std::vector<std::string> validateBool(const std::string& key, bool value);
    static std::vector<std::string> validateString(const std::string& key, const std::string& value);
    static std::vector<std::string> validateIntVector(const std::string& key, const std::vector<int>& value);
    static std::vector<std::string> validateFloatVector(const std::string& key, const std::vector<float>& value);
    static std::vector<std::string> validateStringVector(const std::string& key, const std::vector<std::string>& value);
    
    // Range validation
    static bool isInRange(int value, int min, int max);
    static bool isInRange(float value, float min, float max);
    static bool isInRange(double value, double min, double max);
    static bool isInRange(int64_t value, int64_t min, int64_t max);
    static bool isInRange(size_t value, size_t min, size_t max);
    
    // Dependency validation
    static std::vector<std::string> checkDependencies(const Config& config);
};

} // namespace nlm
