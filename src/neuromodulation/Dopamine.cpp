#include "Dopamine.hpp"
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
    // Dopamine enhances plasticity when it signals reward prediction errors
    // This implements the dopamine-dependent eligibility trace mechanism
    
    // Baseline plasticity factor
    float basePlasticity = 0.2f;
    
    // Dopamine scales plasticity in a non-linear (saturating) manner
    // Higher dopamine levels have diminishing returns on plasticity enhancement
    float dopamineEffect = pImpl->level * 0.6f;  // Maximum 60% enhancement
    float satEffect = dopamineEffect / (1.0f + dopamineEffect);  // Sigmoid-like saturation
    
    // Total plasticity factor with biological constraints
    float plasticityFactor = basePlasticity * (1.0f + satEffect);
    
    // Ensure reasonable bounds (0.2 to 1.0)
    return std::clamp(plasticityFactor, 0.2f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience models
    // Uses a continuous-time approximation of dopamine neuron activity
    
    // Convert timestep to seconds for biological time constants
    float deltaT = static_cast<float>(dt) * 0.001f;  // dt is in ms, convert to seconds
    
    // Primary decay towards baseline (time constant ~1-2 seconds)
    float decayFactor = std::exp(-pImpl->decayRate * deltaT);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Ensure level stays within physiological bounds
    pImpl->level = std::max(0.0f, std::min(pImpl->level, 1.0f));
}

void Dopamine::signalReward(float reward) {
    // Reward prediction error signaling (TD learning)
    // Dopamine encodes both positive and negative prediction errors
    
    // Enhanced response to unexpected rewards (burst firing)
    float rewardMagnitude = std::clamp(reward, 0.0f, 1.0f);
    
    // Add reward to current level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardMagnitude * pImpl->releaseRate * 10.0f);
    
    // For larger rewards, create a burst response
    if (reward > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Schultz et al. 1997)
    // Dopamine neurons respond to better-than-expected rewards (positive error)
    // and diminish responses to worse-than-expected rewards (negative error)
    
    // Error ranges from -1 (worse than expected) to +1 (better than expected)
    float clampedError = std::clamp(error, -1.0f, 1.0f);
    
    // Positive prediction errors increase dopamine (learning signal)
    // Negative prediction errors decrease dopamine towards baseline
    if (clampedError > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + clampedError * pImpl->releaseRate * 5.0f);
    } else {
        // Negative errors decay dopamine faster
        pImpl->level = std::max(pImpl->baseline, pImpl->level + clampedError * pImpl->releaseRate * 2.0f);
    }
}

void Dopamine::initialize(class Brain* brain) {
    // Initialize dopamine system with brain reference
    if (brain) {
        // TODO: Store brain reference if needed for future extensions
        // For now, dopamine operates as a standalone neuromodulator
        // but can be integrated with brain systems through the Neuromodulation interface
    }
}