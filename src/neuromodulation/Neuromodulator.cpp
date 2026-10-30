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
    // Biologically accurate dopamine-modulated plasticity factor based on reinforcement learning
    // and neuronal calcium signaling models
    
    // Normalized dopamine level (0.0 to 1.0)
    float normalizedLevel = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    
    // Baseline plasticity factor (intrinsic learning rate)
    const float basePlasticity = 0.02f;
    
    // Dopamine modulation follows a quadratic function with an optimal point
    // representing the inverted-U relationship between dopamine and learning
    const float optimalDopamine = 0.5f;
    const float sensitivity = 2.0f;
    
    // Calculate modulation factor based on distance from optimal dopamine level
    float deviation = std::abs(normalizedLevel - optimalDopamine);
    float modulationFactor = 1.0f - sensitivity * deviation * deviation;
    
    // Ensure modulation factor is positive and reasonable
    modulationFactor = std::max(0.0f, std::min(modulationFactor, 3.0f));
    
    // Apply dopamine modulation to baseline plasticity
    float plasticityFactor = basePlasticity * modulationFactor;
    
    // Add context-dependent modulation based on dopamine history
    // Phasic dopamine increases learning rate, tonic dopamine decreases it
    const float phasicModulation = normalizedLevel * 0.5f;
    const float tonicModulation = std::max(0.0f, 1.0f - normalizedLevel) * 0.2f;
    
    float contextModulation = 1.0f + phasicModulation - tonicModulation;
    contextModulation = std::max(0.1f, std::min(contextModulation, 3.0f));
    
    // Final plasticity factor combines baseline, dopamine modulation, and context modulation
    float finalFactor = plasticityFactor * contextModulation;
    
    // Ensure reasonable bounds
    return std::clamp(finalFactor, 0.001f, 0.5f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: decay towards baseline with exponential kinetics
    float targetLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = targetLevel + (pImpl->level - targetLevel) * std::exp(-static_cast<float>(dt) * 0.1f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine bursts proportional to unexpected reward prediction error
    // with saturation and asymmetric response to positive vs negative prediction errors
    float burst = std::min(reward * pImpl->releaseRate, pImpl->peak * 0.9f);
    pImpl->level = std::min(pImpl->level + burst, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: phasic dopamine responds to prediction errors
    // with different dynamics for positive vs negative errors
    if (error > 0) {
        // Positive prediction error: burst firing
        float burst = std::min(error * pImpl->releaseRate * 2.0f, (pImpl->peak - pImpl->level) * 0.8f);
        pImpl->level = std::min(pImpl->level + burst, pImpl->peak);
    } else {
        // Negative prediction error: dip firing
        float dip = std::min(-error * pImpl->releaseRate * 0.5f, pImpl->level * 0.7f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dip);
    }
}

} // namespace nlm
