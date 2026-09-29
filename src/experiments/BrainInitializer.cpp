#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

std::shared_ptr<Brain> BrainInitializer::createBrain(std::shared_ptr<Config> config) {
    try {
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return nullptr;
        }
        
        NLM_LOG_INFO("Brain created and initialized successfully");
        return brain;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create brain: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<Brain> BrainInitializer::createBrain(size_t neuronCount) {
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", neuronCount);
        config->set("region_count", 1);
        
        return createBrain(config);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create brain with default config: " + std::string(e.what()));
        return nullptr;
    }
}

bool BrainInitializer::validateBrain(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        NLM_LOG_ERROR("Cannot validate null brain");
        return false;
    }
    
    bool isValid = true;
    
    // Check essential systems
    if (!brain->getWorkingMemory()) {
        NLM_LOG_WARNING("Working memory not initialized");
        isValid = false;
    }
    
    if (!brain->getEpisodicMemory()) {
        NLM_LOG_WARNING("Episodic memory not initialized");
        isValid = false;
    }
    
    if (!brain->getDopamine()) {
        NLM_LOG_WARNING("Dopamine system not initialized");
        isValid = false;
    }
    
    if (!brain->getPredictionSystem()) {
        NLM_LOG_WARNING("Prediction system not initialized");
        isValid = false;
    }
    
    if (!brain->getDevelopmentSystem()) {
        NLM_LOG_WARNING("Development system not initialized");
        isValid = false;
    }
    
    return isValid;
}

bool BrainInitializer::isBrainInitialized(std::shared_ptr<Brain> brain) const {
    return brain != nullptr && brain->getWorkingMemory() != nullptr && 
           brain->getEpisodicMemory() != nullptr;
}

std::shared_ptr<Config> BrainInitializer::createMinimalBrainConfig(size_t neuronCount) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", neuronCount);
    config->set("region_count", 1);
    
    // Add essential brain constants
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    return config;
}

std::shared_ptr<Config> BrainInitializer::createSimulationBrainConfig(const Phase6Config& phaseConfig) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", phaseConfig.neuronCount);
    config->set("region_count", phaseConfig.regionCount);
    config->set("connection_probability", phaseConfig.connectionProbability);
    config->set("stdp_ltp_weight", ExperimentConstants::DEFAULT_STDP_LTP_WEIGHT);
    config->set("stdp_ltd_weight", ExperimentConstants::DEFAULT_STDP_LTD_WEIGHT);
    config->set("synaptogenesis_rate", ExperimentConstants::DEFAULT_SYNAPTOGENESIS_RATE);
    config->set("pruning_rate", ExperimentConstants::DEFAULT_PRUNING_RATE);
    
    return config;
}

} // namespace nlm
