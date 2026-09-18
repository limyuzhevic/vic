#include "ConfigBuilder.hpp"
#include "Config.hpp"
#include <stdexcept>

namespace nlm {

struct ConfigBuilder::Impl {
    std::shared_ptr<Config> config;
    bool developmentEnabled;
    bool neuromodulationEnabled;
    bool memoryEnabled;
    bool predictionEnabled;
    bool curiosityEnabled;
    bool noveltyDetectionEnabled;
    bool rewardPredictionEnabled;
    std::string simulationMode;
    bool experimentalFeatures;
    bool debugMode;
    
    Impl() 
        : config(std::make_shared<Config>())
        , developmentEnabled(false)
        , neuromodulationEnabled(false)
        , memoryEnabled(false)
        , predictionEnabled(false)
        , curiosityEnabled(false)
        , noveltyDetectionEnabled(false)
        , rewardPredictionEnabled(false)
        , simulationMode("balanced")
        , experimentalFeatures(false)
        , debugMode(false) {}
};

ConfigBuilder::ConfigBuilder() : pImpl(new Impl()) {}

ConfigBuilder::~ConfigBuilder() = default;

ConfigBuilder& ConfigBuilder::withRandomSeed(int seed) {
    if (seed < 0) {
        throw std::invalid_argument("Random seed must be non-negative");
    }
    pImpl->config->set("random_seed", seed, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withNeuronCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Neuron count must be positive");
    }
    pImpl->config->set("neuron_count", static_cast<int>(count), ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withRegionCount(size_t count) {
    if (count == 0) {
        throw std::invalid_argument("Region count must be positive");
    }
    pImpl->config->set("region_count", static_cast<int>(count), ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withConnectionProbability(double prob) {
    if (prob < 0.0 || prob > 1.0) {
        throw std::invalid_argument("Connection probability must be between 0.0 and 1.0");
    }
    pImpl->config->set("connection_probability", prob, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDevelopmentEnabled(bool enabled) {
    pImpl->developmentEnabled = enabled;
    pImpl->config->set("development_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withNeuromodulationEnabled(bool enabled) {
    pImpl->neuromodulationEnabled = enabled;
    pImpl->config->set("neuromodulation_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withMemoryEnabled(bool enabled) {
    pImpl->memoryEnabled = enabled;
    pImpl->config->set("memory_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withPredictionEnabled(bool enabled) {
    pImpl->predictionEnabled = enabled;
    pImpl->config->set("prediction_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withCuriosityEnabled(bool enabled) {
    pImpl->curiosityEnabled = enabled;
    pImpl->config->set("curiosity_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withNoveltyDetectionEnabled(bool enabled) {
    pImpl->noveltyDetectionEnabled = enabled;
    pImpl->config->set("novelty_detection_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withRewardPredictionEnabled(bool enabled) {
    pImpl->rewardPredictionEnabled = enabled;
    pImpl->config->set("reward_prediction_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withSimulationTimestep(double timestep) {
    if (timestep <= 0.0) {
        throw std::invalid_argument("Simulation timestep must be positive");
    }
    pImpl->config->set("simulation_timestep", timestep, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withMaxSteps(size_t maxSteps) {
    if (maxSteps == 0) {
        throw std::invalid_argument("Max steps must be positive");
    }
    pImpl->config->set("max_simulation_steps", static_cast<int>(maxSteps), ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDevelopmentRate(double rate) {
    if (rate < 0.0) {
        throw std::invalid_argument("Development rate must be non-negative");
    }
    pImpl->config->set("development_synaptogenesis_rate", rate, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withPlasticityLearningRate(double rate) {
    if (rate < 0.0) {
        throw std::invalid_argument("Plasticity learning rate must be non-negative");
    }
    pImpl->config->set("plasticity_learning_rate", rate, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDopamineBaseline(float baseline) {
    if (baseline < 0.0 || baseline > 1.0) {
        throw std::invalid_argument("Dopamine baseline must be between 0.0 and 1.0");
    }
    pImpl->config->set("dopamine_baseline", baseline, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::addParameter(const std::string& key, const std::string& value) {
    if (key.empty()) {
        throw std::invalid_argument("Parameter key cannot be empty");
    }
    pImpl->config->set(key, value, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::addParameter(const std::string& key, double value) {
    if (key.empty()) {
        throw std::invalid_argument("Parameter key cannot be empty");
    }
    pImpl->config->set(key, value, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::addParameter(const std::string& key, int value) {
    if (key.empty()) {
        throw std::invalid_argument("Parameter key cannot be empty");
    }
    pImpl->config->set(key, value, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::addParameter(const std::string& key, bool value) {
    if (key.empty()) {
        throw std::invalid_argument("Parameter key cannot be empty");
    }
    pImpl->config->set(key, value, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withExperimentalFeatures(bool enabled) {
    pImpl->experimentalFeatures = enabled;
    pImpl->config->set("experimental_enabled", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withDebugMode(bool enabled) {
    pImpl->debugMode = enabled;
    pImpl->config->set("debug_mode", enabled, ConfigSource::Runtime);
    return *this;
}

ConfigBuilder& ConfigBuilder::withSimulationMode(const std::string& mode) {
    if (mode != "fast" && mode != "accurate" && mode != "balanced") {
        throw std::invalid_argument("Simulation mode must be 'fast', 'accurate', or 'balanced'");
    }
    pImpl->simulationMode = mode;
    pImpl->config->set("simulation_mode", mode, ConfigSource::Runtime);
    return *this;
}

std::shared_ptr<Config> ConfigBuilder::build() {
    // Apply builder flags to config
    if (pImpl->developmentEnabled) {
        pImpl->config->set("development_enabled", true, ConfigSource::Runtime);
    }
    if (pImpl->neuromodulationEnabled) {
        pImpl->config->set("neuromodulation_enabled", true, ConfigSource::Runtime);
    }
    if (pImpl->memoryEnabled) {
        pImpl->config->set("memory_enabled", true, ConfigSource::Runtime);
    }
    if (pImpl->predictionEnabled) {
        pImpl->config->set("prediction_enabled", true, ConfigSource::Runtime);
    }
    
    // Set simulation mode
    pImpl->config->set("simulation_mode", pImpl->simulationMode, ConfigSource::Runtime);
    
    // Set experimental features flag
    pImpl->config->set("experimental_enabled", pImpl->experimentalFeatures, ConfigSource::Runtime);
    
    // Set debug mode
    pImpl->config->set("debug_mode", pImpl->debugMode, ConfigSource::Runtime);
    
    return pImpl->config;
}

void ConfigBuilder::reset() {
    pImpl.reset(new Impl());
}

std::string ConfigBuilder::summary() const {
    return pImpl->config->summary();
}

} // namespace nlm