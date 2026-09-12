#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics state
    float level;                    // Current dopamine concentration [0, 1]
    float baseline;                 // Baseline dopamine level
    float peak;                     // Maximum dopamine level
    float decayRate;                // Rate of decay towards baseline
    float releaseRate;              // Rate of increase on signals
    float adaptationTime;           // Time constant for adaptation
    float predictionError;         // Reward prediction error input
    float rewardInput;              // Raw reward input
    float history;                  // Rolling average of recent level
    float historyWeight;            // Weight for exponential moving average
    float thresholdLow;             // Low threshold for burst signaling
    float thresholdHigh;            // High threshold for sustained signaling
    bool isBursting;                // Whether dopamine is in burst mode
    
    Impl() 
        : level(0.0f)
        , baseline(0.02f)           // Typical baseline DA ~20 nM
        , peak(1.0f)
        , decayRate(0.05f)          // Decay time constant ~20 steps
        , releaseRate(1.5f)          // Fast release for bursts
        , adaptationTime(50.0f)      // Slow adaptation
        , predictionError(0.0f)
        , rewardInput(0.0f)
        , history(0.0f)
        , historyWeight(0.1f)
        , thresholdLow(0.2f)
        , thresholdHigh(0.6f)
        , isBursting(false) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

Dopamine::~Dopamine() {
    delete pImpl;
}

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
    // Real dopamine-modulated plasticity: Bell-shaped curve
    // Optimal learning at moderate dopamine levels
    float normalizedLevel = pImpl->level / 0.5f;  // Normalize to peak at 0.5
    float plasticity = 0.2f + 0.8f * exp(-(normalizedLevel - 1.0f) * 
                                         (normalizedLevel - 1.0f) / 0.3f);
    return std::clamp(plasticity, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    
    // Update rolling average
    pImpl->history = pImpl->history * (1.0f - pImpl->historyWeight) + 
                     pImpl->level * pImpl->historyWeight;
    
    // Apply adaptive decay rate
    float adaptiveDecay = pImpl->decayRate;
    if (pImpl->isBursting) {
        adaptiveDecay *= 2.0f;  // Faster decay during bursts
    }
    
    // Decay towards baseline with adaptation
    float decayAmount = adaptiveDecay * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply prediction error
    if (std::abs(pImpl->predictionError) > 0.01f) {
        float errorUpdate = pImpl->releaseRate * pImpl->predictionError * dtf;
        pImpl->level += errorUpdate;
        pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
        
        // Detect burst mode
        if (pImpl->level > pImpl->thresholdHigh) {
            pImpl->isBursting = true;
        } else if (pImpl->level < pImpl->thresholdLow) {
            pImpl->isBursting = false;
        }
    }
    
    // Apply reward input
    if (pImpl->rewardInput > 0.0f) {
        float rewardUpdate = pImpl->releaseRate * pImpl->rewardInput * dtf;
        pImpl->level += rewardUpdate;
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        pImpl->isBursting = true;
    }
    
    // Reset inputs
    pImpl->predictionError = 0.0f;
    pImpl->rewardInput = 0.0f;
    
    // Log significant changes
    if (pImpl->isBursting && pImpl->level > 0.8f) {
        NLM_LOG_DEBUG("Dopamine burst: level = " + std::to_string(pImpl->level));
    }
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardInput = std::clamp(reward, 0.0f, 1.0f);
    
    // Burst firing on reward
    if (reward > 0.5f) {
        // Large reward triggers strong burst
        float burst = 0.5f + 0.5f * reward;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        NLM_LOG_INFO("Dopamine reward burst: " + std::to_string(reward));
    } else {
        // Small reward increases level gradually
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * 0.3f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionError = error;
    
    // Reward prediction error directly modulates dopamine
    if (error > 0.0f) {
        // Positive RPE increases dopamine (better than expected)
        float update = 0.3f * error;
        pImpl->level = std::min(pImpl->peak, pImpl->level + update);
        NLM_LOG_DEBUG("Dopamine RPE positive: " + std::to_string(error));
    } else if (error < 0.0f) {
        // Negative RPE decreases dopamine (worse than expected)
        float update = 0.2f * std::abs(error);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - update);
        NLM_LOG_DEBUG("Dopamine RPE negative: " + std::to_string(error));
    }
}

void Dopamine::applyToExcitability(float& restingPotential) const {
    // Dopamine modulates neuronal excitability
    float daEffect = pImpl->level * 0.5f;  // 0 to 0.5 effect
    restingPotential += daEffect * 10.0f;  // 0 to 5 mV increase
}

void Dopamine::applyToLearningRate(float& learningRate) const {
    // Dopamine modulates learning rate
    learningRate = getPlasticityFactor();
}

} // namespace nlm