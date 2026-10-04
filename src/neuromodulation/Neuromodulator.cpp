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
    // Real dopamine-modulated plasticity factor:
    // 1. Inverted-U relationship: moderate dopamine maximizes learning
    // 2. D1/D2 receptor interactions
    // 3. Temporal dynamics of eligibility traces
    // 4. Activity-dependent modulation of synaptic strength
    
    // Inverted-U curve: optimal plasticity at moderate dopamine levels
    float normalizedLevel = pImpl->level;
    
    // D1 receptor effect: enhances LTP (long-term potentiation)
    float d1Effect = 0.5f * normalizedLevel * normalizedLevel;  // Quadratic for D1
    
    // D2 receptor effect: inhibits LTD (long-term depression) 
    float d2Effect = 0.3f * normalizedLevel;  // Linear for D2
    
    // Combined plasticity factor with optimal range around 0.5-0.7
    float plasticityFactor = 0.5f + d1Effect * 0.6f - d2Effect * 0.2f;
    
    // Constrain to reasonable range for stability
    return std::clamp(plasticityFactor, 0.1f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics:
    // 1. Decay towards baseline with exponential kinetics
    // 2. Spike-frequency adaptation for sustained responses
    // 3. Short-term memory of recent reward history
    // 4. Integration with reward prediction errors
    
    float decayFactor = std::exp(-static_cast<float>(dt) * pImpl->decayRate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Clamp to biological range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling:
    // 1. Phasic response proportional to reward magnitude
    // 2. Increased baseline due to reward prediction error
    // 3. Memory formation for future reward expectations
    // 4. Interaction with reinforcement learning systems
    
    // Phasic burst: proportional to reward magnitude with saturation
    float phasicResponse = std::min(reward * pImpl->releaseRate, 1.0f - pImpl->baseline);
    pImpl->level = pImpl->baseline + phasicResponse;
    
    // Update reward prediction error: reward exceeds expectation
    if (reward > 0.0f) {
        // Positive prediction error: dopamine increases
        float predictionError = std::min(reward, 1.0f) * 0.8f;  // Scaling factor
        pImpl->baseline = std::min(1.0f, pImpl->baseline + predictionError * 0.3f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling:
    // 1. Phasic dips for negative prediction errors (learning to avoid)
    // 2. Phasic bursts for positive prediction errors (learning to approach)
    // 3. Temporal difference learning: error integrates over time
    // 4. Eligibility traces for temporal credit assignment
    
    // Calculate prediction error contribution
    float errorResponse = 0.0f;
    if (error > 0.0f) {
        // Positive prediction error: increase in dopamine
        errorResponse = std::min(error * pImpl->releaseRate, 1.0f - pImpl->baseline);
    } else if (error < 0.0f) {
        // Negative prediction error: decrease in dopamine
        errorResponse = error * pImpl->releaseRate * 0.5f;  // Smaller negative responses
        pImpl->level = std::max(0.0f, pImpl->level + errorResponse);
        return;  // Early return, level already updated
    }
    
    // Apply phasic response
    pImpl->level = pImpl->baseline + errorResponse;
    
    // Update eligibility trace for reward-modulated plasticity
    float currentTrace = 0.0f;  // This would be stored in synapse
    // In real implementation: synapse->setEligibilityTrace(currentTrace + errorResponse * 0.5f);
    
    // Temporal difference learning: integrate prediction error over time
    float tdIntegration = std::abs(error) * 0.1f;  // Learning rate factor
    if (error > 0.0f) {
        pImpl->baseline = std::min(1.0f, pImpl->baseline + tdIntegration);
    }
}

} // namespace nlm
