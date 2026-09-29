#include "BrainConfig.hpp"
#include "Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

BrainConfig::BrainConfig() {
    initializeDefaults();
}

BrainConfig::BrainConfig(std::shared_ptr<Config> config) {
    initializeDefaults();
    if (config) {
        applyConfigFile(config);
    }
}

void BrainConfig::initializeDefaults() {
    // Default region configuration
    addRegionConfig("DefaultRegion", 1000, 0.1f, 0.2f);
    
    // Default memory configuration
    memoryConfig_ = {
        100,              // workingMemoryCapacity
        1000,             // maxEpisodicEpisodes
        0.3f,             // consolidationThreshold
        100,              // replayInterval
        1000              // consolidationInterval
    };
    
    // Default plasticity configuration
    plasticityConfig_ = {
        0.01f,            // stdpLTPWeight
        0.012f,           // stdpLTDWeight
        20.0f,            // stdpTimeConstant
        1e-5f,            // synaptogenesisRate
        1e-6f             // pruningRate
    };
    
    // Default development configuration
    developmentConfig_ = {
        1.0f,             // plasticityModulation
        0.01f,            // excitabilityChangeRate
        1e-4f             // structuralPlasticityRate
    };
    
    // Default neuromodulation configuration
    neuromodulationConfig_ = {
        0.5f,             // curiosityThreshold
        0.99f,            // noveltyDecayRate
        1.0f              // dopamineInfluence
    };
    
    // Simulation parameters
    timestep_ = 0.001f;
    checkpointDir_ = "checkpoints";
    randomSeed_ = 42;  // Default seed
}

void BrainConfig::addRegionConfig(const std::string& name, size_t neuronCount,
                                 float connectionProbability, float weightRange) {
    RegionConfig config;
    config.name = name;
    config.neuronCount = neuronCount;
    config.connectionProbability = std::clamp(connectionProbability, 0.0f, 1.0f);
    config.weightRange = weightRange;
    regionConfigs_.push_back(config);
}

bool BrainConfig::validate() const {
    validationError_.clear();
    
    if (!validateRegionConfigs()) {
        return false;
    }
    
    if (!validateMemoryConfig()) {
        return false;
    }
    
    if (!validatePlasticityConfig()) {
        return false;
    }
    
    if (!validateDevelopmentConfig()) {
        return false;
    }
    
    if (!validateNeuromodulationConfig()) {
        return false;
    }
    
    if (timestep_ <= 0.0f) {
        validationError_ = "Simulation timestep must be positive";
        return false;
    }
    
    return true;
}

bool BrainConfig::validateRegionConfigs() const {
    if (regionConfigs_.empty()) {
        validationError_ = "At least one region configuration must be specified";
        return false;
    }
    
    for (const auto& region : regionConfigs_) {
        if (region.neuronCount == 0) {
            validationError_ = "Region \"" + region.name + "\" must have at least one neuron";
            return false;
        }
        
        if (region.connectionProbability < 0.0f || region.connectionProbability > 1.0f) {
            validationError_ = "Region \"" + region.name + "\" has invalid connection probability";
            return false;
        }
    }
    
    return true;
}

bool BrainConfig::validateMemoryConfig() const {
    if (memoryConfig_.workingMemoryCapacity == 0) {
        validationError_ = "Working memory capacity must be positive";
        return false;
    }
    
    if (memoryConfig_.maxEpisodicEpisodes == 0) {
        validationError_ = "Max episodic episodes must be positive";
        return false;
    }
    
    if (memoryConfig_.consolidationThreshold < 0.0f || memoryConfig_.consolidationThreshold > 1.0f) {
        validationError_ = "Consolidation threshold must be between 0 and 1";
        return false;
    }
    
    return true;
}

bool BrainConfig::validatePlasticityConfig() const {
    if (plasticityConfig_.stdpLTPWeight < 0.0f || plasticityConfig_.stdpLTPWeight > 1.0f) {
        validationError_ = "STDP LTP weight must be between 0 and 1";
        return false;
    }
    
    if (plasticityConfig_.stdpLTDWeight < 0.0f || plasticityConfig_.stdpLTDWeight > 1.0f) {
        validationError_ = "STDP LTD weight must be between 0 and 1";
        return false;
    }
    
    if (plasticityConfig_.stdpTimeConstant <= 0.0f) {
        validationError_ = "STDP time constant must be positive";
        return false;
    }
    
    return true;
}

bool BrainConfig::validateDevelopmentConfig() const {
    if (developmentConfig_.plasticityModulation <= 0.0f) {
        validationError_ = "Plasticity modulation must be positive";
        return false;
    }
    
    return true;
}

bool BrainConfig::validateNeuromodulationConfig() const {
    if (neuromodulationConfig_.curiosityThreshold < 0.0f || neuromodulationConfig_.curiosityThreshold > 1.0f) {
        validationError_ = "Curiosity threshold must be between 0 and 1";
        return false;
    }
    
    if (neuromodulationConfig_.noveltyDecayRate <= 0.0f || neuromodulationConfig_.noveltyDecayRate > 1.0f) {
        validationError_ = "Novelty decay rate must be between 0 and 1";
        return false;
    }
    
    return true;
}

void BrainConfig::applyConfigFile(std::shared_ptr<Config> config) {
    if (!config) return;
    
    // Override defaults with config file values if present
    if (auto timestepOpt = config->getOr<float>("simulation_timestep")) {
        timestep_ = *timestepOpt;
    }
    
    if (auto seedOpt = config->getOr<uint64_t>("random_seed")) {
        randomSeed_ = *seedOpt;
    }
    
    if (auto checkpointOpt = config->getOr<std::string>("checkpoint_dir")) {
        checkpointDir_ = *checkpointOpt;
    }
    
    // Apply system-specific configurations
    applySystemConfigs(config);
}

void BrainConfig::applySystemConfigs(std::shared_ptr<Config> config) {
    // This method would apply system-specific configurations
    // For now, it just serves as a placeholder for future extensions
    (void)config; // Avoid unused parameter warning
}

void BrainConfig::applyToBrain(Brain& brain) const {
    if (!brain.getConfig()) {
        NLM_LOG_ERROR("Brain has no configuration - cannot apply config");
        return;
    }
    
    auto config = brain.getConfig();
    
    // Apply memory system configuration
    if (auto workingMemory = brain.getWorkingMemory()) {
        workingMemory->setCapacity(memoryConfig_.workingMemoryCapacity);
    }
    
    if (auto episodicMemory = brain.getEpisodicMemory()) {
        episodicMemory->setMaxEpisodes(memoryConfig_.maxEpisodicEpisodes);
    }
    
    // Apply neuromodulation configuration
    if (auto novelty = brain.getNovelty()) {
        // Set novelty detection parameters
    }
    
    if (auto curiosity = brain.getCuriosity()) {
        // Set curiosity parameters
    }
    
    if (auto dopamine = brain.getDopamine()) {
        // Set dopamine parameters
    }
    
    // Apply plasticity configuration
    if (auto stdp = brain.getSTDP()) {
        stdp->configure(plasticityConfig_.stdpLTPWeight,
                       plasticityConfig_.stdpLTDWeight,
                       plasticityConfig_.stdpTimeConstant);
    }
    
    auto* structuralPlasticity = brain.getStructuralPlasticity();
    if (structuralPlasticity) {
        structuralPlasticity->setSynaptogenesisRate(plasticityConfig_.synaptogenesisRate);
        structuralPlasticity->setPruningRate(plasticityConfig_.pruningRate);
    }
    
    NLM_LOG_INFO("Applied BrainConfig to brain system");
}

} // namespace nlm
