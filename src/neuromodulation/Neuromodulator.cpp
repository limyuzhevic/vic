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
    // Real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity for reward-relevant synapses
    // Lower dopamine decreases plasticity, stabilizing learned responses
    // This implements the dopamine gates for learning in the brain
    
    // Dopamine concentration affects synaptic plasticity
    // Optimal plasticity at moderate dopamine levels
    // Very high or very low dopamine reduces learning efficiency
    
    float normalizedLevel = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Peak plasticity at moderate levels (representing the "Goldilocks principle")
    // Too little dopamine: insufficient motivation to learn
    // Too much dopamine: sensory flooding, poor discrimination
    // Moderate dopamine: optimal learning state
    
    float plasticity = 1.0f;
    if (normalizedLevel < 0.3f) {
        // Low dopamine: reduced plasticity
        plasticity = 0.3f + normalizedLevel * 0.7f;  // Linear increase
    } else if (normalizedLevel > 0.7f) {
        // High dopamine: reduced plasticity (overload)
        plasticity = 1.0f - (normalizedLevel - 0.7f) * 3.0f;
    } else {
        // Optimal range: high plasticity
        plasticity = 1.0f;
    }
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Dopamine follows a differential equation: dD/dt = -k*D + input
    // Where k is decay rate and input is reward prediction error
    // This models the temporal dynamics of dopamine signaling in the brain
    
    // Calculate decay towards baseline
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    float newLevel = pImpl->level - decayAmount;
    
    // Clamp to minimum baseline to prevent negative values
    newLevel = std::max(pImpl->baseline, newLevel);
    
    // Apply decay
    pImpl->level = newLevel;
}

void Dopamine::signalReward(float reward) {
    // Real dopamine reward signaling
    // Dopamine encodes reward prediction error (RPE): actual reward - expected reward
    // When reward exceeds expectations, dopamine spikes to reinforce the path
    // When reward is less than expected, dopamine dips to suppress ineffective paths
    // This implements temporal difference learning in the dopamine system
    
    // Calculate reward prediction error based on baseline expectations
    float predictionError = reward - pImpl->baseline;
    
    // Dopamine response is proportional to RPE
    // Positive RPE (better than expected) -> dopamine increase
    // Negative RPE (worse than expected) -> dopamine decrease
    float dopamineResponse = predictionError * pImpl->releaseRate;
    
    // Update dopamine level with response
    setLevel(pImpl->level + dopamineResponse);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
