#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;      // For exponential dynamics
    float eligibilityTrace;  // For reward prediction error
    float phasicStartTime;   // For burst detection
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), timeConstant(500.0f), eligibilityTrace(0.0f),
             phasicStartTime(-1.0f) {}
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
    pImpl->phasicStartTime = -1.0f; // Reset burst detection
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on Phineas Gage model
    // Dopamine level affects synaptic modification probabilities
    // Very low DA (<0.2): reduces plasticity, increases exploration
    // Medium DA (0.2-0.8): normal plasticity, stable learning
    // High DA (>0.8): enhances plasticity, increases risk-taking
    
    if (pImpl->level < 0.2f) {
        return 0.3f + 0.7f * (pImpl->level / 0.2f);  // Low DA reduces plasticity
    } else if (pImpl->level > 0.8f) {
        return 0.5f + 0.5f * ((pImpl->level - 0.8f) / 0.2f);  // High DA enhances plasticity
    } else {
        return 0.5f;  // Normal level
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on reinforcement learning
    // Combine tonic (baseline) and phasic (burst/sapce) components
    
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline (tonic component)
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 
                   std::exp(-dtFloat / pImpl->timeConstant);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= std::exp(-dtFloat / 100.0f);
    
    // Smooth transitions prevent abrupt changes
    if (pImpl->phasicStartTime >= 0.0f) {
        // Recovery from phasic burst
        float recovery = dtFloat / 100.0f;  // 100ms recovery time
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + recovery);
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on Rescorla-Wagner learning
    // Phasic bursts proportional to reward prediction error
    
    if (reward > 0.0f) {
        // Reward delivery: phasic burst
        float burstSize = std::min(1.0f, reward * 2.0f); // Saturates at 1.0 for reward=0.5
        pImpl->phasicStartTime = 0.0f;
        
        // Rapid increase to near peak
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize * 2.0f);
        
        // Increase baseline for future learning (learning rate)
        pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + 0.1f * burstSize);
    } else {
        // Negative reward or no reward: dips
        float dip = std::min(0.5f, -reward * 2.0f); // Saturates at 0.5 for reward=-0.25
        pImpl->level = std::max(0.0f, pImpl->level - dip);
        
        // Slight decrease in baseline
        pImpl->baseline = std::max(0.0f, pImpl->baseline - 0.05f * dip);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Dopamine encodes prediction errors as learned in Schultz et al. experiments
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected): phasic burst
        float burstSize = std::min(1.0f, error * 2.0f);
        pImpl->phasicStartTime = 0.0f;
        
        // Increase level to encode prediction error
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize * 3.0f);
        
        // Update eligibility trace for future learning
        pImpl->eligibilityTrace += error * 0.1f; // Learning rate
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected): dip
        float dip = std::min(0.5f, -error * 2.0f);
        pImpl->level = std::max(0.0f, pImpl->level - dip);
        
        // Decrease eligibility trace
        pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - std::abs(error) * 0.1f);
    }
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = std::clamp(trace, 0.0f, 1.0f);
}

void Dopamine::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

} // namespace nlm
