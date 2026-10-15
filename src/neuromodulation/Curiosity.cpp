#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float plasticityCoupling;
    float explorationDecayRate;
    float adaptationTime;
    float maxLevel;
    float optimalExploration;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , plasticityCoupling(0.8f)
        , explorationDecayRate(0.03f)
        , adaptationTime(0.0f)
        , maxLevel(1.0f)
        , optimalExploration(0.5f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized for neural integration");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Compute target curiosity from novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Ensure target is within valid range
    target = std::clamp(target, 0.0f, pImpl->maxLevel);
    
    // Integrate with plasticity mechanisms - neuromodulatory coupling
    float plasticityFactor = pImpl->plasticityCoupling;
    if (pImpl->brain) {
        if (auto* dopamine = pImpl->brain->getDopamine()) {
            plasticityFactor *= dopamine->getPlasticityFactor();
        }
    }
    
    // Smooth update with plasticity-modulated learning rate
    float learningRate = 0.1f * plasticityFactor;
    pImpl->level += (target - pImpl->level) * learningRate * dtFloat;
    
    // Exploration-specific decay with adaptation
    pImpl->adaptationTime += dtFloat;
    
    // Curiosity adapts to environmental statistics
    float environmentalChange = std::abs(target - pImpl->optimalExploration);
    if (environmentalChange > 0.3f) {
        // Environment is changing significantly - increase curiosity
        pImpl->level = std::min(pImpl->maxLevel, pImpl->level * 1.1f);
    }
    
    // Exploration-specific decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->explorationDecayRate * dtFloat);
    
    // Log high curiosity states
    if (pImpl->level > 0.8f) {
        NLM_LOG_DEBUG("High curiosity level: " + std::to_string(pImpl->level));
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::setExplorationDecayRate(float rate) {
    pImpl->explorationDecayRate = rate;
}

void Curiosity::setPlasticityCoupling(float coupling) {
    pImpl->plasticityCoupling = coupling;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->adaptationTime = 0.0f;
}

float Curiosity::getPlasticityModulator() const {
    // Curiosity modulates plasticity to balance exploration vs exploitation
    // Higher curiosity increases plasticity for exploratory learning
    float explorationBias = pImpl->level / pImpl->maxLevel;
    float plasticityModulation = 1.0f + explorationBias * (pImpl->plasticityCoupling - 1.0f);
    return plasticityModulation;
}

void Curiosity::signalNoveltySpike() {
    // Spike-based novelty signaling for burst firing
    pImpl->level = std::min(pImpl->maxLevel, pImpl->level * 1.2f);
}

void Curiosity::signalPredictionErrorSpike() {
    // Spike-based prediction error signaling for learning bursts
    pImpl->level = std::min(pImpl->maxLevel, pImpl->level * 1.5f);
}

} // namespace nlm
