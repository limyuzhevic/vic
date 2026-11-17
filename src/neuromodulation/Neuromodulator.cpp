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
    float predictionError;
    float rewardHistory[5];  // Last 5 reward values for prediction
    int historyIndex;
    float tonicLevel;
    float phasicComponent;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.05f), releaseRate(1.0f),
             predictionError(0.0f), historyIndex(0), tonicLevel(0.1f), phasicComponent(0.0f) {
        for (int i = 0; i < 5; ++i) rewardHistory[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components
    return pImpl->tonicLevel + pImpl->phasicComponent;
}

void Dopamine::setLevel(float level) {
    pImpl->tonicLevel = std::clamp(level * 0.5f, 0.0f, 1.0f);
    pImpl->phasicComponent = 0.0f;  // Phasic component reset
}

float Dopamine::getPlasticityFactor() const {
    // Realistic dopamine-modulated plasticity factor
    // Based on neuroscience research: moderate dopamine levels optimize learning
    float daLevel = getLevel();
    
    if (daLevel < 0.2f) {
        // Low dopamine: reduced plasticity
        return 0.3f + 0.3f * daLevel;
    } else if (daLevel < 0.6f) {
        // Optimal range: increased plasticity for learning
        return 0.6f + 0.4f * (daLevel - 0.2f) / 0.4f;
    } else {
        // High dopamine: stabilized plasticity to prevent overwriting
        return 1.0f - 0.3f * (daLevel - 0.6f) / 0.4f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics
    float dtFloat = static_cast<float>(dt);
    
    // Tonic component decays towards baseline
    pImpl->tonicLevel = std::max(pImpl->baseline, pImpl->tonicLevel - pImpl->decayRate * dtFloat);
    
    // Phasic component decays exponentially
    pImpl->phasicComponent *= std::exp(-pImpl->decayRate * dtFloat);
    
    // Update prediction error-based prediction
    pImpl->predictionError *= std::exp(-0.02f * dtFloat);  // Slower decay of prediction error
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling
    // Update reward prediction history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    
    // Calculate reward prediction error
    float expectedReward = 0.0f;
    for (int i = 0; i < 5; ++i) {
        expectedReward += pImpl->rewardHistory[i];
    }
    expectedReward /= 5.0f;
    
    pImpl->predictionError = reward - expectedReward;
    
    // Phasic response: stronger for unexpected rewards
    float phasicResponse = 0.0f;
    if (std::abs(pImpl->predictionError) > 0.01f) {
        // Scale phasic response with prediction error magnitude
        phasicResponse = pImpl->predictionError * pImpl->releaseRate * 0.5f;
    }
    
    // Add sustained component for actual reward value
    float sustainedResponse = std::min(0.5f, reward * 0.3f);
    
    // Update total dopamine level
    pImpl->phasicComponent += phasicResponse;
    pImpl->tonicLevel += sustainedResponse * 0.1f;  // Slower tonic increase
    
    // Clamp levels
    pImpl->phasicComponent = std::clamp(pImpl->phasicComponent, -0.5f, 1.5f);
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct prediction error signaling (for TD learning)
    pImpl->predictionError = error;
    
    // Phasic dopamine response to prediction error
    pImpl->phasicComponent += error * pImpl->releaseRate;
    
    // Tonic adjustment based on sustained prediction error
    if (std::abs(error) > 0.05f) {
        // Large prediction errors adjust tonic baseline
        pImpl->baseline += error * 0.02f;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 0.5f);
    }
    
    // Clamp phasic component
    pImpl->phasicComponent = std::clamp(pImpl->phasicComponent, -0.5f, 1.5f);
}

} // namespace nlm
