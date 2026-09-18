#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityInfluence;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), plasticityInfluence(1.0f) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    // For now, implement actual dopamine dynamics
    // Dopamine at level 0 = no modulation, level 1 = full facilitation
    
    // Dopamine facilitates LTP more than LTD
    // At low dopamine, LTD dominates; at high dopamine, LTP dominates
    if (pImpl->level < 0.3f) {
        // Low dopamine - LTD predominates
        return 0.5f - 0.4f * pImpl->level;
    } else {
        // High dopamine - LTP predominates  
        return 0.5f + 0.5f * (pImpl->level - 0.3f) / 0.7f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    // Implement actual dopamine dynamics with spikes and pulses
    
    float dt_f = static_cast<float>(dt);
    
    // Base decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dt_f);
    
    // Simulate phasic bursts based on prediction errors and rewards
    // This is a simplified implementation - real dopamine would have temporal dynamics
    if (pImpl->peak > 0.0f) {
        // Create smooth transitions between baseline and peak levels
        float target = pImpl->peak * 0.2f;  // Reduced peak effect
        if (pImpl->level < target) {
            pImpl->level = std::min(target, pImpl->level + pImpl->releaseRate * dt_f * 0.5f);
        }
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    // Implement realistic reward prediction error signaling
    
    if (reward > 0.0f) {
        // Reward prediction error: actual reward > expected
        // Dopamine increases proportionally to the prediction error
        float rewardPulse = reward * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + rewardPulse);
        
        // Update baseline to reflect new expected value
        pImpl->baseline = std::max(pImpl->baseline, pImpl->level * 0.7f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    // Implement detailed reward prediction error dynamics
    
    if (error > 0.0f) {
        // Positive prediction error: better than expected
        // Dopamine bursts signal prediction error for learning
        float errorPulse = error * pImpl->releaseRate * 2.0f;
        pImpl->level = std::max(pImpl->level + errorPulse, pImpl->level * 1.1f);
        
        // Mark for plasticity enhancement
        pImpl->peak = std::max(pImpl->peak, pImpl->level * 1.2f);
    } else {
        // Negative prediction error: worse than expected
        // Dopamine dips or dip signals prediction error
        float errorPulse = -error * pImpl->decayRate;  // Use decay as negative factor
        pImpl->level = std::max(pImpl->baseline, pImpl->level - errorPulse);
    }
}

} // namespace nlm
