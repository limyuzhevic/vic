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
    // Real dopamine-modulated plasticity factor based on computational neuroscience
    // Dopamine at low levels (0.0-0.2): Reduces plasticity (stability)
    // Dopamine at medium levels (0.2-0.6): Optimizes plasticity (learning)
    // Dopamine at high levels (0.6-1.0): Increases plasticity (exploration)
    if (pImpl->level < 0.2f) {
        return 0.1f + 0.3f * (pImpl->level / 0.2f);  // Low: 0.1 to 0.4
    } else if (pImpl->level < 0.6f) {
        return 0.4f + 0.5f * ((pImpl->level - 0.2f) / 0.4f);  // Medium: 0.4 to 0.9
    } else {
        return 0.9f + 0.1f * ((pImpl->level - 0.6f) / 0.4f);  // High: 0.9 to 1.0
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics implementing pharmacokinetics and neural release
    // Time-dependent decay towards baseline with stochastic fluctuations
    static constexpr float NEURONAL_RELEASE_SCALE = 2.0f;
    static constexpr float VARIABILITY = 0.05f;
    
    // Deterministic decay
    float newLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add neuronal release noise based on current level (dopamine neurons self-regulate)
    float releaseNoise = 1.0f + VARIABILITY * std::abs(pImpl->level - pImpl->baseline);
    newLevel = std::min(1.0f, newLevel * releaseNoise);
    
    // Add temporal variability
    static float rngState = 42.0f;
    rngState = std::fmod(rngState * 1103515245.0f + 12345.0f, 32768.0f);
    float temporalVar = (rngState / 32768.0f - 0.5f) * 0.1f * NEURONAL_RELEASE_SCALE;
    
    pImpl->level = std::clamp(newLevel + temporalVar, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling implementing reward prediction error with temporal dynamics
    // Phasic dopamine response with adaptation and homeostatic regulation
    static constexpr float MAX_REWARD_IMPACT = 0.8f;
    static constexpr float EXPECTED_REWARD = 0.5f;
    static constexpr float REWARD_CONSISTENCY = 0.9f;
    
    // Reward prediction error (RPE) following temporal difference learning
    float rpe = reward - EXPECTED_REWARD;
    rpe = std::clamp(rpe * REWARD_CONSISTENCY, -MAX_REWARD_IMPACT, MAX_REWARD_IMPACT);
    
    // Phasic burst with saturation
    float burstSize = rpe * pImpl->releaseRate;
    float newLevel = pImpl->level + burstSize;
    
    // Add short-term depression after large bursts
    if (std::abs(burstSize) > 0.3f) {
        float depression = 0.2f * std::abs(burstSize);
        pImpl->baseline = std::max(pImpl->baseline - depression, 0.0f);
    }
    
    pImpl->level = std::clamp(newLevel, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with asymmetric learning rates
    // Positive errors (unexpected reward) produce stronger response than negative errors
    static constexpr float POSITIVE_ERROR_SCALE = 2.0f;  // Phasic bursts
    static constexpr float NEGATIVE_ERROR_SCALE = 0.6f;  // Sustained dips
    static constexpr float TEMPORAL_FILTER = 0.9f;       // Temporal integration
    
    // Asymmetric response: positive RPE (unexpected reward) > negative RPE (missed reward)
    float errorMagnitude = std::abs(error);
    float responseScale = error > 0.0f ? POSITIVE_ERROR_SCALE : NEGATIVE_ERROR_SCALE;
    
    // Temporal filtered response
    float filteredError = pImpl->level * TEMPORAL_FILTER + errorMagnitude * (1.0f - TEMPORAL_FILTER);
    
    // Sign-preserving application
    float newLevel = pImpl->level + error * responseScale * filteredError;
    
    // Bounded to biologically plausible range
    pImpl->level = std::clamp(newLevel, 0.0f, 1.0f);
    
    // Update expected reward based on prediction error
    if (error > 0.1f) {
        EXPECTED_REWARD = 0.5f + error * 0.1f;
    }
}

} // namespace nlm
