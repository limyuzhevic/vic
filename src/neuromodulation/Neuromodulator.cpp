#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PHASE 2: Implement sigmoidal plasticity modulation based on dopamine level
    // Low dopamine = low plasticity, high dopamine = high plasticity
    float normalizedLevel = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    
    // Sigmoidal relationship: plasticity increases with dopamine but saturates
    float plasticity = 1.0f / (1.0f + std::exp(-10.0f * (normalizedLevel - 0.5f)));
    
    // Scale to reasonable range [0.1, 1.0]
    return 0.9f * plasticity + 0.1f;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PHASE 2: Implement more realistic dopamine dynamics with time-dependent recovery
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small spontaneous fluctuations for biological realism
    pImpl->level += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.01f * decayAmount;
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PHASE 2: Implement robust reward signaling with temporal dynamics
    if (reward > 0.0f) {
        // Reward prediction error signal
        float rewardGain = 2.0f; // Reward sensitivity
        float burstAmount = reward * rewardGain;
        
        // Add to current level, but cap at peak
        pImpl->level += burstAmount;
        
        // Limit maximum level
        if (pImpl->level > pImpl->peak) {
            pImpl->level = pImpl->peak;
        }
        
        // Add some physiological noise
        pImpl->level += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PHASE 2: Implement reward prediction error with Phasic response
    if (std::abs(error) > 0.01f) { // Only respond to meaningful errors
        // Phasic dopamine response proportional to error magnitude
        float errorGain = 1.5f; // Error sensitivity
        float phasicResponse = error * errorGain;
        
        // For positive errors (better than expected), increase dopamine
        if (error > 0.0f) {
            pImpl->level += phasicResponse;
            pImpl->level = std::min(pImpl->level, pImpl->peak);
        } else {
            // For negative errors (worse than expected), decrease dopamine
            pImpl->level += phasicResponse; // phasicResponse is negative
            pImpl->level = std::max(pImpl->level, pImpl->baseline);
        }
        
        // Add some physiological noise
        pImpl->level += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.03f;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    }
}

} // namespace nlm
