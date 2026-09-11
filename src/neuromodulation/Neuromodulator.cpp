#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationFactor;  // For metaplastic adaptation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), adaptationFactor(1.0f) {}
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
    // Dynamic plasticity factor based on dopamine level with saturating nonlinearity
    // Implements D1/D2 receptor balance and metaplasticity
    float d1Factor = std::min(pImpl->level, 0.8f) / 0.8f;  // D1 receptor activation (0-1)
    float d2Factor = std::min(pImpl->level, 0.6f) / 0.6f;  // D2 receptor activation (0-1)
    float metaplasticFactor = 1.0f - 0.2f * std::sin(pImpl->level * 3.14159f);  // Metaplastic modulation
    
    // Compute overall plasticity factor: D1 enhances, D2 suppresses
    float plasticityFactor = 0.5f + (d1Factor * 0.5f) - (d2Factor * 0.3f) + (metaplasticFactor * 0.2f);
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // Add adaptation based on recent reward history
    plasticityFactor *= pImpl->adaptationFactor;
    
    return plasticityFactor;
}
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
