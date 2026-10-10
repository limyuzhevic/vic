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
    
    // Additional DA effects
    float attentionModulation;
    float workingMemoryModulation;
    float explorationModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , attentionModulation(1.0f), workingMemoryModulation(1.0f), explorationModulation(0.0f) {}
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
    
    // Update downstream effects based on dopamine level
    // Moderate DA (0.3-0.7) enhances attention and working memory
    // High DA (>0.8) promotes exploration and novelty seeking
    if (pImpl->level >= 0.3f && pImpl->level <= 0.7f) {
        pImpl->attentionModulation = 1.0f + (pImpl->level - 0.5f) * 0.5f;  // Peak attention at moderate DA
        pImpl->workingMemoryModulation = 1.0f + (pImpl->level - 0.5f) * 0.3f;  // Working memory enhancement
        pImpl->explorationModulation = 0.0f;  // Focused, not exploratory
    } else if (pImpl->level > 0.8f) {
        pImpl->attentionModulation = 0.5f;  // Less focused, more diffuse
        pImpl->workingMemoryModulation = 0.8f;  // Potential working memory overload
        pImpl->explorationModulation = 1.0f;  // High exploration tendency
    } else {
        pImpl->attentionModulation = 0.7f + pImpl->level * 0.6f;  // Low attention at low DA
        pImpl->workingMemoryModulation = 0.8f + pImpl->level * 0.4f;  // Baseline working memory
        pImpl->explorationModulation = 0.0f;  // Little exploration at low DA
    }
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
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

float Dopamine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Dopamine::getWorkingMemoryModulation() const {
    return pImpl->workingMemoryModulation;
}

float Dopamine::getExplorationModulation() const {
    return pImpl->explorationModulation;
}

} // namespace nlm
