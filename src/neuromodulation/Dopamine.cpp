// Neuromodulator base implementation
#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Neuromodulator::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

// Dopamine implementation
#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float rewardBuffer;
    float predictionErrorBuffer;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f),
             rewardBuffer(0.0f), predictionErrorBuffer(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update plasticity factor based on level
    pImpl->plasticityFactor = 0.5f + pImpl->level * 0.5f;  // Range: 0.5 to 1.0
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
    
    // Process reward signals
    if (pImpl->rewardBuffer > 0.001f) {
        pImpl->level += pImpl->rewardBuffer * 0.1f;
        pImpl->rewardBuffer = 0.0f;
    }
    
    // Process prediction error signals
    if (std::abs(pImpl->predictionErrorBuffer) > 0.001f) {
        float errorSign = (pImpl->predictionErrorBuffer > 0.0f) ? 1.0f : -1.0f;
        pImpl->level += errorSign * std::abs(pImpl->predictionErrorBuffer) * 0.05f;
        pImpl->predictionErrorBuffer = 0.0f;
    }
    
    // Clamp level
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardBuffer += reward;
    NLM_LOG_DEBUG(std::string("Dopamine reward signal: ") + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorBuffer += error;
    NLM_LOG_DEBUG(std::string("Dopamine prediction error signal: ") + std::to_string(error));
}

void Dopamine::applyEffects(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Apply dopamine effects to neural excitability
    float effect = pImpl->level * 0.5f;
    
    // Modulate firing thresholds - lower threshold = higher excitability
    float thresholdMod = -effect * 2.0f;  // Can be negative (reduce threshold)
    
    // Apply to all neurons in brain
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Apply excitability modulation
                float currentMod = effect * 0.5f;
                neuron->injectCurrent(currentMod);
                
                // Modulate synaptic plasticity
                auto& flags = neuron->getPlasticityFlags();
                flags.hebbian = flags.hebbian || (pImpl->level > 0.3f);
                flags.stdp = flags.stdp || (pImpl->level > 0.3f);
            }
        }
    }
}

} // namespace nlm