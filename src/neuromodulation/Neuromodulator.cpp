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
    // Real dopamine-modulated plasticity factor based on dopamine concentration
    // Follows biologically realistic dose-response curve:
    // Low dopamine (0-0.3): weak plasticity, promotes stability
    // Moderate dopamine (0.3-0.7): optimal plasticity for learning
    // High dopamine (0.7-1.0): very high plasticity, can cause instability but enables rapid learning
    
    if (pImpl->level < 0.3f) {
        // Low dopamine: minimal plasticity, synaptic strengthening suppressed
        return 0.1f + 0.4f * (pImpl->level / 0.3f);
    } else if (pImpl->level < 0.7f) {
        // Moderate dopamine: optimal learning window
        return 0.5f + 0.5f * ((pImpl->level - 0.3f) / 0.4f);
    } else {
        // High dopamine: very high plasticity, can lead to overwrite of existing memories
        return 0.9f + 0.9f * ((pImpl->level - 0.7f) / 0.3f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple time scales
    // Includes automatic decay toward baseline and response to prediction errors
    
    // Primary decay toward baseline (tonic dopamine dynamics)
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Add small spontaneous fluctuations for biological realism
    // Simulates phasic dopamine activity independent of external rewards
    static float timeSinceLastUpdate = 0.0f;
    timeSinceLastUpdate += static_cast<float>(dt);
    if (timeSinceLastUpdate > 10.0f) {  // Small spontaneous bursts ~10ms intervals
        float spontaneousBurst = 0.05f * std::sin(timeSinceLastUpdate * 0.5f);
        pImpl->level = std::clamp(pImpl->level + spontaneousBurst, 0.0f, 1.0f);
    }
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected reward ( reward prediction error > 0 )
    // Follows temporal difference learning rule: δ = reward + γV(s') - V(s)
    
    // Compute prediction error
    float predictionError = reward - (1.0f - pImpl->baseline) * pImpl->level;
    
    // Phasic dopamine burst proportional to prediction error
    float burst = std::max(0.0f, predictionError) * pImpl->releaseRate;
    
    // Ensure level doesn't exceed peak
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update baseline to track recent average (eligibility trace for dopamine)
    pImpl->baseline = pImpl->baseline * 0.99f + pImpl->level * 0.01f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling for reward-modulated plasticity
    // Allows for negative prediction errors (omission of expected reward)
    
    // Normalize prediction error to reasonable bounds
    error = std::clamp(error, -1.0f, 1.0f);
    
    // Update dopamine level based on prediction error
    // Positive errors (better than expected) cause dopamine increase
    // Negative errors (worse than expected) cause dopamine decrease
    float delta = error * pImpl->releaseRate * 0.5f;
    pImpl->level = std::clamp(pImpl->level + delta, 0.0f, 1.0f);
    
    // Adjust baseline to track recent activity
    pImpl->baseline = pImpl->baseline * 0.98f + pImpl->level * 0.02f;
}

} // namespace nlm
