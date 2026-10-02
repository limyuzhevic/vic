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
    float predictionErrorHistory[3];  // Keep last 3 prediction errors for temporal dynamics
    float rewardHistory[3];            // Keep last 3 rewards for temporal dynamics
    float timeSinceLastEvent;
    float burstIntensity;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , timeSinceLastEvent(0.0f), burstIntensity(0.0f) {
        predictionErrorHistory[0] = predictionErrorHistory[1] = predictionErrorHistory[2] = 0.0f;
        rewardHistory[0] = rewardHistory[1] = rewardHistory[2] = 0.0f;
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
    // Real dopamine-modulated plasticity factor:
    // - Low DA: reduced plasticity (stabilization)
    // - Medium DA: optimal plasticity (learning)
    // - High DA: excessive plasticity (forgetting)
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Inverted U-shaped function for optimal learning
    float optimalLevel = 0.6f;  // Peak of inverted U
    float sigma = 0.2f;         // Width of the curve
    
    float plasticityFactor = std::exp(-std::pow(normalizedLevel - optimalLevel, 2) / (2 * sigma * sigma));
    
    // Apply temporal modulation based on recent events
    float temporalMod = 1.0f;
    if (pImpl->timeSinceLastEvent < 1.0f) {
        temporalMod = std::max(0.5f, 1.0f - pImpl->timeSinceLastEvent);
    }
    
    return plasticityFactor * temporalMod;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    pImpl->timeSinceLastEvent += deltaTime;
    
    // Real dopamine dynamics with homeostatic regulation
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline with activity-dependent rate
    float decay = pImpl->decayRate;
    if (pImpl->level > pImpl->baseline * 1.1f) {
        decay *= 1.5f;  // Faster decay when above baseline
    }
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * deltaTime);
    
    // Handle burst reset
    pImpl->burstIntensity = std::max(0.0f, pImpl->burstIntensity - 0.5f * deltaTime);
}

void Dopamine::signalReward(float reward) {
    // Temporal integration of reward history
    // Shift history buffer
    for (int i = 2; i >= 0; --i) {
        pImpl->rewardHistory[i] = pImpl->rewardHistory[i-1];
    }
    pImpl->rewardHistory[0] = reward;
    
    // Calculate temporal reward rate
    float recentRewardRate = (rewardHistory[0] + rewardHistory[1] + rewardHistory[2]) / 3.0f;
    
    // Real dopamine burst response with phasic dynamics
    float burstAmount = reward * pImpl->releaseRate;
    
    // Modulate burst based on recent reward history
    if (recentRewardRate > 0.5f) {
        burstAmount *= 0.5f;  // Diminished response to predictable rewards
    }
    
    // Apply burst with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    
    // Reset event timer
    pImpl->timeSinceLastEvent = 0.0f;
    
    // Increase burst intensity for next events
    pImpl->burstIntensity = std::min(1.0f, pImpl->burstIntensity + 0.3f * reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Temporal integration of prediction error history
    for (int i = 2; i >= 0; --i) {
        pImpl->predictionErrorHistory[i] = pImpl->predictionErrorHistory[i-1];
    }
    pImpl->predictionErrorHistory[0] = error;
    
    // Calculate recent prediction error average
    float recentErrorAvg = (predictionErrorHistory[0] + predictionErrorHistory[1] + predictionErrorHistory[2]) / 3.0f;
    
    // Real dopamine response to prediction error
    // - Positive error (better than expected): increase dopamine
    // - Negative error (worse than expected): decrease dopamine
    float errorSignal = error * pImpl->releaseRate;
    
    // Apply temporal filtering
    float temporalFilter = 0.7f + 0.3f * std::abs(recentErrorAvg);
    errorSignal *= temporalFilter;
    
    // Update dopamine level with bounded change
    float newLevel = pImpl->level + errorSignal;
    newLevel = std::max(pImpl->baseline, std::min(pImpl->peak, newLevel));
    
    // Apply short-term depression for large errors
    if (std::abs(error) > 1.0f) {
        newLevel *= 0.7f;
    }
    
    pImpl->level = newLevel;
    pImpl->timeSinceLastEvent = 0.0f;
}

} // namespace nlm