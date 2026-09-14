#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float lastPredictionError;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError(0.0f), lastPredictionError(0.0f) {}
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
    // REAL DOPAMINE-MODULATED PLASTICITY:
    // Dopamine dynamically regulates synaptic plasticity based on behavioral relevance
    // Higher dopamine increases learning rate for rewarded behaviors, decreases for unrewarded
    
    if (pImpl->level < 0.2f) {
        // Low dopamine: Suppress plasticity to prevent noisy learning
        return 0.1f;
    } else if (pImpl->level < 0.5f) {
        // Moderate dopamine: Baseline plasticity
        return 0.5f + 0.5f * pImpl->level;
    } else {
        // High dopamine: Enhance plasticity for rewarded learning
        float enhancement = std::pow(pImpl->level - 0.5f, 2.0f) * 2.0f;  // Stronger enhancement at high levels
        return 1.0f + enhancement;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // REAL DOPAMINE DYNAMICS:
    // Dopamine follows a burst-pause pattern responding to rewards and prediction errors
    // Models phasic dopamine signaling in basal ganglia
    
    // Background dopamine (tonic) - sustained level
    if (pImpl->level < pImpl->baseline) {
        // Gradually recover to baseline when below it
        pImpl->level = std::min(pImpl->baseline, pImpl->level + pImpl->decayRate * static_cast<float>(dt) * 10.0f);
    } else {
        // Decay back to baseline (simulating dopamine neuron firing rate adaptation)
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
    
    // Handle prediction error signal during learning
    if (pImpl->predictionError > 0.0f) {
        // Positive prediction error (better than expected) -> dopamine burst
        float burstAmount = pImpl->predictionError * pImpl->releaseRate * 2.0f;  // Stronger for larger errors
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
        pImpl->predictionError = 0.0f;  // Clear error after processing
    } else if (pImpl->predictionError < 0.0f) {
        // Negative prediction error (worse than expected) -> dopamine dip
        float dipAmount = std::abs(pImpl->predictionError) * pImpl->releaseRate * 1.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipAmount);
        pImpl->predictionError = 0.0f;
    }
}

void Dopamine::signalReward(float reward) {
    // REAL REWARD SIGNALING:
    // Dopamine encodes reward prediction error (temporal difference learning)
    // Phasic bursts occur when rewards are unexpected or larger than predicted
    
    // Calculate prediction error based on reward and current expectation
    float expected = pImpl->level * 0.1f;  // Current expectation (proportional to baseline dopamine)
    float predictionError = reward - expected;
    
    // Scale reward signal based on reward magnitude and valence
    float scaledError = predictionError * pImpl->releaseRate;
    
    // Burst of dopamine on positive reward prediction errors
    if (scaledError > 0.0f) {
        float burst = std::min(pImpl->peak - pImpl->level, scaledError * 2.0f);  // Limit to peak
        pImpl->level += burst;
    } else if (scaledError < 0.0f) {
        // Inhibition for negative prediction errors
        float dip = std::min(pImpl->level - pImpl->baseline, std::abs(scaledError) * 1.5f);
        pImpl->level -= dip;
    }
    
    // Store last prediction error for learning
    pImpl->lastPredictionError = scaledError;
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL PREDICTION ERROR SIGNALING:
    // Direct encoding of reward prediction error signals to modulate synaptic plasticity
    // This is the core of dopamine's role in reinforcement learning
    
    // Scale error by learning rate and current dopamine level
    float scaledError = error * pImpl->releaseRate * (0.1f + pImpl->level * 0.2f);
    
    // Phasic dopamine response to prediction error
    if (scaledError > 0.0f) {
        // Positive prediction error: strengthen synaptic connections
        // Long-lasting potentiation signal
        float potentiation = std::min(pImpl->peak - pImpl->level, scaledError * 1.5f);
        pImpl->level += potentiation * 1.2f;  // Slightly amplified for LTP
    } else if (scaledError < 0.0f) {
        // Negative prediction error: weaken synaptic connections  
        // Transient depression signal
        float depression = std::min(pImpl->level - pImpl->baseline, std::abs(scaledError) * 1.8f);
        pImpl->level -= depression * 0.8f;  // Slightly dampened for LTD
    }
    
    pImpl->lastPredictionError = scaledError;
}

} // namespace nlm
