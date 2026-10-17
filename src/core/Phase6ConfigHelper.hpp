// Phase 6 Configuration Helper
// Utilities for creating and managing Phase 6 integration configurations

#include "core/Config/Config.hpp"
#include <string>
#include <map>

namespace nlm {
namespace Phase6Config {

// Default Phase 6 configuration settings
struct DefaultPhase6Settings {
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 2;
    static constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    static constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    
    // Memory system settings
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    static constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;
    
    // Neuromodulation settings
    static constexpr float DEFAULT_DOPAMINE_BASELINE = 0.0f;
    static constexpr float DEFAULT_NOVITY_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_CURIOUSITY_LEVEL = 0.5f;
    
    // Plasticity settings
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    static constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    static constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Development settings
    static constexpr float DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001f;
    static constexpr float DEFAULT_DEVELOPMENT_PRUNING_RATE = 0.0001f;
    
    // Integration settings
    static constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
    static constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
    static constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;
};

// Create a standard Phase 6 configuration
std::shared_ptr<Config> createPhase6Config(const std::string& name = "Phase6_Default") {
    auto config = std::make_shared<Config>();
    
    // Set name
    config->set("config_name", name, ConfigSource::Runtime);
    
    // Core simulation settings
    config->set("random_seed", DefaultPhase6Settings::DEFAULT_RANDOM_SEED, ConfigSource::Default);
    config->set("simulation_timestep", DefaultPhase6Settings::DEFAULT_SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", DefaultPhase6Settings::DEFAULT_NEURON_COUNT, ConfigSource::Default);
    config->set("region_count", DefaultPhase6Settings::DEFAULT_REGION_COUNT, ConfigSource::Default);
    config->set("connection_probability", DefaultPhase6Settings::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    
    // Memory system settings
    config->set("working_memory_capacity", DefaultPhase6Settings::DEFAULT_WORKING_MEMORY_CAPACITY, ConfigSource::Default);
    config->set("episodic_memory_max_episodes", DefaultPhase6Settings::DEFAULT_EPISODIC_MEMORY_MAX_EPISODES, ConfigSource::Default);
    
    // Neuromodulation settings
    config->set("dopamine_baseline", DefaultPhase6Settings::DEFAULT_DOPAMINE_BASELINE, ConfigSource::Default);
    config->set("novelty_sensitivity", DefaultPhase6Settings::DEFAULT_NOVITY_SENSITIVITY, ConfigSource::Default);
    config->set("curiosity_level", DefaultPhase6Settings::DEFAULT_CURIOUSITY_LEVEL, ConfigSource::Default);
    
    // Plasticity settings
    config->set("stdp_ltp_weight", DefaultPhase6Settings::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", DefaultPhase6Settings::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_time_constant", DefaultPhase6Settings::DEFAULT_STDP_TIME_CONSTANT, ConfigSource::Default);
    config->set("synaptogenesis_rate", DefaultPhase6Settings::DEFAULT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", DefaultPhase6Settings::DEFAULT_PRUNING_RATE, ConfigSource::Default);
    
    // Development settings
    config->set("development_synaptogenesis_rate", DefaultPhase6Settings::DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE, ConfigSource::Default);
    config->set("development_pruning_rate", DefaultPhase6Settings::DEFAULT_DEVELOPMENT_PRUNING_RATE, ConfigSource::Default);
    
    // Integration settings
    config->set("replay_interval", DefaultPhase6Settings::DEFAULT_REPLAY_INTERVAL, ConfigSource::Default);
    config->set("consolidation_interval", DefaultPhase6Settings::DEFAULT_CONSOLIDATION_INTERVAL, ConfigSource::Default);
    config->set("reward_discount_factor", DefaultPhase6Settings::DEFAULT_REWARD_DISCOUNT_FACTOR, ConfigSource::Default);
    
    // Enhanced debugging for development
    config->set("debug_level", 2, ConfigSource::Default);  // 0=Debug, 1=Info, 2=Warning, 3=Error
    config->set("log_to_file", true, ConfigSource::Default);
    config->set("log_filename", "nlm_phase6_integration.log", ConfigSource::Default);
    
    return config;
}

// Create a compact research configuration for smaller experiments
std::shared_ptr<Config> createResearchConfig(size_t neuronCount = 200) {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 123, ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001f, ConfigSource::Runtime);
    config->set("neuron_count", neuronCount, ConfigSource::Runtime);
    config->set("region_count", 1, ConfigSource::Runtime);
    config->set("connection_probability", 0.05f, ConfigSource::Runtime);
    
    // Focused memory systems
    config->set("working_memory_capacity", 50, ConfigSource::Runtime);
    config->set("episodic_memory_max_episodes", 500, ConfigSource::Runtime);
    
    // Learning-focused neuromodulation
    config->set("dopamine_baseline", 0.05f, ConfigSource::Runtime);
    config->set("curiosity_level", 0.7f, ConfigSource::Runtime);
    
    // Aggressive plasticity for learning
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.018f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.0005f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.00005f, ConfigSource::Runtime);
    
    // Faster replay for learning
    config->set("replay_interval", 50, ConfigSource::Runtime);
    config->set("consolidation_interval", 500, ConfigSource::Runtime);
    
    return config;
}

// Create a stable adult brain configuration
std::shared_ptr<Config> createAdultBrainConfig() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 42, ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001f, ConfigSource::Runtime);
    config->set("neuron_count", 2000, ConfigSource::Runtime);
    config->set("region_count", 3, ConfigSource::Runtime);
    config->set("connection_probability", 0.08f, ConfigSource::Runtime);
    
    // Mature memory systems
    config->set("working_memory_capacity", 200, ConfigSource::Runtime);
    config->set("episodic_memory_max_episodes", 5000, ConfigSource::Runtime);
    
    // Balanced neuromodulation
    config->set("dopamine_baseline", 0.1f, ConfigSource::Runtime);
    config->set("curiosity_level", 0.3f, ConfigSource::Runtime);
    
    // Stable plasticity
    config->set("stdp_ltp_weight", 0.005f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.006f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.000001f, ConfigSource::Runtime);
    
    // Conservative replay
    config->set("replay_interval", 500, ConfigSource::Runtime);
    config->set("consolidation_interval", 5000, ConfigSource::Runtime);
    
    return config;
}

// Create an experimental development configuration
std::shared_ptr<Config> createDevelopmentConfig() {
    auto config = std::make_shared<Config>();
    
    config->set("random_seed", 456, ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001f, ConfigSource::Runtime);
    config->set("neuron_count", 500, ConfigSource::Runtime);
    config->set("region_count", 2, ConfigSource::Runtime);
    config->set("connection_probability", 0.12f, ConfigSource::Runtime);
    
    // Large memory capacity for development
    config->set("working_memory_capacity", 300, ConfigSource::Runtime);
    config->set("episodic_memory_max_episodes", 10000, ConfigSource::Runtime);
    
    // High neuromodulation for exploration
    config->set("dopamine_baseline", 0.15f, ConfigSource::Runtime);
    config->set("curiosity_level", 0.9f, ConfigSource::Runtime);
    
    // Dynamic plasticity
    config->set("stdp_ltp_weight", 0.03f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.028f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.0002f, ConfigSource::Runtime);
    
    // Frequent replay for development
    config->set("replay_interval", 20, ConfigSource::Runtime);
    config->set("consolidation_interval", 200, ConfigSource::Runtime);
    
    return config;
}

// Print configuration summary
void printConfigSummary(const Config& config) {
    std::cout << "=== Phase 6 Configuration Summary ===" << std::endl;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::cout << key << " = ";
        
        auto value = config.get<std::string>(key);
        if (value) {
            std::cout << *value << std::endl;
        } else {
            auto intVal = config.get<int>(key);
            if (intVal) {
                std::cout << *intVal << std::endl;
            } else {
                auto doubleVal = config.get<double>(key);
                if (doubleVal) {
                    std::cout << *doubleVal << std::endl;
                } else {
                    auto floatVal = config.get<float>(key);
                    if (floatVal) {
                        std::cout << *floatVal << std::endl;
                    } else {
                        auto boolVal = config.get<bool>(key);
                        if (boolVal) {
                            std::cout << (*boolVal ? "true" : "false") << std::endl;
                        } else {
                            std::cout << "[unknown type]" << std::endl;
                        }
                    }
                }
            }
        }
    }
}

} // namespace Phase6Config

} // namespace nlm
