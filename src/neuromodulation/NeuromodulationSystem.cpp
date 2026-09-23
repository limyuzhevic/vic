#include "neuromodulation/NeuromodulationSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>

namespace nlm {

struct NeuromodulationSystem::Impl {
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    Brain* brain;
    
    // Configuration
    float rewardSensitivity;
    float explorationRate;
    float noveltyThreshold;
    float predictionLearningRate;
    
    Impl()
        : dopamine(nullptr)
        , curiosity(nullptr)
        , novelty(nullptr)
        , predictionError(nullptr)
        , brain(nullptr)
        , rewardSensitivity(1.0f)
        , explorationRate(0.5f)
        , noveltyThreshold(0.3f)
        , predictionLearningRate(0.01f) {}
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(std::make_unique<Impl>()) {}
NeuromodulationSystem::~NeuromodulationSystem() = default;

NeuromodulationSystem::NeuromodulationSystem(NeuromodulationSystem&& other) noexcept = default;
NeuromodulationSystem& NeuromodulationSystem::operator=(NeuromodulationSystem&& other) noexcept = default;

void NeuromodulationSystem::initialize(Brain* b) {
    brain = b;
    
    // Create neuromodulators
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    novelty = std::make_unique<Novelty>();
    predictionError = std::make_unique<PredictionError>();
    
    // Initialize with brain reference
    dopamine->initialize(b);
    curiosity->initialize(b);
    novelty->initialize(b);
    predictionError->initialize(b);
    
    // Apply configuration
    setRewardSensitivity(pImpl->rewardSensitivity);
    setExplorationRate(pImpl->explorationRate);
    setNoveltyThreshold(pImpl->noveltyThreshold);
    setPredictionLearningRate(pImpl->predictionLearningRate);
}

Dopamine* NeuromodulationSystem::getDopamine() {
    return dopamine.get();
}

const Dopamine* NeuromodulationSystem::getDopamine() const {
    return dopamine.get();
}

Curiosity* NeuromodulationSystem::getCuriosity() {
    return curiosity.get();
}

const Curiosity* NeuromodulationSystem::getCuriosity() const {
    return curiosity.get();
}

Novelty* NeuromodulationSystem::getNovelty() {
    return novelty.get();
}

const Novelty* NeuromodulationSystem::getNovelty() const {
    return novelty.get();
}

PredictionError* NeuromodulationSystem::getPredictionError() {
    return predictionError.get();
}

const PredictionError* NeuromodulationSystem::getPredictionError() const {
    return predictionError.get();
}

void NeuromodulationSystem::updateNeuromodulators(float timestep) {
    if (dopamine) dopamine->update(timestep);
    if (curiosity) curiosity->update(timestep);
    if (novelty) novelty->update(timestep);
    if (predictionError) predictionError->update(timestep);
}

void NeuromodulationSystem::applyNeuromodulationToBrain() {
    if (!brain) return;
    
    // Apply dopamine effects on neural excitability
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopamineLevel * 0.5f;
        
        // Apply to all neurons in brain
        for (const auto& region : brain->getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void NeuromodulationSystem::emitReward(float reward, float predictionError) {
    if (dopamine) {
        // Reward signal based on prediction error
        float rewardSignal = reward * (1.0f + predictionError);
        dopamine->receiveRewardSignal(rewardSignal);
    }
}

void NeuromodulationSystem::detectNovelty(const class SensoryInput& input) {
    if (novelty) {
        // Novelty detection based on input changes
        float noveltyLevel = novelty->detectNovelty(input);
        if (curiosity) {
            curiosity->updateNoveltyDrivenMotivation(noveltyLevel);
        }
    }
}

void NeuromodulationSystem::updateCuriosity(float explorationValue) {
    if (curiosity) {
        curiosity->updateExplorationDrive(explorationValue);
    }
}

void NeuromodulationSystem::processPredictionError(float predictedValue, float actualValue) {
    if (predictionError) {
        predictionError->updatePredictionError(predictedValue, actualValue);
    }
    
    // Emit reward signal based on prediction error
    emitReward(std::abs(actualValue - predictedValue), actualValue - predictedValue);
}

float NeuromodulationSystem::getDopamineLevel() const {
    return dopamine ? dopamine->getLevel() : 0.0f;
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return curiosity ? curiosity->getCuriosityLevel() : 0.0f;
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return novelty ? novelty->getNoveltyLevel() : 0.0f;
}

float NeuromodulationSystem::getPlasticityModulator() const {
    return dopamine ? dopamine->getPlasticityFactor() : 1.0f;
}

void NeuromodulationSystem::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = sensitivity;
    if (dopamine) {
        dopamine->setRewardSensitivity(sensitivity);
    }
}

void NeuromodulationSystem::setExplorationRate(float rate) {
    pImpl->explorationRate = rate;
    if (curiosity) {
        curiosity->setExplorationRate(rate);
    }
}

void NeuromodulationSystem::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
    if (novelty) {
        novelty->setNoveltyThreshold(threshold);
    }
}

void NeuromodulationSystem::setPredictionLearningRate(float rate) {
    pImpl->predictionLearningRate = rate;
    if (predictionError) {
        predictionError->setLearningRate(rate);
    }
}

} // namespace nlm