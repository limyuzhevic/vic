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
    // Dopamine enhances LTP and suppresses LTD based on level
    // Phasic bursts (high DA) strongly enhance plasticity
    // Tonic DA (moderate level) maintains baseline plasticity
    // Low DA reduces plasticity
    
    if (pImpl->level < 0.1f) {
        // Low dopamine suppresses plasticity
        return 0.1f * pImpl->level;
    } else if (pImpl->level < 0.5f) {
        // Moderate dopamine enables baseline plasticity
        return 0.1f + 0.4f * (pImpl->level - 0.1f) / 0.4f;
    } else {
        // High dopamine strongly enhances LTP
        return 0.5f + 0.5f * (pImpl->level - 0.5f) / 0.5f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics
    // Models both phasic (burst) and tonic responses
    // Uses differential equations for realistic temporal dynamics
    
    const float dt_sec = static_cast<float>(dt) * 0.001f;  // Convert to seconds
    float tau_decay = 2.0f;  // Decay time constant (seconds)
    float tau_release = 0.5f;  // Release time constant (seconds)
    
    // Differential equation for dopamine level: dDA/dt = -DA/tau_decay + input
    // where input is from reward and prediction error signals
    float input_rate = (pImpl->peak - pImpl->level) / tau_release;  // Driving towards peak
    float decay_rate = pImpl->level / tau_decay;  // Natural decay
    
    // Update dopamine level using Euler integration
    pImpl->level += (input_rate - decay_rate) * dt_sec;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling
    // Phasic dopamine bursts proportional to reward magnitude
    // Includes prediction error component for learning
    // Models both primary and secondary reward processing
    
    if (reward > 0.0f) {
        // Phasic burst: dopamine level spikes proportionally to reward
        // With saturation to model response limits
        float burst_intensity = std::tanh(reward * 2.0f);  // Non-linear saturation
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst_intensity * 0.8f);
        
        // Add a small baseline to model sustained activity
        pImpl->level += 0.1f * burst_intensity;
        
        // Clamp to ensure reasonable values
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // Based on temporal difference (TD) learning theory
    // Phasic responses to positive errors (surprise rewards)
    // Suppress responses to negative errors (prediction errors)
    // Models biological findings of asymmetric dopamine responses
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected): phasic burst
        // More intense for larger errors (up to saturation)
        float burst_intensity = std::tanh(error * 1.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst_intensity * 0.6f);
    } else {
        // Negative prediction error (worse than expected): mild suppression
        // Smaller effect to match biological asymmetry
        float suppression = std::max(0.0f, -error * 0.3f);
        pImpl->level = std::max(0.0f, pImpl->level - suppression);
    }
    
    // Add a small baseline recovery component
    pImpl->level = std::max(pImpl->level, pImpl->baseline * 0.5f);
}

} // namespace nlm
