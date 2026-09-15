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
    // Enhanced implementation with phase-dependent modulation
    // Dopamine increases plasticity for reward prediction errors
    float modulation = 1.0f;
    if (pImpl->level > 0.5f) {
        modulation = 1.0f + (pImpl->level - 0.5f) * 1.5f; // Up to 2.5x modulation
    } else {
        modulation = 1.0f - pImpl->level * 0.5f; // Down to 0.5x modulation
    }
    
    // Additional modulation from recent reward bursts
    if (pImpl->level > 0.8f) {
        modulation *= 1.5f;
    }
    
    return modulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with time constants
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Faster decay at high levels (haste to return to baseline)
    if (pImpl->level > 0.7f) {
        decayAmount *= 1.5f;
    }
    
    // Phasic dips for prediction errors
    if (pImpl->level > 0.3f && pImpl->level < 0.8f) {
        decayAmount += 0.01f * static_cast<float>(dt);
    }
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small spontaneous fluctuations for realism
    float fluctuation = (std::rand() % 1000) / 1000.0f * 0.01f - 0.005f;
    pImpl->level += fluctuation;
}

void Dopamine::signalReward(float reward) {
    // More realistic reward signaling with burst dynamics
    // Dopamine neurons show phasic bursts proportional to reward prediction error
    float rewardSignal = reward * pImpl->releaseRate;
    
    // Soft bounds for realism (not perfect, but plausible)
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Increase peak for higher rewards
    pImpl->peak = std::min(1.0f, 1.0f + reward * 0.2f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Realistic prediction error signaling
    // Positive errors (better than expected) cause dopamine increase
    // Negative errors (worse than expected) cause suppression
    
    if (error > 0.0f) {
        // Better than expected - phasic burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    } else {
        // Worse than expected - dip
        float dipAmount = -error * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipAmount);
        
        // Lower baseline for subsequent trials (learning)
        pImpl->baseline = std::max(pImpl->baseline - 0.05f * error, 0.0f);
    }
}

// Reset to baseline for new trials
void Dopamine::resetToBaseline() {
    pImpl->level = pImpl->baseline;
}

// Set learning rate based on performance
void Dopamine::setLearningRate(float rate) {
    pImpl->releaseRate = std::clamp(rate, 0.1f, 2.0f);
}

// Boost dopamine for unexpected rewards (exploration incentive)
void Dopamine::boostForNovelty(float noveltyFactor) {
    if (noveltyFactor > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * noveltyFactor);
    }
}

} // namespace nlm
