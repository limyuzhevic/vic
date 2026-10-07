#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Reward.hpp"
#include <algorithm>

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain_;
    bool enabled_;
    
    // Statistics
    float totalSteps_;
    float plasticityUpdateCount_;
    float stateUpdateCount_;
    
    // Neuromodulation parameters
    float globalModulation_;
    float rewardInfluence_;
    float curiosityInfluence_;
    float noveltyInfluence_;
    
    Impl() 
        : brain_(nullptr)
        , enabled_(true)
        , totalSteps_(0)
        , plasticityUpdateCount_(0)
        , stateUpdateCount_(0)
        , globalModulation_(1.0f)
        , rewardInfluence_(1.0f)
        , curiosityInfluence_(0.8f)
        , noveltyInfluence_(0.7f) {}
};

NeuromodulationManager::NeuromodulationManager() : pImpl(new Impl) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    NLM_LOG_INFO("NeuromodulationManager initialized with biological integration");
}

void NeuromodulationManager::updateNeuromodulators(TimestepDuration dt) {
    if (!pImpl->brain_ || !pImpl->enabled_) return;
    
    // Get prediction system error for neuromodulation
    float predictionError = 0.0f;
    if (pImpl->brain_->getPredictionSystem()) {
        predictionError = pImpl->brain_->getPredictionSystem()->getPredictionError();
    }
    
    // Update novelty detection
    if (pImpl->brain_->getNovelty()) {
        pImpl->brain_->getNovelty()->update(dt);
    }
    
    // Update curiosity system with prediction error
    if (pImpl->brain_->getCuriosity()) {
        pImpl->brain_->getCuriosity()->update(predictionError, dt);
    }
    
    // Update prediction error signal
    if (pImpl->brain_->getPredictionErrorSignal()) {
        pImpl->brain_->getPredictionErrorSignal()->update(dt);
    }
    
    // Update dopamine for reward prediction error
    if (pImpl->brain_->getDopamine()) {
        // Calculate reward prediction error from prediction error
        float rewardPredictionError = predictionError * 0.5f;  // Simplified RPE calculation
        pImpl->brain_->getDopamine()->update(predictionError, rewardPredictionError, dt);
    }
    
    pImpl->stateUpdateCount_++;
    pImpl->totalSteps_++;
}

void NeuromodulationManager::applyNeuromodulationEffects() {
    if (!pImpl->brain_ || !pImpl->enabled_) return;
    
    // Apply dopamine effects on neural excitability
    if (pImpl->brain_->getDopamine()) {
        applyDopamineExcitabilityEffects();
    }
    
    // Apply other neuromodulator effects (curiosity, novelty, prediction error)
    applyCuriosityEffects();
    applyNoveltyEffects();
    applyPredictionErrorEffects();
}

void NeuromodulationManager::applyDopamineExcitabilityEffects() {
    if (!pImpl->brain_->getDopamine()) return;
    
    float dopamineLevel = pImpl->brain_->getDopamine()->getLevel();
    
    for (auto& region : pImpl->brain_->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Dopamine modulates excitability by injecting additional current
                // Positive dopamine increases excitability (lower effective threshold)
                float excitabilityMod = dopamineLevel * 0.5f * pImpl->globalModulation_;
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void NeuromodulationManager::applyCuriosityEffects() {
    if (!pImpl->brain_->getCuriosity()) return;
    
    float curiosityLevel = pImpl->brain_->getCuriosity()->getLevel();
    // Enhanced curiosity boosts exploration and prediction system engagement
    if (curiosityLevel > 0.5f) {
        // Boost prediction system activity
        if (pImpl->brain_->getPredictionSystem()) {
            auto* predSys = pImpl->brain_->getPredictionSystem();
            // Increase prediction confidence for exploration
            // This would be implemented in PredictionSystem if needed
        }
    }
}

void NeuromodulationManager::applyNoveltyEffects() {
    if (!pImpl->brain_->getNovelty()) return;
    
    float noveltyLevel = pImpl->brain_->getNovelty()->getLevel();
    // Novelty detection enhances prediction error signaling
    if (noveltyLevel > 0.5f) {
        // This would enhance prediction error computation
        // in a real implementation, this would affect PredictionErrorSignal
    }
}

void NeuromodulationManager::applyPredictionErrorEffects() {
    if (!pImpl->brain_->getPredictionErrorSignal()) return;
    
    float predictionError = pImpl->brain_->getPredictionErrorSignal()->getLevel();
    // High prediction error drives learning and exploration
    if (predictionError > 0.3f) {
        // Strong prediction error should enhance synaptic plasticity
        // This is implemented in the brain's plasticity management
    }
}

void NeuromodulationManager::applyPlasticityFactors() {
    if (!pImpl->brain_ || !pImpl->brain_->getDopamine()) return;
    
    // Calculate plasticity modulation factor
    float plasticityMod = pImpl->brain_->getDopamine()->getPlasticityFactor();
    
    // Apply plasticity rules to all regions
    pImpl->brain_->getPlasticityManager()->applyToAllRegions(
        pImpl->brain_->getRegions(), 
        pImpl->brain_->getTimestep(), 
        plasticityMod * pImpl->globalModulation_, 
        pImpl->brain_->getCurrentStep()
    );
    
    // Update statistics
    pImpl->plasticityUpdateCount_++;
}

void NeuromodulationManager::processReward(float reward) {
    if (!pImpl->brain_ || !pImpl->enabled_) return;
    
    // Update reward system
    if (pImpl->brain_->getDopamine()) {
        pImpl->brain_->getDopamine()->signalReward(reward);
    }
    
    // Apply reward influence to global modulation
    pImpl->globalModulation_ = std::min(2.0f, pImpl->globalModulation_ + reward * pImpl->rewardInfluence_ * 0.1f);
    
    NLM_LOG_DEBUG("NeuromodulationManager processed reward: " + std::to_string(reward));
}

void NeuromodulationManager::processPredictionError(float error) {
    if (!pImpl->brain_ || !pImpl->enabled_) return;
    
    // Update prediction error signal
    if (pImpl->brain_->getPredictionErrorSignal()) {
        pImpl->brain_->getPredictionErrorSignal()->computeError(
            pImpl->brain_->getPredictionErrorSignal()->getPrediction(), error);
    }
    
    NLM_LOG_DEBUG("NeuromodulationManager processed prediction error: " + std::to_string(error));
}

void NeuromodulationManager::reset() {
    pImpl->globalModulation_ = 1.0f;
    NLM_LOG_INFO("NeuromodulationManager reset");
}

NeuromodulationManager::NeuromodulationStats NeuromodulationManager::getStats() const {
    NeuromodulationStats stats;
    stats.dopamineLevel = pImpl->brain_ ? pImpl->brain_->getDopamine()->getLevel() : 0.0f;
    stats.curiosityLevel = pImpl->brain_ ? pImpl->brain_->getCuriosity()->getLevel() : 0.0f;
    stats.noveltyLevel = pImpl->brain_ ? pImpl->brain_->getNovelty()->getLevel() : 0.0f;
    stats.predictionErrorLevel = pImpl->brain_ ? pImpl->brain_->getPredictionErrorSignal()->getLevel() : 0.0f;
    stats.totalPlasticityUpdates = pImpl->plasticityUpdateCount_;
    stats.totalStateUpdates = pImpl->stateUpdateCount_;
    stats.totalSteps = pImpl->totalSteps_;
    stats.meanPlasticityFactor = 1.0f;
    stats.meanLearningRate = 0.01f;
    return stats;
}

void NeuromodulationManager::enableNeuromodulation(bool enable) {
    pImpl->enabled_ = enable;
    NLM_LOG_INFO("NeuromodulationManager " + std::string(enable ? "enabled" : "disabled"));
}

bool NeuromodulationManager::isNeuromodulationEnabled() const {
    return pImpl->enabled_;
}

} // namespace nlm
