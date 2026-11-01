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
    // Implement real dopamine-modulated plasticity factor
    // This is the core of reward-modulated learning
    
    if (pImpl->level <= 0.0f) {
        // No dopamine: baseline plasticity (exploration)
        return 0.1f;  // Minimal plasticity
    }
    
    // Dopamine modulates plasticity: higher levels increase plasticity
    // This implements the core dopamine hypothesis of reinforcement learning
    float plasticityFactor = 0.1f + 0.8f * pImpl->level;
    
    // Saturation: very high dopamine may inhibit plasticity
    if (pImpl->level > 0.9f) {
        plasticityFactor = 0.95f + 0.05f * (1.0f - pImpl->level);
    }
    
    // Log the modulation
    NLM_LOG_DEBUG("Dopamine level " + std::to_string(pImpl->level) + 
                  " -> plasticity factor " + std::to_string(plasticityFactor));
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics with temporal filtering
    // This models the temporal dynamics of dopamine neuron firing
    
    if (dt <= 0.0) {
        return;
    }
    
    // Decay towards baseline: dopamine neurons have tonic firing
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Add small random fluctuations (noise)
    float noise = (std::rand() / (RAND_MAX + 1.0f)) * 0.02f - 0.01f;
    pImpl->level += noise;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Log update
    if (pImpl->level > 0.5f) {
        NLM_LOG_TRACE("Dopamine update: " + std::to_string(pImpl->level));
    }
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling with burst dynamics
    // This models reward prediction error signaling
    
    if (reward <= 0.0f) {
        return;  // No negative rewards
    }
    
    // Reward causes burst of dopamine activity
    float burstAmount = std::min(reward * pImpl->releaseRate, 
                               pImpl->peak - pImpl->level);
    
    pImpl->level += burstAmount;
    
    // Log reward signaling
    NLM_LOG_INFO("Reward signal: " + std::to_string(reward) + 
                 " -> dopamine burst: " + std::to_string(burstAmount));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // This is crucial for TD learning and updating value predictions
    
    // Reward prediction error has both positive (better than expected) 
    // and negative (worse than expected) components
    
    if (error > 0.0f) {
        // Positive prediction error: unexpected reward
        // Signals better-than-expected outcome
        float burst = std::min(error * pImpl->releaseRate, 
                              pImpl->peak - pImpl->level);
        pImpl->level += burst;
        
        NLM_LOG_INFO("RPE positive: " + std::to_string(error) + 
                     " -> dopamine burst: " + std::to_string(burst));
    } else if (error < 0.0f) {
        // Negative prediction error: worse-than-expected outcome
        // Causes transient dip in dopamine (learning signal)
        float dip = std::min(std::abs(error) * pImpl->releaseRate, pImpl->level);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dip);
        
        NLM_LOG_INFO("RPE negative: " + std::to_string(error) + 
                     " -> dopamine dip: " + std::to_string(dip));
    }
}

} // namespace nlm
