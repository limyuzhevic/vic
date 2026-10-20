// Curiosity implementation
#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float explorationBuffer;
    float noveltyBuffer;
    float predictionErrorBuffer;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f),
             explorationBuffer(0.0f), noveltyBuffer(0.0f), predictionErrorBuffer(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}
Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->plasticityFactor = 1.0f + pImpl->level * 1.2f;  // Range: 1.0 to 2.2
}

float Curiosity::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Curiosity::update(TimestepDuration dt) {
    // Decay curiosity level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
    
    // Process exploration signals
    if (pImpl->explorationBuffer > 0.001f) {
        pImpl->level += pImpl->explorationBuffer * 0.15f;
        pImpl->explorationBuffer = 0.0f;
    }
    
    // Process novelty signals
    if (pImpl->noveltyBuffer > 0.001f) {
        pImpl->level += pImpl->noveltyBuffer * 0.1f;
        pImpl->noveltyBuffer = 0.0f;
    }
    
    // Process prediction error signals
    if (std::abs(pImpl->predictionErrorBuffer) > 0.001f) {
        pImpl->level += std::abs(pImpl->predictionErrorBuffer) * 0.05f;
        pImpl->predictionErrorBuffer = 0.0f;
    }
    
    // Clamp level
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Curiosity::increaseExploration(Brain* brain) {
    if (!brain) return;
    
    // Drive exploration behavior
    float explorationEffect = pImpl->level * 0.8f;
    
    // Apply to neural systems that control action selection
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Boost exploratory activity
                neuron->injectCurrent(explorationEffect * 0.6f);
                
                // Enhance plasticity for exploratory learning
                auto& flags = neuron->getPlasticityFlags();
                flags.hebbian = true;
                flags.stdp = true;
            }
        }
    }
}

void Curiosity::reduceExploration(Brain* brain) {
    if (!brain) return;
    
    // Reduce exploration and promote exploitation
    float stabilityEffect = pImpl->level * 0.4f;
    
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Apply stability modulation
                neuron->injectCurrent(-stabilityEffect * 0.3f);
            }
        }
    }
}

float Curiosity::computeCuriosityDrive(float novelty, float predictionError) {
    // Combine novelty and prediction error to drive curiosity
    float curiosity = (novelty * 0.6f) + (std::abs(predictionError) * 0.4f);
    return std::min(1.0f, curiosity);
}

} // namespace nlm