#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicBaseline;
    float phasicAmplitude;
    float predictionErrorHistory[5];  // For temporal difference learning
    size_t predictionHistoryIndex;
    float learningRate;  // For reward prediction error computation
    float eligibilityTrace;  // For reward-modulated plasticity
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             tonicBaseline(0.2f), phasicAmplitude(0.5f), predictionHistoryIndex(0),
             learningRate(0.1f), eligibilityTrace(0.0f) {
        // Initialize prediction error history
        for (size_t i = 0; i < 5; ++i) {
            predictionErrorHistory[i] = 0.0f;
        }
    }
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
    // PHASE 2: Real dopamine-modulated plasticity factor
    // Implement phasic and tonic dopamine effects on plasticity
    
    float tonicFactor = pImpl->level * pImpl->tonicBaseline;
    float phasicFactor = pImpl->phasicAmplitude * std::tanh(pImpl->level);
    
    // Combine tonic and phasic components
    float plasticityModulation = tonicFactor + phasicFactor;
    
    // Apply sigmoidal transformation for non-linear effects
    return 1.0f / (1.0f + std::exp(-10.0f * (plasticityModulation - 0.5f)));
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2: Real dopamine dynamics
    // Implement temporal dynamics with phasic responses to prediction errors
    
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    
    // Decay toward tonic baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Update eligibility trace for reward-modulated plasticity
    if (pImpl->eligibilityTrace > 0.001f) {
        pImpl->eligibilityTrace *= (1.0f - 0.01f * static_cast<float>(dt));  // Slow decay
    }
}

void Dopamine::signalReward(float reward) {
    // PHASE 2: Real reward signaling with prediction error computation
    // Implement reward prediction error as difference between received reward and expected reward
    
    // Compute expected reward (average of recent prediction errors)
    float expectedReward = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        expectedReward += pImpl->predictionErrorHistory[i];
    }
    expectedReward /= 5.0f;
    
    // Compute reward prediction error (RPE): reward - expected reward
    float rpe = reward - expectedReward;
    
    // Update prediction error history (circular buffer)
    pImpl->predictionErrorHistory[pImpl->predictionHistoryIndex] = rpe;
    pImpl->predictionHistoryIndex = (pImpl->predictionHistoryIndex + 1) % 5;
    
    // Phasic dopamine response to prediction error
    float phasicResponse = pImpl->phasicAmplitude * std::tanh(rpe * pImpl->learningRate);
    
    // Update dopamine level with both tonic and phasic components
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicResponse);
    
    // Update eligibility trace for reward-modulated plasticity
    // Eligibility trace accumulates weight changes when dopamine is present
    pImpl->eligibilityTrace += std::max(0.0f, rpe) * 0.1f;  // Only positive RPE affects eligibility
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2: Reward prediction error signaling
    // Implement temporal difference learning and error propagation
    
    // Update prediction error history
    pImpl->predictionErrorHistory[pImpl->predictionHistoryIndex] = error;
    pImpl->predictionHistoryIndex = (pImpl->predictionHistoryIndex + 1) % 5;
    
    // Direct dopamine response to prediction error
    float directResponse = pImpl->phasicAmplitude * std::tanh(error * pImpl->learningRate);
    
    // Update dopamine level
    pImpl->level = std::max(0.0f, pImpl->level + directResponse);
    
    // Update eligibility trace based on prediction error
    pImpl->eligibilityTrace += error * 0.05f;
}

// Get eligibility trace for reward-modulated plasticity
float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

// Reset eligibility trace
void Dopamine::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

} // namespace nlm
