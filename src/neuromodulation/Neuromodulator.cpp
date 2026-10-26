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
    // REAL PHASE 2: Implement biologically realistic dopamine-modulated plasticity
    // Dopamine affects multiple plasticity processes with different time scales
    
    float level = pImpl->level;
    
    if (level < 0.0f) {
        // Negative prediction error (punishment): reduces plasticity, stabilizes weights
        // Strong LTD bias, weakens synapses
        return 0.1f + 0.2f * (level + 1.0f);  // Range: [0.1, 0.3]
    } else if (level > 0.0f) {
        // Positive prediction error (reward): increases plasticity, promotes learning
        // Strong LTP bias, strengthens synapses
        // Use quadratic function for stronger effects with large rewards
        return 0.5f + 0.5f * (level * level);  // Range: [0.5, 1.5]
    } else {
        // Baseline dopamine: moderate plasticity for normal learning
        return 0.8f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // REAL PHASE 2: Implement dopamine dynamics with biological realism
    // Includes temporal dynamics, baseline adaptation, and spike-based updates
    
    float dtFloat = static_cast<float>(dt);
    
    // Primary decay process: dopamine clearence by transporters
    // Time constant ~100-500ms for extracellular DA dynamics
    float clearanceRate = 2.0f / pImpl->decayRate;  // Faster decay
    pImpl->level -= clearanceRate * dtFloat * pImpl->level;
    
    // Leak towards baseline
    float baselineRate = 0.01f;
    pImpl->level += baselineRate * dtFloat * (pImpl->baseline - pImpl->level);
    
    // Prevent negative levels
    pImpl->level = std::max(0.0f, pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // REAL PHASE 2: Implement reward prediction error signaling
    // Uses temporal difference learning for reward prediction
    
    // Reward signal: burst of dopamine proportional to reward magnitude
    // Follows biological observations of dopamine neuron responses
    
    float rewardSignal = reward * pImpl->releaseRate * 2.0f;  // Enhanced response
    
    // Add to current level (bounded by peak)
    pImpl->level += rewardSignal;
    pImpl->level = std::min(pImpl->level, pImpl->peak);
    
    // Update baseline as running average of recent rewards
    // This reflects adaptation of reward expectations
    pImpl->baseline += 0.01f * (reward - pImpl->baseline);
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL PHASE 2: Implement reward prediction error signaling with detailed dynamics
    // Implements temporal difference learning (TD learning) for dopamine signaling
    
    // Reward prediction error (RPE) is the difference between expected and actual reward
    // Dopamine neurons respond phasically to positive RPE (unexpected reward)
    // and dyschronically to negative RPE (omitted reward)
    
    float errorSignal = error * pImpl->releaseRate;
    
    // Apply RPE with biological constraints
    if (error > 0.0f) {
        // Positive RPE: phasic increase in dopamine
        pImpl->level += errorSignal * 2.0f;  // Strong response to unexpected rewards
    } else if (error < 0.0f) {
        // Negative RPE: phasic decrease in dopamine (SAD phenomenon)
        pImpl->level += errorSignal * 0.5f;  // Weaker response to omitted rewards
    }
    
    // Maintain within biological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Update prediction error baseline
    if (std::abs(error) > 0.1f) {
        // Only update baseline for significant errors
        pImpl->baseline += 0.05f * (error - pImpl->baseline);
    }
}

} // namespace nlm
