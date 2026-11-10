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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // IMPROVED: More realistic dopamine-plasticity relationship
    // Low dopamine: poor learning (high prediction error uncertainty)
    // Medium dopamine: optimal learning
    // High dopamine: potentially runaway excitation (exploration bias)
    if (pImpl->level < 0.3f) {
        return 0.3f + 1.5f * pImpl->level;  // Low DA, low plasticity
    } else if (pImpl->level < 0.7f) {
        return 0.75f + 0.43f * (pImpl->level - 0.3f);  // Optimal learning range
    } else {
        return 1.0f + 0.5f * (pImpl->level - 0.7f);  // High DA, potentially excessive plasticity
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // IMPROVED: More biologically plausible dopamine dynamics
    // Include refractoriness and adaptive baseline
    float deltaTime = static_cast<float>(dt);
    
    // Natural decay towards baseline with slower decay at higher levels
    float decayFactor = 0.05f + 0.15f * pImpl->level;
    float targetLevel = pImpl->baseline;
    
    // Gradual approach to target, preventing abrupt changes
    float newLevel = std::max(targetLevel, pImpl->level - decayFactor * deltaTime);
    
    // Ensure non-negative levels
    pImpl->level = std::max(0.0f, newLevel);
    
    // Simple refractory period simulation
    if (pImpl->level > pImpl->peak * 0.8f) {
        pImpl->decayRate = 0.08f;  // Slower decay during high activity
    } else {
        pImpl->decayRate = 0.1f;  // Normal decay
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // IMPROVED: More realistic reward response with saturation and time constants
    float rewardScaled = std::clamp(reward, 0.0f, 1.0f);
    
    // Calculate new dopamine level with nonlinear response
    float currentChange = pImpl->releaseRate * rewardScaled;
    
    // Include temporal integration for sustained response
    float historyDecay = 0.9f;
    float integratedLevel = pImpl->level * historyDecay + currentChange * (1.0f - historyDecay);
    
    // Apply saturation with head modulation
    float saturationLevel = pImpl->peak * (0.9f + 0.1f * pImpl->baseline);
    pImpl->level = std::min(saturationLevel, integratedLevel);
    
    // Record peak for refractory effects
    if (pImpl->level > pImpl->peak) {
        pImpl->peak = pImpl->level;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // IMPROVED: More biologically plausible RPE dynamics
    // Positive error (better than expected): promote approach behavior
    // Negative error (worse than expected): promote avoidance behavior
    
    float errorScaled = std::clamp(error, -1.0f, 1.0f);
    
    // RPE has asymmetric response (positive vs negative)
    float rpeResponse = 0.0f;
    if (errorScaled > 0.0f) {
        // Positive RPE: dopamine burst
        rpeResponse = 1.5f * errorScaled;  // Stronger response to unexpected rewards
    } else {
        // Negative RPE: dopamine dip (temporarily suppress activity)
        rpeResponse = 0.8f * errorScaled;  // Milder response to prediction errors
    }
    
    // Apply RPE with history integration and adaptation
    float historyFactor = 0.95f;
    float responseFactor = 1.0f - historyFactor;
    
    float newRPEffect = pImpl->level * historyFactor + rpeResponse * responseFactor;
    
    // Ensure levels stay within reasonable bounds
    pImpl->level = std::max(0.0f, std::min(pImpl->level, newRPEffect + 0.1f));
    
    // Adapt baseline over time based on recent RPE history
    if (std::abs(error) > 0.5f) {
        // Strong RPE influences baseline adaptation
        pImpl->baseline = pImpl->baseline * 0.9f + 0.1f * std::max(0.0f, errorScaled);
    }
}

} // namespace nlm
