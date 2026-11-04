// Implementation of ConfigFactory.cpp
#include "ConfigFactory.hpp"
#include "Config/Config.hpp"
#include "ConfigDefaults.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default brain parameters
    config->set(ConfigDefaults::BRAIN_NEURON_COUNT, ConfigDefaults::BRAIN_DEFAULT_NEURON_COUNT, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_CONNECTION_PROBABILITY, ConfigDefaults::BRAIN_DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_V_THRESH, ConfigDefaults::BRAIN_DEFAULT_V_THRESH, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_V_REST, ConfigDefaults::BRAIN_DEFAULT_V_REST, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_V_RESET, ConfigDefaults::BRAIN_DEFAULT_V_RESET, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_TAU_MEM, ConfigDefaults::BRAIN_DEFAULT_TAU_MEM, ConfigSource::Default);
    config->set(ConfigDefaults::BRAIN_TAU_REF, ConfigDefaults::BRAIN_DEFAULT_TAU_REF, ConfigSource::Default);
    
    // Set default plasticity parameters
    config->set(ConfigDefaults::PLASTICITY_STDP_ENABLE, ConfigDefaults::PLASTICITY_DEFAULT_STDP_ENABLE, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_STDP_LEARNING_RATE, ConfigDefaults::PLASTICITY_DEFAULT_STDP_LEARNING_RATE, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_STDP_TAU_PLUS, ConfigDefaults::PLASTICITY_DEFAULT_STDP_TAU_PLUS, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_STDP_TAU_MINUS, ConfigDefaults::PLASTICITY_DEFAULT_STDP_TAU_MINUS, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_HEBBIAN_ENABLE, ConfigDefaults::PLASTICITY_DEFAULT_HEBBIAN_ENABLE, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_HEBBIAN_THRESHOLD, ConfigDefaults::PLASTICITY_DEFAULT_HEBBIAN_THRESHOLD, ConfigSource::Default);
    config->set(ConfigDefaults::PLASTICITY_STRUCTURAL_ENABLE, ConfigDefaults::PLASTICITY_DEFAULT_STRUCTURAL_ENABLE, ConfigSource::Default);
    
    // Set default neuromodulation parameters
    config->set(ConfigDefaults::NEUROMOD_DOPAMINE_SCALE, ConfigDefaults::NEUROMOD_DEFAULT_DOPAMINE_SCALE, ConfigSource::Default);
    config->set(ConfigDefaults::NEUROMOD_CURIOUSITY_ENABLE, ConfigDefaults::NEUROMOD_DEFAULT_CURIOUSITY_ENABLE, ConfigSource::Default);
    config->set(ConfigDefaults::NEUROMOD_NOVELTY_ENABLE, ConfigDefaults::NEUROMOD_DEFAULT_NOVELTY_ENABLE, ConfigSource::Default);
    
    // Set default memory parameters
    config->set(ConfigDefaults::MEMORY_WORKING_CAPACITY, ConfigDefaults::MEMORY_DEFAULT_WORKING_CAPACITY, ConfigSource::Default);
    config->set(ConfigDefaults::MEMORY_EPISODIC_MAX_EPISODES, ConfigDefaults::MEMORY_DEFAULT_MAX_EPISODES, ConfigSource::Default);
    config->set(ConfigDefaults::MEMORY_ASSOCIATIVE_MAX_ASSOCIATIONS, ConfigDefaults::MEMORY_DEFAULT_MAX_ASSOCIATIONS, ConfigSource::Default);
    
    // Set default prediction parameters
    config->set(ConfigDefaults::PREDICTION_ALPHA, ConfigDefaults::PREDICTION_DEFAULT_ALPHA, ConfigSource::Default);
    config->set(ConfigDefaults::PREDICTION_SURPRISE_THRESHOLD, ConfigDefaults::PREDICTION_DEFAULT_SURPRISE_THRESHOLD, ConfigSource::Default);
    
    // Set default cognition parameters
    config->set(ConfigDefaults::COGNITION_PLANNING_DEPTH, ConfigDefaults::COGNITION_DEFAULT_PLANNING_DEPTH, ConfigSource::Default);
    config->set(ConfigDefaults::COGNITION_ATTENTION_STRENGTH, ConfigDefaults::COGNITION_DEFAULT_ATTENTION_STRENGTH, ConfigSource::Default);
    config->set(ConfigDefaults::COGNITION_CONCEPT_FORMATION_THRESHOLD, ConfigDefaults::COGNITION_DEFAULT_CONCEPT_FORMATION_THRESHOLD, ConfigSource::Default);
    
    // Set default development parameters
    config->set(ConfigDefaults::DEVELOPMENT_INITIAL_AGE, ConfigDefaults::DEVELOPMENT_DEFAULT_INITIAL_AGE, ConfigSource::Default);
    config->set(ConfigDefaults::DEVELOPMENT_CRITICAL_PERIOD_END, ConfigDefaults::DEVELOPMENT_DEFAULT_CRITICAL_PERIOD_END, ConfigSource::Default);
    
    // Set default simulation parameters
    config->set(ConfigDefaults::SIMULATION_TIMESTEP, ConfigDefaults::SIMULATION_DEFAULT_TIMESTEP, ConfigSource::Default);
    config->set(ConfigDefaults::RANDOM_SEED, ConfigDefaults::SIMULATION_DEFAULT_RANDOM_SEED, ConfigSource::Default);
    
    // Set default performance parameters
    config->set(ConfigDefaults::PERFORMANCE_CHECKPOINT_DIR, ConfigDefaults::PERFORMANCE_DEFAULT_CHECKPOINT_DIR, ConfigSource::Default);
    config->set(ConfigDefaults::PERFORMANCE_CHECKPOINT_INTERVAL, ConfigDefaults::PERFORMANCE_DEFAULT_CHECKPOINT_INTERVAL, ConfigSource::Default);
    config->set(ConfigDefaults::PERFORMANCE_REPLAY_INTERVAL, ConfigDefaults::PERFORMANCE_DEFAULT_REPLAY_INTERVAL, ConfigSource::Default);
    config->set(ConfigDefaults::PERFORMANCE_CONSOLIDATION_INTERVAL, ConfigDefaults::PERFORMANCE_DEFAULT_CONSOLIDATION_INTERVAL, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> createConfig(
    size_t neuronCount,
    float connectionProbability,
    float simulationTimestep,
    int randomSeed
) {
    auto config = createDefaultConfig();
    
    // Override with provided parameters
    config->set(ConfigDefaults::BRAIN_NEURON_COUNT, static_cast<int64_t>(neuronCount), ConfigSource::Runtime);
    config->set(ConfigDefaults::BRAIN_CONNECTION_PROBABILITY, connectionProbability, ConfigSource::Runtime);
    config->set(ConfigDefaults::SIMULATION_TIMESTEP, simulationTimestep, ConfigSource::Runtime);
    config->set(ConfigDefaults::RANDOM_SEED, randomSeed, ConfigSource::Runtime);
    
    return config;
}

std::shared_ptr<Config> createConfigFromFile(const std::string& filepath) {
    auto config = createDefaultConfig();
    
    if (config->loadFromFile(filepath)) {
        return config;
    }
    
    // Return default config if file loading fails
    return createDefaultConfig();
}

std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv) {
    auto config = createDefaultConfig();
    
    if (config->loadFromArgs(argc, argv)) {
        return config;
    }
    
    return createDefaultConfig();
}

void applyPreset(std::shared_ptr<Config> config, const std::string& presetName) {
    std::transform(presetName.begin(), presetName.end(), presetName.begin(), ::tolower);
    
    if (presetName == "fast") {
        // Fast simulation with fewer neurons
        config->set(ConfigDefaults::BRAIN_NEURON_COUNT, 100, ConfigSource::Runtime);
        config->set(ConfigDefaults::SIMULATION_TIMESTEP, 0.01f, ConfigSource::Runtime);
    } else if (presetName == "detailed") {
        // Detailed simulation with more neurons
        config->set(ConfigDefaults::BRAIN_NEURON_COUNT, 5000, ConfigSource::Runtime);
        config->set(ConfigDefaults::SIMULATION_TIMESTEP, 0.0001f, ConfigSource::Runtime);
    } else if (presetName == "learning") {
        // Emphasize learning with stronger plasticity
        config->set(ConfigDefaults::PLASTICITY_STDP_LEARNING_RATE, 0.01f, ConfigSource::Runtime);
        config->set(ConfigDefaults::PLASTICITY_HEBBIAN_THRESHOLD, 0.05f, ConfigSource::Runtime);
    } else if (presetName == "stable") {
        // Stable simulation with less plasticity
        config->set(ConfigDefaults::PLASTICITY_STDP_LEARNING_RATE, 0.0001f, ConfigSource::Runtime);
        config->set(ConfigDefaults::PLASTICITY_HEBBIAN_THRESHOLD, 0.001f, ConfigSource::Runtime);
    }
}

std::vector<std::string> getAvailablePresets() {
    return {"fast", "detailed", "learning", "stable"};
}

bool validateConfig(const Config& config, std::vector<std::string>& errors) {
    errors.clear();
    
    auto neuronCount = config.get<int64_t>(ConfigDefaults::BRAIN_NEURON_COUNT);
    if (!neuronCount) {
        errors.push_back("Missing brain.neuron_count parameter");
    } else if (*neuronCount <= 0) {
        errors.push_back("brain.neuron_count must be positive");
    }
    
    auto connectionProb = config.get<float>(ConfigDefaults::BRAIN_CONNECTION_PROBABILITY);
    if (!connectionProb) {
        errors.push_back("Missing brain.connection_probability parameter");
    } else if (*connectionProb < 0.0f || *connectionProb > 1.0f) {
        errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
    }
    
    auto timestep = config.get<float>(ConfigDefaults::SIMULATION_TIMESTEP);
    if (!timestep) {
        errors.push_back("Missing simulation.timestep parameter");
    } else if (*timestep <= 0.0f) {
        errors.push_back("simulation.timestep must be positive");
    }
    
    auto seed = config.get<int64_t>(ConfigDefaults::RANDOM_SEED);
    if (!seed) {
        errors.push_back("Missing random.seed parameter");
    }
    
    return errors.empty();
}

std::string configSummary(const Config& config) {
    return config.summary();
}

} // namespace nlm