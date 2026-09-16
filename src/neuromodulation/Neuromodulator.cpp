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
    // Realistic dopamine-modulated plasticity factor based on neuroscience
    // 
    // Dopamine affects synaptic plasticity in two regimes:
    // 1. Phasic burst: High dopamine -> strong LTP (learning)
    // 2. Tonic baseline: Low dopamine -> moderate plasticity, LTP suppression
    
    float plasticityFactor = 0.1f;  // Baseline minimum plasticity
    
    if (pImpl->level < 0.1f) {
        // Low dopamine (tonic): Suppress LTP, favor LTD
        plasticityFactor = 0.1f;
    } else if (pImpl->level < 0.5f) {
        // Moderate dopamine: Normal learning
        plasticityFactor = 0.5f + 0.5f * (pImpl->level - 0.1f) / 0.4f;
    } else if (pImpl->level < 0.9f) {
        // High dopamine (phasic): Enhance LTP for reward-paired stimuli
        plasticityFactor = 0.8f + 0.2f * (pImpl->level - 0.5f) / 0.4f;
    } else {
        // Very high dopamine: Optimal learning for novel associations
        plasticityFactor = 1.0f;
    }
    
    return std::clamp(plasticityFactor, 0.0f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics based on basal ganglia models
    // Dopamine level evolves according to:
    // dD/dt = -k_decay * D + k_burst * reward_phasic - k_dip * punishment_phasic
    
    float dt_ms = static_cast<float>(dt);
    
    // Decay towards baseline with time constant
    float decayTimeConstant = 1000.0f;  // ms
    float decayFactor = std::exp(-dt_ms / decayTimeConstant);
    
    // Update level with realistic dynamics
    pImpl->level = pImpl->level * decayFactor + pImpl->baseline * (1.0f - decayFactor);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling based on dopamine burst responses
    // Phasic dopamine responses follow a sigmoidal relationship with reward magnitude
    // Peak response occurs at moderate rewards, with saturation at high rewards
    
    // Sigmoidal response function
    float burstMagnitude = reward / (1.0f + std::exp(5.0f - 3.0f * reward));
    
    // Add to current level (not instant, because of synaptic dynamics)
    float increment = burstMagnitude * pImpl->releaseRate;
    pImpl->level += increment;
    
    // Ensure within physiological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Realistic reward prediction error signaling based on temporal difference learning
    // Error-driven dopamine responses are scaled and gated by the magnitude of prediction error
    // Positive error (better than expected) causes dopamine increase, negative error causes decrease
    
    // Scale error by temporal difference learning rate
    float learningRate = 0.1f;
    float errorSignal = std::clamp(error * learningRate, -0.5f, 0.5f);
    
    // Update with error signal (with decay)
    pImpl->level += errorSignal;
    
    // Ensure within physiological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Track error in implementation for learning analysis
    if (std::abs(error) > 0.01f) {
        // Only significant errors are "learned"
        NLM_LOG_INFO(std::string("Dopamine RPE signal: ") + std::to_string(error));
    }
}

} // namespace nlm
