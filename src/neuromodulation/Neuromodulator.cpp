#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;      // Time constant for decay (seconds)
    float burstGain;         // Gain for burst responses
    float predictionErrorGain; // Gain for prediction error responses
    float lastUpdateTime;    // For time-based dynamics
    
    Impl() : level(0.0f), baseline(0.15f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f),
             timeConstant(2.0f), burstGain(2.0f), predictionErrorGain(1.2f), lastUpdateTime(0.0f) {}
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
    // Based on neuroscience: Dopamine modulates synaptic plasticity
    // - Low DA: Suppress plasticity (focus, stability)
    // - Medium DA: Optimal learning (reward prediction)
    // - High DA: Enhance plasticity (exploration, memory)
    
    float level = pImpl->level;
    float baseline = pImpl->baseline;
    
    // Center around baseline
    float deviation = level - baseline;
    
    // Nonlinear response: bell-shaped curve for optimal learning
    // Maximum plasticity at moderate deviation from baseline
    float plasticity = 1.0f - std::abs(deviation * 1.5f);
    
    // Ensure plasticity is within reasonable bounds
    plasticity = std::clamp(plasticity, 0.1f, 1.0f);
    
    // Add some time-dependent modulation
    float timeMod = std::exp(-pImpl->lastUpdateTime / pImpl->timeConstant);
    plasticity *= (0.7f + 0.3f * timeMod);
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on neuroscience
    // Includes: decay to baseline, time-dependent effects, prediction error integration
    
    float deltaTime = static_cast<float>(dt);
    pImpl->lastUpdateTime += deltaTime;
    
    // Natural decay towards baseline (dopamine neurons have tonic firing)
    float decayAmount = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Time-dependent decay to prevent indefinite elevation
    if (pImpl->level > pImpl->baseline * 1.1f) {
        float timeDecay = std::exp(-deltaTime / pImpl->timeConstant);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * timeDecay;
    }
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling based on neuroscience
    // Includes: phasic bursts for unexpected rewards, temporal dynamics
    
    // Reset last update time for phasic response
    pImpl->lastUpdateTime = 0.0f;
    
    // Burst response: unexpected rewards cause transient dopamine release
    // Reward magnitude affects burst size
    float burstSize = reward * pImpl->burstGain;
    
    // Dopamine cannot exceed peak
    if (pImpl->level + burstSize > pImpl->peak) {
        burstSize = pImpl->peak - pImpl->level;
    }
    
    // Add burst response (phasic increase)
    pImpl->level += burstSize;
    
    // Cap at peak level
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement real reward prediction error signaling based on neuroscience
    // Includes: prediction error magnitude, sign-dependent responses
    
    // Reset last update time for prediction error response
    pImpl->lastUpdateTime = 0.0f;
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected): dopamine burst
        float burstSize = error * pImpl->predictionErrorGain * 2.0f; // Stronger for positive errors
        pImpl->level += burstSize;
        if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected): dopamine dip
        float dipSize = std::abs(error) * pImpl->predictionErrorGain;
        pImpl->level -= dipSize;
        if (pImpl->level < pImpl->baseline * 0.2f) pImpl->level = pImpl->baseline * 0.2f;
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

} // namespace nlm
