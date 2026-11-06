#include "AgentBrain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Private helper method to initialize neuron manager
void AgentBrain::initializeNeuronManager() {
    NLM_LOG_INFO("Initializing AgentBrain neuron manager...");
    
    // Use the neuron manager to distribute neurons
    neuronManager_.addNeurons(brain_);
    
    NLM_LOG_INFO("Neuron manager initialization complete:");
    NLM_LOG_INFO("  Motor neurons: " + std::to_string(neuronManager_.getMotorNeuronsCount()));
    NLM_LOG_INFO("  Sensory neurons: " + std::to_string(neuronManager_.getSensoryNeuronsCount()));
    NLM_LOG_INFO("  Internal neurons: " + std::to_string(neuronManager_.getInternalNeuronsCount()));
}

// Private helper method to validate state
void AgentBrain::validateState() const {
    NLM_LOG_DEBUG("Validating AgentBrain state...");
    
    // Validate brain pointer
    if (!AgentBrainStateValidator::validateBrainPointer(brain_)) {
        NLM_LOG_ERROR("Invalid brain pointer in AgentBrain");
        return;
    }
    
    // Validate neuromodulation levels
    if (!AgentBrainStateValidator::validateNeuromodulationLevels(
        dopamineLevel_, noveltyLevel_, curiosityLevel_, predictionError_)) {
        NLM_LOG_WARNING("Neuromodulation level validation warnings detected");
    }
    
    // Validate developmental state
    if (!AgentBrainStateValidator::validateDevelopmentalState(
        developmentalAge_, plasticityModifier_)) {
        NLM_LOG_ERROR("Developmental state validation failed");
    }
    
    // Validate cognitive state
    if (!AgentBrainStateValidator::validateCognitiveState(
        lastVision_, lastTouch_)) {
        NLM_LOG_ERROR("Cognitive state validation failed");
    }
    
    NLM_LOG_DEBUG("State validation complete");
}

// Private helper method to apply configuration
void AgentBrain::applyConfiguration(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_WARNING("Cannot apply configuration: null config pointer");
        return;
    }
    
    NLM_LOG_INFO("Applying AgentBrain configuration...");
    
    // Use the config manager to apply defaults and overrides
    AgentBrainConfigManager::setDefaultConfig(config);
    AgentBrainConfigManager::applyConfigOverrides(config);
    
    // Validate configuration
    if (!AgentBrainConfigManager::validateAgentBrainConfig(*config)) {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    NLM_LOG_INFO("Configuration application complete");
}

// Private helper method to log configuration
void AgentBrain::logConfiguration(const std::shared_ptr<Config>& config) const {
    if (!config) {
        NLM_LOG_ERROR("Cannot log configuration: null config pointer");
        return;
    }
    
    NLM_LOG_INFO("AgentBrain Configuration:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
}

} // namespace nlm