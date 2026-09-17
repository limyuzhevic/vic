#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Dopamine implementation

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             plasticityModulation(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Positive dopamine enhances LTP, inhibits LTD
    // Negative dopamine enhances LTD, inhibits LTP
    // Complex interaction with prediction error
    float plasticityMod = 1.0f;
    
    if (pImpl->level > 0.5f) {
        // High dopamine: enhance LTP, reduce LTD
        plasticityMod = 1.0f + (pImpl->level - 0.5f) * 1.5f;
    } else {
        // Low dopamine: enhance LTD, reduce LTP
        plasticityMod = 1.0f - (0.5f - pImpl->level) * 1.5f;
    }
    
    // Ensure plasticity stays within reasonable bounds
    return std::clamp(plasticityMod, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Decay towards baseline, modulated by prediction error
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay plasticity modulation
    pImpl->plasticityModulation = pImpl->plasticityModulation * 0.95f + 1.0f * 0.05f;
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error signaling
    // Phasic burst on unexpected reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Phasic dip on unexpected punishment, burst on reward prediction
    pImpl->level = std::clamp(pImpl->level + error * pImpl->releaseRate, 0.0f, pImpl->peak);
    
    // Update plasticity modulation based on prediction error
    if (std::abs(error) > 0.1f) {
        pImpl->plasticityModulation += error * 0.5f;
        pImpl->plasticityModulation = std::clamp(pImpl->plasticityModulation, 0.1f, 3.0f);
    }
}
