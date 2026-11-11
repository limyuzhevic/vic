#!/usr/bin/env cpp

/**
 * Configuration Management and Validation Module
 * 
 * Enhanced configuration system for NLM with validation, presets, and advanced options.
 */

#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include <algorithm>
#include <unordered_set>
#include <cmath>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager() {
        initializeDefaultConfig();
        initializeValidationRules();
        initializePresets();
    }
    
    /**
     * Validate configuration and return list of errors
     */
    std::vector<std::string> validateConfig(const Config& config) const {
        std::vector<std::string> errors;
        
        // Validate neuron count
        auto neuronCount = config.get<int64_t>("brain.neuron_count");
        if (!neuronCount.has_value()) {
            errors.push_back("Missing required configuration: brain.neuron_count");
        } else if (*neuronCount <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        } else if (*neuronCount > 100000) {
            errors.push_back("brain.neuron_count too large (max 100000)");
        }
        
        // Validate region count
        auto regionCount = config.get<int64_t>("brain.region_count");
        if (!regionCount.has_value()) {
            errors.push_back("Missing required configuration: brain.region_count");
        } else if (*regionCount <= 0) {
            errors.push_back("brain.region_count must be positive");
        }
        
        // Validate connection probability
        auto connProb = config.get<float>("brain.connection_probability");
        if (connProb.has_value()) {
            if (*connProb < 0.0f || *connProb > 1.0f) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
        
        // Validate simulation timestep
        auto timestep = config.get<double>("simulation.timestep");
        if (timestep.has_value()) {
            if (*timestep <= 0.0) {
                errors.push_back("simulation.timestep must be positive");
            } else if (*timestep > 0.1) {
                errors.push_back("simulation.timestep too large (max 0.1)");
            }
        }
        
        // Validate plasticity parameters
        auto stdpLTP = config.get<float>("plasticity.stdp.ltp_weight");
        if (stdpLTP.has_value() && *stdpLTP < 0.0f) {
            errors.push_back("plasticity.stdp.ltp_weight must be positive");
        }
        
        auto stdpLTD = config.get<float>("plasticity.stdp.ltd_weight");
        if (stdpLTD.has_value() && *stdpLTD < 0.0f) {
            errors.push_back("plasticity.stdp.ltd_weight must be positive");
        }
        
        // Validate that STDP LTP > LTD
        if (stdpLTP.has_value() && stdpLTD.has_value() && *stdpLTP <= *stdpLTD) {
            errors.push_back("plasticity.stdp.ltp_weight should be greater than ltd_weight");
        }
        
        // Validate neuromodulation parameters
        auto dopamineScale = config.get<float>("neuromod.dopamine.scale");
        if (dopamineScale.has_value() && *dopamineScale < 0.0f) {
            errors.push_back("neuromod.dopamine.scale must be positive");
        }
        
        // Validate development parameters
        auto synaptogenesisRate = config.get<float>("development.synaptogenesis.rate");
        if (synaptogenesisRate.has_value() && *synaptogenesisRate < 0.0f) {
            errors.push_back("development.synaptogenesis.rate must be positive");
        }
        
        // Validate that connection probability and synaptogenesis are compatible
        if (connProb.has_value() && synaptogenesisRate.has_value()) {
            float maxSynaptogenesis = *connProb * 0.01f;  // Synaptogenesis shouldn't exceed 1% of connections
            if (*synaptogenesisRate > maxSynaptogenesis) {
                errors.push_back("development.synaptogenesis.rate too high for given connection_probability");
            }
        }
        
        return errors;
    }
    
    /**
     * Get all available configuration options with descriptions
     */
    std::vector<std::pair<std::string, std::string>> getAllConfigOptions() const {
        std::vector<std::pair<std::string, std::string>> options;
        
        // Core brain configuration
        options.emplace_back("brain.neuron_count", "Total number of neurons in the brain");
        options.emplace_back("brain.region_count", "Number of brain regions");
        options.emplace_back("brain.connection_probability", "Probability of connection between neurons");
        options.emplace_back("brain.initial_weight_mean", "Mean initial synaptic weight");
        options.emplace_back("brain.initial_weight_std", "Standard deviation of initial weights");
        options.emplace_back("brain.v_thresh", "Neuron threshold potential (mV)");
        options.emplace_back("brain.v_rest", "Resting potential (mV)");
        options.emplace_back("brain.v_reset", "Reset potential after spike (mV)");
        
        // Simulation parameters
        options.emplace_back("simulation.timestep", "Simulation timestep (seconds)");
        options.emplace_back("simulation.max_steps", "Maximum simulation steps");
        options.emplace_back("simulation.random_seed", "Random seed for reproducibility");
        
        // Plasticity configuration
        options.emplace_back("plasticity.stdp.enable", "Enable spike-timing dependent plasticity");
        options.emplace_back("plasticity.stdp.ltp_weight", "STDP long-term potentiation weight");
        options.emplace_back("plasticity.stdp.ltd_weight", "STDP long-term depression weight");
        options.emplace_back("plasticity.stdp.tau_plus", "STDP time constant for potentiation (ms)");
        options.emplace_back("plasticity.stdp.tau_minus", "STDP time constant for depression (ms)");
        options.emplace_back("plasticity.hebbian.enable", "Enable Hebbian learning");
        options.emplace_back("plasticity.hebbian.learning_rate", "Hebbian learning rate");
        options.emplace_back("plasticity.structural.enable", "Enable structural plasticity");
        options.emplace_back("plasticity.synaptogenesis.rate", "Synaptogenesis rate");
        options.emplace_back("plasticity.pruning.rate", "Pruning rate");
        
        // Neuromodulation configuration
        options.emplace_back("neuromod.dopamine.scale", "Dopamine modulation scale");
        options.emplace_back("neuromod.curiosity.enable", "Enable curiosity-driven exploration");
        options.emplace_back("neuromod.novelty.enable", "Enable novelty detection");
        options.emplace_back("neuromod.prediction.error_threshold", "Prediction error threshold");
        
        // Development configuration
        options.emplace_back("development.enable", "Enable developmental processes");
        options.emplace_back("development.initial_plasticity", "Initial plasticity rate");
        options.emplace_back("development.plasticity_decay_rate", "Plasticity decay rate with development");
        options.emplace_back("development.synaptogenesis.rate", "Synaptogenesis rate");
        options.emplace_back("development.pruning.rate", "Pruning rate");
        options.emplace_back("development.maturation.threshold", "Maturation threshold");
        
        // Memory configuration
        options.emplace_back("memory.working.capacity", "Working memory capacity");
        options.emplace_back("memory.working.decay_rate", "Working memory decay rate");
        options.emplace_back("memory.episodic.max_episodes", "Maximum episodic memory episodes");
        options.emplace_back("memory.associative.max_associations", "Maximum associative memory associations");
        
        // Cognition configuration
        options.emplace_back("cognition.planner.enable", "Enable neural planning");
        options.emplace_back("cognition.planner.exploration_bonus", "Exploration bonus for planning");
        options.emplace_back("cognition.concept.formation_threshold", "Concept formation threshold");
        options.emplace_back("cognition.attention.inhibition_strength", "Attention inhibition strength");
        
        // Prediction configuration
        options.emplace_back("prediction.enable", "Enable prediction system");
        options.emplace_back("prediction.horizon", "Prediction horizon (steps)");
        options.emplace_back("prediction.error_decay_rate", "Prediction error decay rate");
        
        // Environment configuration
        options.emplace_back("environment.width", "Environment width");
        options.emplace_back("environment.height", "Environment height");
        options.emplace_back("environment.vision.width", "Vision width");
        options.emplace_back("environment.vision.height", "Vision height");
        options.emplace_back("environment.energy.decay_rate", "Energy decay rate");
        
        return options;
    }
    
    /**
     * Get preset configurations for different use cases
     */
    std::vector<std::string> getAvailablePresets() const {
        std::vector<std::string> presets;
        presets.push_back("default");
        presets.push_back("fast");
        presets.push_back("accurate");
        presets.push_back("curious");
        presets.push_back("social");
        presets.push_back("developmental");
        presets.push_back("experimental");
        return presets;
    }
    
    /**
     * Get a preset configuration
     */
    Config getPreset(const std::string& presetName) const {
        Config config;
        
        if (presetName == "default") {
            // Balanced configuration
            config.set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config.set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config.set("simulation.timestep", 0.001, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.01f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.012f, ConfigSource::Default);
            config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", false, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.0001f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00001f, ConfigSource::Default);
            
        } else if (presetName == "fast") {
            // Fast, simple configuration for quick testing
            config.set("brain.neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config.set("brain.connection_probability", 0.05f, ConfigSource::Default);
            config.set("simulation.timestep", 0.002, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.02f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.015f, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 2.0f, ConfigSource::Default);
            config.set("development.enable", false, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.0005f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00005f, ConfigSource::Default);
            
        } else if (presetName == "accurate") {
            // High accuracy configuration for detailed experiments
            config.set("brain.neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(5), ConfigSource::Default);
            config.set("brain.connection_probability", 0.15f, ConfigSource::Default);
            config.set("simulation.timestep", 0.0005, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.005f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.008f, ConfigSource::Default);
            config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 0.5f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", true, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.00005f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.000005f, ConfigSource::Default);
            config.set("development.maturation.threshold", 0.8f, ConfigSource::Default);
            
        } else if (presetName == "curious") {
            // Curiosity-driven exploration configuration
            config.set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(2), ConfigSource::Default);
            config.set("brain.connection_probability", 0.12f, ConfigSource::Default);
            config.set("simulation.timestep", 0.001, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.015f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.01f, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 1.5f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", true, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.00008f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00002f, ConfigSource::Default);
            
        } else if (presetName == "social") {
            // Social learning configuration
            config.set("brain.neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config.set("brain.connection_probability", 0.18f, ConfigSource::Default);
            config.set("simulation.timestep", 0.001, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.008f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.006f, ConfigSource::Default);
            config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 0.8f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", true, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.00003f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00001f, ConfigSource::Default);
            
        } else if (presetName == "developmental") {
            // Development-focused configuration
            config.set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(4), ConfigSource::Default);
            config.set("brain.connection_probability", 0.1f, ConfigSource::Default);
            config.set("simulation.timestep", 0.0008, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.003f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.004f, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 0.7f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", true, ConfigSource::Default);
            config.set("development.initial_plasticity", 1.5f, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.0001f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00002f, ConfigSource::Default);
            config.set("development.maturation.threshold", 0.9f, ConfigSource::Default);
            
        } else if (presetName == "experimental") {
            // Experimental configuration with high exploration
            config.set("brain.neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
            config.set("brain.region_count", static_cast<int64_t>(6), ConfigSource::Default);
            config.set("brain.connection_probability", 0.2f, ConfigSource::Default);
            config.set("simulation.timestep", 0.0005, ConfigSource::Default);
            config.set("plasticity.stdp.enable", true, ConfigSource::Default);
            config.set("plasticity.stdp.ltp_weight", 0.02f, ConfigSource::Default);
            config.set("plasticity.stdp.ltd_weight", 0.025f, ConfigSource::Default);
            config.set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config.set("neuromod.dopamine.scale", 2.5f, ConfigSource::Default);
            config.set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config.set("neuromod.novelty.enable", true, ConfigSource::Default);
            config.set("development.enable", true, ConfigSource::Default);
            config.set("development.synaptogenesis.rate", 0.0002f, ConfigSource::Default);
            config.set("development.pruning.rate", 0.00004f, ConfigSource::Default);
        }
        
        return config;
    }
    
    /**
     * Merge two configurations (source overrides destination)
     */
    Config mergeConfigs(const Config& source, const Config& destination) const {
        Config merged = destination;
        
        for (const auto& entry : source.getKeys()) {
            auto value = source.getByKey(entry);
            if (value.has_value()) {
                // Try to find the corresponding type in destination
                auto destValue = destination.getByKey(entry);
                if (destValue.has_value()) {
                    // Replace if sources differ
                    if (std::visit([](const auto& a, const auto& b) { return a != b; }, 
                                  value.value(), destValue.value())) {
                        merged.set(entry, value.value(), ConfigSource::Runtime);
                    }
                } else {
                    // Add if not in destination
                    merged.set(entry, value.value(), ConfigSource::Runtime);
                }
            }
        }
        
        return merged;
    }
    
private:
    void initializeDefaultConfig() {
        // Basic configuration defaults
        // These are the minimum required values
        // Note: Config constructor handles defaults, but we define them here for reference
        
        // Core brain parameters
        // brain.neuron_count: total number of neurons
        // brain.region_count: number of brain regions  
        // brain.connection_probability: connection probability
        // brain.initial_weight_mean: initial weight mean
        // brain.initial_weight_std: initial weight std dev
        // brain.v_thresh: neuron threshold
        // brain.v_rest: resting potential
        // brain.v_reset: reset potential
        // brain.tau_mem: membrane time constant
        // brain.tau_ref: refractory period
        
        // Simulation parameters
        // simulation.timestep: simulation timestep
        // simulation.max_steps: max simulation steps
        // simulation.random_seed: random seed
        
        // Plasticity parameters
        // plasticity.stdp.enable: STDP enable flag
        // plasticity.stdp.ltp_weight: STDP LTP weight
        // plasticity.stdp.ltd_weight: STDP LTD weight
        // plasticity.stdp.tau_plus: STDP tau plus
        // plasticity.stdp.tau_minus: STDP tau minus
        // plasticity.hebbian.enable: Hebbian enable flag
        // plasticity.hebbian.learning_rate: Hebbian learning rate
        // plasticity.structural.enable: structural plasticity enable flag
        // plasticity.synaptogenesis.rate: synaptogenesis rate
        // plasticity.pruning.rate: pruning rate
        
        // Neuromodulation parameters
        // neuromod.dopamine.scale: dopamine scale
        // neuromod.curiosity.enable: curiosity enable flag
        // neuromod.novelty.enable: novelty enable flag
        // neuromod.prediction.error_threshold: prediction error threshold
        
        // Development parameters
        // development.enable: development enable flag
        // development.initial_plasticity: initial plasticity rate
        // development.plasticity_decay_rate: plasticity decay rate
        // development.synaptogenesis.rate: synaptogenesis rate
        // development.pruning.rate: pruning rate
        // development.maturation.threshold: maturation threshold
        
        // Memory parameters
        // memory.working.capacity: working memory capacity
        // memory.working.decay_rate: working memory decay rate
        // memory.episodic.max_episodes: max episodic episodes
        // memory.associative.max_associations: max associative associations
        
        // Cognition parameters
        // cognition.planner.enable: planner enable flag
        // cognition.planner.exploration_bonus: exploration bonus
        // cognition.concept.formation_threshold: concept formation threshold
        // cognition.attention.inhibition_strength: attention inhibition strength
        
        // Prediction parameters
        // prediction.enable: prediction enable flag
        // prediction.horizon: prediction horizon
        // prediction.error_decay_rate: prediction error decay rate
        
        // Environment parameters
        // environment.width: environment width
        // environment.height: environment height
        // environment.vision.width: vision width
        // environment.vision.height: vision height
        // environment.energy.decay_rate: energy decay rate
    }
    
    void initializeValidationRules() {
        // Define validation rules for different configuration types
        // These are used in the validateConfig method
        
        // Value ranges for numeric parameters
        validationRanges_[
            std::make_pair("brain.neuron_count", std::numeric_limits<int64_t>::lowest()),
            std::make_pair("brain.neuron_count", std::numeric_limits<int64_t>::max())
        ] = {1, 100000, "Neuron count must be between 1 and 100000"};
        
        validationRanges_[
            std::make_pair("brain.region_count", std::numeric_limits<int64_t>::lowest()),
            std::make_pair("brain.region_count", std::numeric_limits<int64_t>::max())
        ] = {1, 100, "Region count must be between 1 and 100"};
        
        validationRanges_[
            std::make_pair("brain.connection_probability", -1.0f),
            std::make_pair("brain.connection_probability", 1.0f)
        ] = {0.0f, 1.0f, "Connection probability must be between 0.0 and 1.0"};
        
        validationRanges_[
            std::make_pair("simulation.timestep", 0.0),
            std::make_pair("simulation.timestep", std::numeric_limits<double>::max())
        ] = {0.0001f, 0.1f, "Simulation timestep must be between 0.0001s and 0.1s"};
        
        validationRanges_[
            std::make_pair("plasticity.stdp.ltp_weight", 0.0),
            std::make_pair("plasticity.stdp.ltp_weight", std::numeric_limits<float>::max())
        ] = {0.001f, 1.0f, "STDP LTP weight must be between 0.001 and 1.0"};
        
        validationRanges_[
            std::make_pair("plasticity.stdp.ltd_weight", 0.0),
            std::make_pair("plasticity.stdp.ltd_weight", std::numeric_limits<float>::max())
        ] = {0.001f, 1.0f, "STDP LTD weight must be between 0.001 and 1.0"};
        
        validationRanges_[
            std::make_pair("neuromod.dopamine.scale", 0.0),
            std::make_pair("neuromod.dopamine.scale", std::numeric_limits<float>::max())
        ] = {0.0f, 5.0f, "Dopamine scale must be between 0.0 and 5.0"};
        
        validationRanges_[
            std::make_pair("development.synaptogenesis.rate", 0.0),
            std::make_pair("development.synaptogenesis.rate", 1.0)
        ] = {0.00001f, 0.5f, "Synaptogenesis rate must be between 0.00001 and 0.5"};
        
        validationRanges_[
            std::make_pair("development.pruning.rate", 0.0),
            std::make_pair("development.pruning.rate", 1.0)
        ] = {0.000001f, 0.1f, "Pruning rate must be between 0.000001 and 0.1"};
    }
    
    void initializePresets() {
        // Preset configurations are initialized in getPreset method
    }
    
    struct ValidationRange {
        float min;
        float max;
        std::string errorMessage;
    };
    
    std::vector<std::pair<std::pair<std::string, double>, std::pair<std::string, double>>> validationRanges_;
};

} // namespace nlm
