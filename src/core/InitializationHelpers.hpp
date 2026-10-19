#pragma once

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <memory>

namespace nlm {

// Forward declaration to avoid circular includes
class Logger;

// Helper functions for common initialization patterns

/**
 * Initialize the logging system with default configuration
 * Reduces duplication of logger setup code
 */
inline void initializeLogger() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    NLM_LOG_INFO("Logger initialized successfully");
}

/**
 * Initialize configuration with default values for Phase 2
 * Sets up standard neural simulation parameters and defaults
 */
inline std::shared_ptr<Config> initializePhase2Config() {
    auto config = std::make_shared<Config>();
    
    // Set default values for Phase 2
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return config;
}

/**
 * Create and initialize a brain with the given configuration
 * Combines brain creation and initialization into one step
 */
inline std::shared_ptr<Brain> createAndInitializeBrain(std::shared_ptr<Config> config) {
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain from configuration! "
                     "Please check configuration parameters and available resources.");
        return nullptr;
    }
    
    brain->logStatus();
    return brain;
}

/**
 * Execute a complete brain initialization cycle for an experiment
 * Combines reset + initialize pattern used for multiple experiments
 */
inline std::shared_ptr<Brain> prepareBrainForExperiment(std::shared_ptr<Config> config) {
    auto brain = std::make_shared<Brain>(config);
    
    // Reset brain state for fresh experiment
    brain->reset();
    
    // Initialize brain with configuration
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for experiment! "
                     "Brain may be in an invalid state; consider resetting before retry.");
        return nullptr;
    }
    
    brain->logStatus();
    return brain;
}

/**
 * Initialize all memory systems in the brain
 * Reduces duplication of memory system initialization code
 */
inline void initializeBrainMemorySystems(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    auto* associativeMemory = brain->getAssociativeMemory();
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    auto* novelty = brain->getNovelty();
    auto* curiosity = brain->getCuriosity();
    
    if (workingMemory) {
        workingMemory->initialize(brain.get());
        workingMemory->setCapacity(100);  // Default capacity
    }
    
    if (episodicMemory) {
        episodicMemory->initialize(brain.get());
        episodicMemory->setMaxEpisodes(1000);
    }
    
    if (associativeMemory) {
        associativeMemory->initialize(brain.get());
    }
    
    if (planner) {
        planner->initialize(brain.get());
        planner->setPlanningDepth(5);
    }
    
    if (conceptFormation) {
        conceptFormation->initialize(brain.get());
    }
    
    if (attention) {
        attention->initialize(brain.get());
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    if (novelty) {
        novelty->initialize(brain.get());
    }
    
    if (curiosity) {
        curiosity->initialize(brain.get());
    }
}

/**
 * Configure plasticity systems with parameters from config
 * Reduces duplication of plasticity configuration code
 */
inline void configurePlasticitySystems(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (!brain || !config) return;
    
    auto* stdp = brain->getSTDP();
    auto* hebbian = brain->getHebbian();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    if (stdp) {
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    if (structuralPlasticity) {
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
}

/**
 * Print configuration summary to logs
 * Reduces duplication of config logging code
 */
inline void logConfigurationSummary(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_WARNING("Configuration is null - cannot log summary");
        return;
    }
    
    NLM_LOG_INFO("=== Configuration Summary ===");
    NLM_LOG_INFO("random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("=== End Configuration Summary ===");
}

} // namespace nlm

