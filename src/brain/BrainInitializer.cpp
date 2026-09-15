#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include <sstream>
#include <iostream>

namespace nlm {

struct BrainInitializer::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    bool initialized;
    std::string errorMessage;
    bool validateConfiguration(const Config* cfg) {
        if (!cfg) {
            return false;
        }
        
        // Check essential parameters
        if (!cfg->has("random_seed") || !cfg->has("simulation_timestep")) {
            errorMessage = "Missing essential configuration parameters";
            return false;
        }
        
        // Validate parameter ranges
        auto seed = cfg->get<int64_t>("random_seed");
        if (!seed) {
            errorMessage = "Invalid random_seed value";
            return false;
        }
        
        auto timestep = cfg->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            errorMessage = "simulation_timestep must be in range (0.0, 1.0]";
            return false;
        }
        
        // Validate neuron count if present
        if (cfg->has("neuron_count")) {
            auto neuronCount = cfg->getOr<size_t>("neuron_count", 1000);
            if (neuronCount == 0) {
                errorMessage = "neuron_count must be positive";
                return false;
            }
        }
        
        // Validate connection probability if present
        if (cfg->has("connection_probability")) {
            auto connProb = cfg->getOr<float>("connection_probability", 0.1f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errorMessage = "connection_probability must be in range [0.0, 1.0]";
                return false;
            }
        }
        
        return true;
    }
    
    void applyPhase6Defaults(Config* cfg) {
        if (!cfg) return;
        
        // Core brain parameters
        if (!cfg->has("random_seed")) {
            cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        
        if (!cfg->has("simulation_timestep")) {
            cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
        }
        
        // Phase 6 specific defaults
        if (!cfg->has("neuron_count")) {
            cfg->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        }
        
        if (!cfg->has("region_count")) {
            cfg->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        }
        
        // STDP parameters
        if (!cfg->has("stdp_ltp_weight")) {
            cfg->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        }
        
        if (!cfg->has("stdp_ltd_weight")) {
            cfg->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        }
        
        if (!cfg->has("stdp_tau")) {
            cfg->set("stdp_tau", 20.0f, ConfigSource::Default);
        }
        
        // Structural plasticity
        if (!cfg->has("synaptogenesis_rate")) {
            cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        }
        
        if (!cfg->has("pruning_rate")) {
            cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
        }
        
        // Memory system settings
        if (!cfg->has("replay_interval")) {
            cfg->set("replay_interval", static_cast<size_t>(100), ConfigSource::Default);
        }
        
        if (!cfg->has("consolidation_interval")) {
            cfg->set("consolidation_interval", static_cast<size_t>(1000), ConfigSource::Default);
        }
        
        // Checkpoint settings
        if (!cfg->has("checkpoint_dir")) {
            cfg->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
        }
        
        // Neuromodulation defaults
        if (!cfg->has("dopamine_level")) {
            cfg->set("dopamine_level", 0.0f, ConfigSource::Default);
        }
        
        if (!cfg->has("curiosity_baseline")) {
            cfg->set("curiosity_baseline", 0.1f, ConfigSource::Default);
        }
        
        // Development stage
        if (!cfg->has("developmental_stage")) {
            // Note: DevelopmentalStage enum needs to be defined for actual usage
            cfg->set("developmental_stage", "Initial", ConfigSource::Default);
        }
        
        // Reward and prediction
        if (!cfg->has("reward_discount_factor")) {
            cfg->set("reward_discount_factor", 0.99, ConfigSource::Default);
        }
        
        if (!cfg->has("prediction_error_decay")) {
            cfg->set("prediction_error_decay", 0.5f, ConfigSource::Default);
        }
        
        // Attention system
        if (!cfg->has("attention_inhibition_strength")) {
            cfg->set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
        }
        
        if (!cfg->has("attention_excitation_strength")) {
            cfg->set("attention_excitation_strength", 1.5f, ConfigSource::Default);
        }
        
        // Concept formation
        if (!cfg->has("concept_learning_rate")) {
            cfg->set("concept_learning_rate", 0.01f, ConfigSource::Default);
        }
        
        if (!cfg->has("concept_similarity_threshold")) {
            cfg->set("concept_similarity_threshold", 0.7f, ConfigSource::Default);
        }
        
        // Logging
        if (!cfg->has("log_level")) {
            cfg->set("log_level", "Info", ConfigSource::Default);
        }
        
        if (!cfg->has("log_to_file")) {
            cfg->set("log_to_file", false, ConfigSource::Default);
        }
        
        if (!cfg->has("log_filename")) {
            cfg->set("log_filename", "nlm.log", ConfigSource::Default);
        }
        
        // Performance monitoring
        if (!cfg->has("enable_performance_monitoring")) {
            cfg->set("enable_performance_monitoring", true, ConfigSource::Default);
        }
        
        if (!cfg->has("performance_report_interval")) {
            cfg->set("performance_report_interval", static_cast<size_t>(1000), ConfigSource::Default);
        }
        
        // System limits
        if (!cfg->has("max_memory_traces")) {
            cfg->set("max_memory_traces", static_cast<size_t>(10000), ConfigSource::Default);
        }
        
        if (!cfg->has("max_episodes")) {
            cfg->set("max_episodes", static_cast<size_t>(5000), ConfigSource::Default);
        }
        
        // Neural parameters
        if (!cfg->has("neuron_threshold_default")) {
            cfg->set("neuron_threshold_default", -55.0, ConfigSource::Default);
        }
        
        if (!cfg->has("neuron_resting_potential_default")) {
            cfg->set("neuron_resting_potential_default", -70.0, ConfigSource::Default);
        }
        
        if (!cfg->has("neuron_reset_potential_default")) {
            cfg->set("neuron_reset_potential_default", -65.0, ConfigSource::Default);
        }
        
        if (!cfg->has("neuron_refractory_period_default")) {
            cfg->set("neuron_refractory_period_default", static_cast<uint32_t>(5), ConfigSource::Default);
        }
    }
    
    static bool initializeLogger(const Config* cfg) {
        if (!cfg) return true; // No config, skip logger init
        
        // In a real implementation, this would initialize the logging system
        // based on log_level and log_to_file settings
        // For now, we just acknowledge the logger will be initialized
        // in the main.cpp file
        return true;
    }
    
    bool initializeBrain(std::shared_ptr<Config> cfg) {
        if (!cfg) {
            errorMessage = "Configuration is null";
            return false;
        }
        
        // Validate configuration first
        if (!validateConfiguration(cfg.get())) {
            return false;
        }
        
        try {
            // Initialize logger
            if (!initializeLogger(cfg.get())) {
                errorMessage = "Failed to initialize logger";
                return false;
            }
            
            // Create brain instance
            brain = std::make_shared<Brain>(cfg);
            
            // Initialize the brain
            if (!brain->initialize()) {
                errorMessage = "Brain initialization failed";
                return false;
            }
            
            // Log initialization status
            brain->logStatus();
            
            initialized = true;
            return true;
            
        } catch (const std::exception& e) {
            errorMessage = std::string("Exception during initialization: ") + e.what();
            return false;
        }
    }
};

BrainInitializer::BrainInitializer(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->initialized = false;
}

BrainInitializer::~BrainInitializer() = default;

bool BrainInitializer::initialize(std::shared_ptr<Config> config) {
    if (config) {
        pImpl->config = config;
    }
    
    if (!pImpl->config) {
        pImpl->config = std::make_shared<Config>();
    }
    
    // Apply Phase 6 defaults
    applyPhase6Defaults(pImpl->config.get());
    
    // Initialize brain
    return pImpl->initializeBrain(pImpl->config);
}

std::shared_ptr<Brain> BrainInitializer::createBrain(std::shared_ptr<Config> config) {
    if (!config) {
        config = std::make_shared<Config>();
    }
    
    applyPhase6Defaults(config.get());
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        // In a real implementation, we might throw an exception or return nullptr
        return nullptr;
    }
    
    return brain;
}

void BrainInitializer::applyPhase6Defaults(std::shared_ptr<Config> config) {
    if (config) {
        pImpl->applyPhase6Defaults(config.get());
    }
}

bool BrainInitializer::validateConfiguration(const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    return pImpl->validateConfiguration(config.get());
}

const std::string& BrainInitializer::getErrorMessage() const {
    return pImpl->errorMessage;
}

bool BrainInitializer::isInitialized() const {
    return pImpl->initialized;
}

void BrainInitializer::reset() {
    pImpl->brain.reset();
    pImpl->config.reset();
    pImpl->initialized = false;
    pImpl->errorMessage.clear();
}

} // namespace nlm