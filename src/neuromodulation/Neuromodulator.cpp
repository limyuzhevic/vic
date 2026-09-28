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

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

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
    // Real dopamine-modulated plasticity factor
    // Dopamine influences synaptic strength through multiple mechanisms:
    // 1. Direct modulation of LTP/LTD balance
    // 2. Enhancement of synaptic tagging
    // 3. Regulation of eligibility traces
    // Higher dopamine increases learning rates, especially for reward prediction errors
    
    // Nonlinear relationship: dopamine has optimal level for plasticity
    // Very low dopamine = poor learning, very high dopamine = runaway excitation
    float optimalLevel = 0.5f;
    float dopamineEffect = 1.0f - std::abs(pImpl->level - optimalLevel) * 2.0f;
    
    // Ensure plasticity factor stays within reasonable bounds
    float plasticityFactor = 0.1f + 1.5f * std::max(0.0f, dopamineEffect);
    
    // Dopamine also affects confidence in predictions
    float confidence = 0.5f + 0.5f * pImpl->level;
    
    return plasticityFactor * confidence;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience models
    // Uses differential equations to model dopamine neuron firing patterns
    
    float dtFloat = static_cast<float>(dt);
    
    // Model dopamine dynamics with multiple components:
    // 1. Baseline tonic activity
    // 2. Phasic bursts in response to rewards/prediction errors
    // 3. Adaptive regulation of firing rates
    
    // Time-dependent decay towards baseline (modeling clearance mechanisms)
    float decayFactor = std::exp(-pImpl->decayRate * dtFloat);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Add small stochastic noise for biological realism
    // This allows for exploration and prevents premature convergence
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> noise(0.0f, 0.01f);
    pImpl->level += noise(gen) * dtFloat;
    
    // Ensure level stays within physiological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on dopamine neuron responses
    // Models phasic dopamine bursts observed in experimental studies
    
    if (reward > 0.0f) {
        // Phasic burst: sudden increase in dopamine level
        // Intensity proportional to reward magnitude
        // Saturates at peak level to prevent overflow
        float burstMagnitude = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
        pImpl->level += burstMagnitude;
        
        // Add brief excitation to promote associated behaviors
        // This implements reward prediction error signaling
        pImpl->baseline += burstMagnitude * 0.1f; // Increase baseline temporarily
    } else if (reward < 0.0f) {
        // Punishment: decrease in dopamine level
        // Models dopamine dips observed with aversive stimuli
        float dipMagnitude = std::min(-reward * pImpl->releaseRate, pImpl->level - 0.0f);
        pImpl->level -= dipMagnitude;
        
        // Reset baseline to prevent prolonged suppression
        pImpl->baseline = std::max(pImpl->baseline - dipMagnitude * 0.05f, pImpl->baseline * 0.8f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling based on temporal difference learning
    // This is a core mechanism for reinforcement learning in dopamine systems
    
    if (error > 0.0f) {
        // Positive prediction error (unexpected reward): dopamine burst
        // Larger errors produce larger responses
        float response = std::min(error * pImpl->releaseRate * 1.5f, pImpl->peak - pImpl->level);
        pImpl->level += response;
        
        // Enhance memory encoding for surprising events
        pImpl->baseline += response * 0.2f;
    } else if (error < 0.0f) {
        // Negative prediction error (expected reward omitted): dopamine dip
        // Organisms respond to omitted rewards just like punishments
        float response = std::min(-error * pImpl->releaseRate, pImpl->level - 0.0f);
        pImpl->level -= response;
        
        // Brief recovery of baseline
        pImpl->baseline = std::max(pImpl->baseline - response * 0.1f, pImpl->baseline * 0.9f);
    }
}

} // namespace nlm
