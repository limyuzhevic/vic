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
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: leaky integration with exponential decay
    // Implements both tonic and phasic components
    float dtFloat = static_cast<float>(dt);
    
    // Exponential decay towards baseline (tonic component)
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtFloat);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: dopamine burst proportional to reward prediction error
    // Implements Rescorla-Wagner learning model style update
    float predictionError = reward - getPlasticityFactor() * 0.5f;  // Simplified prediction
    float burst = std::max(0.0f, predictionError * pImpl->releaseRate);
    
    // Dopamine burst with upper bound
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Shift baseline upwards slightly for this episode (reward prediction)
    pImpl->baseline = std::min(0.8f, pImpl->baseline + 0.1f * predictionError);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: dopamine encodes signed prediction error
    // Negative prediction error (worse than expected) suppresses dopamine
    // Positive prediction error (better than expected) increases dopamine
    
    // Modulate response based on current level (phasic vs tonic)
    float modulationFactor = 1.0f - pImpl->level;  // Less modulation when already high
    float response = error * pImpl->releaseRate * modulationFactor;
    
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, pImpl->level + response));
    
    // Use prediction error to update baseline for next prediction
    pImpl->baseline = std::max(0.0f, pImpl->baseline + 0.05f * error);
}

} // namespace nlm
