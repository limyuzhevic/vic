#include "NeuromodulationManager.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuromodulationManager::Impl {
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Reward> reward;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Curiosity> curiosity;
    
    class Brain* brain;
    
    // Neuromodulation parameters
    float explorationWeight;
    float learningRate;
    float noiseLevel;
    
    Impl() 
        : brain(nullptr)
        , explorationWeight(0.5f)
        , learningRate(0.01f)
        , noiseLevel(0.1f) {}
};

NeuromodulationManager::NeuromodulationManager() : pImpl(std::make_unique<Impl>()) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize all neuromodulators
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->reward = std::make_unique<Reward>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    
    pImpl->predictionError->initialize(brain);
    pImpl->novelty->initialize(brain);
    pImpl->curiosity->initialize(brain);
    
    NLM_LOG_INFO("Neuromodulation manager initialized with all systems");
}

void NeuromodulationManager::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update all neuromodulators
    pImpl->dopamine->update(dt);
    pImpl->novelty->update(dt);
    
    // Get current brain state for other neuromodulators
    if (pImpl->brain) {
        auto* spikeSystem = pImpl->brain->getSpikeSystem();
        if (spikeSystem) {
            // Compute novelty from spike patterns
            float noveltyLevel = spikeSystem->getRecentNovelty();
            pImpl->novelty->setLevel(noveltyLevel);
            
            // Compute prediction error
            float predictionError = spikeSystem->getPredictionError();
            pImpl->predictionError->computeError(
                spikeSystem->getPredictedValue(), 
                spikeSystem->getActualValue()
            );
        }
        
        // Update curiosity based on novelty and prediction error
        float noveltyLevel = pImpl->novelty->getLevel();
        float predictionError = pImpl->predictionError->getError();
        pImpl->curiosity->update(noveltyLevel, predictionError, dt);
    }
}

float NeuromodulationManager::getTotalModulation() const {
    float total = 0.0f;
    if (pImpl->dopamine) total += pImpl->dopamine->getPlasticityFactor();
    if (pImpl->novelty) total += pImpl->novelty->getLevel();
    if (pImpl->curiosity) total += pImpl->curiosity->getLevel();
    if (pImpl->predictionError) total += std::abs(pImpl->predictionError->getError());
    return total;
}

float NeuromodulationManager::getExplorationDrive() const {
    if (pImpl->curiosity) {
        return pImpl->curiosity->getExplorationDrive();
    }
    return 0.0f;
}

void NeuromodulationManager::processObservation(const Observation& observation,
                                                 const Observation& previousObservation,
                                                 float predictedValue) {
    // Compute reward from observation
    float rewardValue = pImpl->reward->computeReward(observation);
    pImpl->reward->setValue(rewardValue);
    
    // Signal reward to dopamine
    if (rewardValue > 0.0f) {
        pImpl->dopamine->signalReward(rewardValue);
    }
    
    // Compute prediction error
    float actualValue = extractValueFromObservation(observation);
    pImpl->predictionError->computeError(predictedValue, actualValue);
    
    // Update dopamine with prediction error
    float error = pImpl->predictionError->getError();
    if (std::abs(error) > 0.01f) {
        pImpl->dopamine->signalRewardPredictionError(error);
    }
    
    // Detect novelty
    pImpl->novelty->detectNovelty(observation, previousObservation);
    
    // Update curiosity
    float noveltyLevel = pImpl->novelty->getLevel();
    pImpl->curiosity->update(noveltyLevel, error, 0.001f); // Default timestep
}

void NeuromodulationManager::reset() {
    if (pImpl->dopamine) pImpl->dopamine->setLevel(0.0f);
    if (pImpl->reward) pImpl->reward->reset();
    if (pImpl->predictionError) {
        pImpl->predictionError->clearHistory();
        pImpl->predictionError->updatePrediction(0.0f);
    }
    if (pImpl->novelty) pImpl->novelty->clearHistory();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    
    NLM_LOG_INFO("Neuromodulation systems reset");
}

float NeuromodulationManager::extractValueFromObservation(const Observation& observation) const {
    // Extract a representative value from observation for prediction error computation
    const auto& sensoryData = observation.getSensoryData();
    if (sensoryData.empty()) return 0.0f;
    
    return std::accumulate(sensoryData.begin(), sensoryData.end(), 0.0f) / sensoryData.size();
}

} // namespace nlm