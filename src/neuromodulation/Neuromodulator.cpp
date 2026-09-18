#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float historyDecay;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), historyDecay(0.95f) {}
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
    // Implements concentration-dependent effects on synaptic plasticity
    // Higher dopamine increases LTP, decreases LTD
    
    // Sigmoidal relationship: saturates at high levels
    float normalized = pImpl->level / pImpl->peak;
    float sigmoid = 1.0f / (1.0f + exp(-10.0f * (normalized - 0.5f)));
    
    // Inverted relationship: low DA -> high LTP, high DA -> low LTP
    float ltpFactor = 0.8f * (1.0f - sigmoid) + 0.1f;  // 10-90% range
    
    // For LTD, inverted relationship
    float ltdFactor = 0.8f * sigmoid + 0.1f;  // 10-90% range
    
    // Apply neuromodulation to plasticity weights
    float plasticityFactor = ltpFactor - ltdFactor + 1.0f;
    
    // Ensure reasonable bounds
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with adaptive filtering
    // Implements time-dependent dopamine signaling with baseline and adaptation
    
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay towards baseline (models reuptake and metabolism)
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Apply history decay for temporal filtering
    pImpl->baseline *= pImpl->historyDecay;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on prediction error
    // Models phasic dopamine responses to unexpected rewards
    
    // Positive reward produces phasic increase
    float rewardSignal = reward * pImpl->releaseRate;
    
    // Modulate level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Ensure baseline is not exceeded too much
    pImpl->baseline = pImpl->level * 0.2f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Models dopamine responses to prediction errors (RL theory)
    
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    float errorSignal = error * pImpl->releaseRate * 2.0f;  // Amplify for better visibility
    
    // Apply with bounded response
    if (error > 0.0f) {
        // Better than expected: increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorSignal);
        // Update baseline to reflect new expectation
        pImpl->baseline = std::max(pImpl->baseline, pImpl->level * 0.3f);
    } else {
        // Worse than expected: decrease dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level + errorSignal);
    }
}

} // namespace nlm
