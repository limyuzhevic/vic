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
    // Real dopamine-modulated plasticity factor implementation
    // Dopamine enhances synaptic plasticity for reward-paired experiences
    // Modulated by prediction error to focus learning on surprising events
    
    if (pImpl->level < 0.05f) {
        return 1.0f;  // Baseline plasticity at low DA
    }
    
    // Higher dopamine increases plasticity non-linearly
    float modulation = 1.0f + 2.0f * pImpl->level;  // Up to 3x increase at peak
    
    // Ensure plasticity doesn't exceed reasonable bounds
    return std::clamp(modulation, 1.0f, 5.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics implementation
    // Based on computational models of DA neuron activity
    
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline exponentially
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-timeDecay);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling implementation
    // Implements phasic dopamine bursts on reward delivery
    // Based on reinforcement learning prediction error models
    
    if (reward > 0.0f) {
        // Phasic burst proportional to reward magnitude
        float burst = std::min(pImpl->peak, reward * pImpl->releaseRate * 2.0f);
        
        // Add to current level (not replace)
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Maintain some memory of the reward
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.1f;  // 10% of peak as baseline
        }
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Implements temporal difference learning prediction error
    // Positive error: better than expected, Negative error: worse than expected
    
    // Dopamine neurons encode prediction error in firing rate
    if (error > 0.0f) {
        // Positive prediction error (better outcome): dopamine burst
        float burst = std::min(0.5f, error * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    } else {
        // Negative prediction error (worse outcome): dopamine dip
        float dip = std::min(0.5f, -error * pImpl->releaseRate * 0.5f);
        pImpl->level = std::max(0.0f, pImpl->level - dip);
    }
}

} // namespace nlm
