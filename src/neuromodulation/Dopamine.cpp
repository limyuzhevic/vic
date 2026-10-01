#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float signalMemoryTime;
    
    // Temporal dynamics
    float accumulatedReward;
    float predictionError;
    
    // For dopamine dynamics
    float timeSinceLastSignal;
    std::random_device rd;
    std::mt19937 rng;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , signalMemoryTime(0.5f)
        , accumulatedReward(0.0f)
        , predictionError(0.0f)
        , timeSinceLastSignal(0.0f) {
        
        rng.seed(rd());
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
    // Real dopamine-modulated plasticity: 
    // Dopamine increases synaptic efficacy via D1 receptors
    // This is based on research showing dopamine modulates synaptic strength
    // through mechanisms like LTP induction
    if (pImpl->level < 0.2f) {
        // Low dopamine: decreased plasticity
        return 0.3f + 0.5f * (pImpl->level / 0.2f);
    } else if (pImpl->level < 0.8f) {
        // Medium dopamine: optimal plasticity for learning
        return 0.8f + 0.4f * ((pImpl->level - 0.2f) / 0.6f);
    } else {
        // High dopamine: increased but regulated plasticity (prevents runaway excitation)
        return 1.0f - 0.2f * ((pImpl->level - 0.8f) / 0.2f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on neuroscience research:
    // 1. Decay towards baseline ( tonic activity )
    // 2. Rapid phasic responses to salient events
    // 3. Adaptation over time
    
    pImpl->timeSinceLastSignal += static_cast<float>(dt);
    
    // Decay towards baseline (tonic dopamine)
    float targetLevel = baseline + (peak - baseline) * 0.1f;  // Lower tonic level
    pImpl->level += (targetLevel - pImpl->level) * 0.05f;
    
    // Ensure it stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    if (reward <= 0.0f) return;
    
    // Phasic dopamine response to reward prediction error
    float phasicBurst = reward * 0.5f;  // Scaling factor
    
    // Ensure we don't exceed peak
    pImpl->level = std::min(pImpl->level + phasicBurst, pImpl->peak);
    
    // Update accumulated reward (for long-term effects)
    pImpl->accumulatedReward += reward * 0.1f;
    
    // Store time for temporal dynamics
    pImpl->timeSinceLastSignal = 0.0f;
    
    // Log for debugging
    if (pImpl->level > baseline * 2) {
        NLM_LOG_INFO("Dopamine burst: reward=" + std::to_string(reward) + 
                    " level=" + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    if (std::abs(error) < 0.01f) return;
    
    // Reward prediction error is a key dopamine signal in neuroscience
    // Underprediction of reward -> negative prediction error -> decreases dopamine
    // Overprediction of reward -> positive prediction error -> increases dopamine
    
    if (error > 0.0f) {
        // Better than expected: increase dopamine
        pImpl->level = std::min(pImpl->level + error * 0.3f, pImpl->peak);
    } else {
        // Worse than expected: decrease dopamine
        pImpl->level = std::max(pImpl->level + error * 0.2f, 0.0f);
    }
    
    pImpl->predictionError = error;
    pImpl->timeSinceLastSignal = 0.0f;
    
    NLM_LOG_INFO("Dopamine RPE: error=" + std::to_string(error) + 
                " level=" + std::to_string(pImpl->level));
}

float Dopamine::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::resetAccumulatedReward() {
    pImpl->accumulatedReward = 0.0f;
}

} // namespace nlm
