#include "BrainFactory.hpp"
#include "../Brain.hpp"
#include <stdexcept>

namespace nlm {

BrainFactory::BrainFactory() = default;

BrainFactory::~BrainFactory() = default;

std::unique_ptr<Brain> BrainFactory::createBrain(std::shared_ptr<Config> config) {
    validateConfig(config);
    auto brain = std::make_unique<Brain>(config);
    if (!initializeBrain(brain, config)) {
        throw std::runtime_error("Failed to initialize brain");
    }
    return brain;
}

void BrainFactory::validateConfig(std::shared_ptr<Config> config) {
    if (!config) {
        throw std::invalid_argument("Config cannot be null");
    }
    
    // Validate required parameters
    auto neuronCountOpt = config->get<size_t>("neuron_count");
    auto regionCountOpt = config->get<size_t>("region_count");
    
    if (neuronCountOpt && *neuronCountOpt == 0) {
        throw std::invalid_argument("neuron_count must be positive");
    }
    if (regionCountOpt && *regionCountOpt == 0) {
        throw std::invalid_argument("region_count must be positive");
    }
    
    // Validate timestep
    auto timestepOpt = config->get<double>("simulation_timestep");
    if (timestepOpt && (*timestepOpt <= 0.0 || *timestepOpt > 1.0)) {
        throw std::invalid_argument("simulation_timestep must be between 0 and 1.0");
    }
}

bool BrainFactory::initializeBrain(std::unique_ptr<Brain>& brain, std::shared_ptr<Config> config) {
    if (!brain) {
        return false;
    }
    
    try {
        // Configure plasticity parameters
        auto* brainPtr = brain.get();
        
        // Initialize neuromodulation systems
        if (auto dopamineOpt = config->get<float>("dopamine_level")) {
            if (auto* dopamine = brainPtr->getDopamine()) {
                dopamine->setLevel(*dopamineOpt);
            }
        }
        
        if (auto noveltyThresholdOpt = config->get<float>("novelty_threshold")) {
            if (auto* novelty = brainPtr->getNovelty()) {
                novelty->setThreshold(*noveltyThresholdOpt);
            }
        }
        
        if (auto curiosityLevelOpt = config->get<float>("curiosity_level")) {
            if (auto* curiosity = brainPtr->getCuriosity()) {
                curiosity->setLevel(*curiosityLevelOpt);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

} // namespace nlm
