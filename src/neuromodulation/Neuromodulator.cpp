#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float rewardHistory[100];  // Last 100 reward values for trace
    size_t historyIndex;
    size_t historySize;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), historyIndex(0), historySize(0) {
        for (int i = 0; i < 100; ++i) rewardHistory[i] = 0.0f;
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
    // More biologically plausible dopamine modulation:
    // - Low dopamine (0.0-0.3): suppresses plasticity (prevents unwanted learning)
    // - Moderate dopamine (0.3-0.7): enhances plasticity (LTP facilitation)
    // - High dopamine (0.7-1.0): enhances plasticity but favors LTD (prediction error signaling)
    float level = pImpl->level;
    
    if (level < 0.3f) {
        // Dopamine suppression: reduces learning rate
        return 0.1f + 0.2f * (level / 0.3f);  // Range: 0.1 to 0.3
    } else if (level < 0.7f) {
        // Dopamine enhancement: promotes LTP
        return 0.3f + 0.5f * ((level - 0.3f) / 0.4f);  // Range: 0.3 to 0.8
    } else {
        // High dopamine: promotes LTD and prediction error
        return 0.5f + 0.5f * ((level - 0.7f) / 0.3f);  // Range: 0.5 to 1.0
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: integrates reward over time with decay
    // Reward increases dopamine level, effects decay naturally
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.1f);
    
    // Store in reward history for trace computation
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 100;
    if (pImpl->historySize < 100) pImpl->historySize++;
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
