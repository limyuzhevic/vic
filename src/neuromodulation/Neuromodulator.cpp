#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float expectedReward;
    float timeConstant;
    float eligibilityTrace;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError(0.0f), expectedReward(0.0f), timeConstant(1.0f), eligibilityTrace(0.0f) {}
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
    // Real dopamine-modulated plasticity factor based on FDA model
    // Higher dopamine increases plasticity, but too high can lead to instability
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Phasic dopamine (prediction error) increases learning rate
    float phasicComponent = 0.4f * normalizedLevel;
    
    // Tonic dopamine (baseline) maintains stability
    float tonicComponent = 0.2f * (1.0f - normalizedLevel) + 0.1f;
    
    // Neuromodulatory gain control
    float plasticityFactor = 0.3f + phasicComponent + tonicComponent;
    
    // Bounded to reasonable range
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline with sigmoidal dynamics
    // Dopamine naturally decays but baseline prevents going to zero
    float decay = pImpl->decayRate * timeStep;
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-decay);
    
    // Eligibility trace decay
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - decay * 0.5f);
}

void Dopamine::signalReward(float reward) {
    // Reward signaling: burst of dopamine proportional to reward magnitude
    // Limited by peak level and incorporates prediction error
    
    float rewardSignal = reward * pImpl->releaseRate;
    
    // Saturating activation function to prevent over-excitation
    rewardSignal = std::tanh(rewardSignal) * pImpl->peak;
    
    // Update level with temporal integration
    pImpl->level = pImpl->level + rewardSignal * 0.1f;
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Record prediction error
    pImpl->predictionError = reward - pImpl->expectedReward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (dopamine's primary function)
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    
    // Prediction error has asymmetric effects
    if (error > 0) {
        // Positive prediction error: burst of dopamine
        float burst = error * pImpl->releaseRate * 2.0f;  // Amplify for positive error
        pImpl->level += burst;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
    } else {
        // Negative prediction error: suppression of dopamine
        float suppression = std::abs(error) * pImpl->decayRate * 1.5f;
        pImpl->level -= suppression;
        pImpl->level = std::max(pImpl->baseline, pImpl->level);
    }
    
    // Update expected reward (moving average)
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * pImpl->level;
    
    // Store eligibility for plasticity
    pImpl->eligibilityTrace += std::abs(error) * 0.5f;
}

} // namespace nlm
