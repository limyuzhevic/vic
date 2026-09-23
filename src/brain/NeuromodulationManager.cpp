#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain{nullptr};
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    Impl() = default;
};

NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl()) {}

NeuromodulationManager::~NeuromodulationManager() {
    delete pImpl;
}

bool NeuromodulationManager::initialize(Brain* brain) {
    if (!brain) {
        return false;
    }
    
    pImpl->brain = brain;
    
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    status_.dopamineLevel = 0.0f;
    status_.curiosityLevel = 0.0f;
    status_.predictionErrorLevel = 0.0f;
    status_.noveltyLevel = 0.0f;
    
    return true;
}

void NeuromodulationManager::update(float timestep) {
    // Update neuromodulators
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        status_.dopamineLevel = pImpl->dopamine->getLevel();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
        status_.curiosityLevel = pImpl->curiosity->getLevel();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update(timestep);
        status_.predictionErrorLevel = pImpl->predictionError->getLevel();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
        status_.noveltyLevel = pImpl->novelty->getLevel();
    }
}

void NeuromodulationManager::applyToNeurons(std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Apply dopamine to neurons (increases excitability)
    float dopamineLevel = status_.dopamineLevel;
    if (dopamineLevel > 0.0f) {
        float excitabilityMod = dopamineLevel * 0.5f;
        for (auto& region : regions) {
            for (auto& population : region->getPopulations()) {
                for (auto* neuron : population->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

bool NeuromodulationManager::validateConfiguration(std::shared_ptr<Config> config) const {
    if (!config) {
        return false;
    }
    
    // Validate neuromodulator parameters
    bool valid = true;
    
    // Check for known neuromodulator parameters
    auto dopamineLevels = config->get<float>("dopamine_levels");
    auto curiosityLevels = config->get<float>("curiosity_levels");
    auto predictionErrorLevels = config->get<float>("prediction_error_levels");
    
    if (dopamineLevels && (*dopamineLevels < 0.0f || *dopamineLevels > 10.0f)) {
        valid = false;
    }
    
    if (curiosityLevels && (*curiosityLevels < 0.0f || *curiosityLevels > 10.0f)) {
        valid = false;
    }
    
    if (predictionErrorLevels && (*predictionErrorLevels < 0.0f || *predictionErrorLevels > 10.0f)) {
        valid = false;
    }
    
    return valid;
}

void NeuromodulationManager::setLevels(float dopamine, float curiosity, float predictionError, float novelty) {
    if (dopamine >= 0.0f && dopamine <= 10.0f) {
        status_.dopamineLevel = dopamine;
    }
    if (curiosity >= 0.0f && curiosity <= 10.0f) {
        status_.curiosityLevel = curiosity;
    }
    if (predictionError >= 0.0f && predictionError <= 10.0f) {
        status_.predictionErrorLevel = predictionError;
    }
    if (novelty >= 0.0f && novelty <= 10.0f) {
        status_.noveltyLevel = novelty;
    }
}

} // namespace nlm
