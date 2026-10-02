#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Biological dopamine parameters
    float level;                  // Current dopamine level (0.0-2.0, can exceed 1.0 for bursts)
    float baseline;               // Tonic firing rate (baseline level)
    float target;                 // Homeostatic target
    float decayRate;              // Rate of return to baseline
    float learningRate;           // Learning rate for prediction error adaptation
    float timeConstant;           // Time constant for dynamics (ms)
    float maxBurst;               // Maximum burst amplitude
    
    // History tracking
    std::vector<float> predictionErrorHistory;
    std::vector<float> rewardHistory;
    float recentPredictionError;
    
    // Internal state for dynamics
    float eligibilityTrace;       // For credit assignment
    float predictionErrorTolerence; // Threshold for significant prediction errors
    
    Impl() : level(0.0f), baseline(0.1f), target(0.5f), decayRate(0.05f), 
             learningRate(0.01f), timeConstant(100.0f), maxBurst(2.0f),
             recentPredictionError(0.0f), eligibilityTrace(0.0f), predictionErrorTolerence(0.1f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxBurst * 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Biological basis: dopamine modulates plasticity through D1/D2 receptor balance
    // D1 receptors (excitatory): enhance LTP when dopamine is high
    // D2 receptors (inhibitory): suppress LTP when dopamine is low
    
    float normalizedLevel = std::clamp(pImpl->level / 1.5f, 0.0f, 2.0f);
    
    // Optimal plasticity occurs at intermediate dopamine levels (inverted U-curve)
    // This reflects the Yerkes-Dodson law of arousal and performance
    if (normalizedLevel < 1.0f) {
        return 0.5f + 0.5f * normalizedLevel;  // Increasing branch
    } else {
        return 2.0f - 0.5f * (normalizedLevel - 1.0f);  // Decreasing branch
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Biological dopamine dynamics: leak towards baseline + homeostatic regulation
    float timeStepMs = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    
    // Leak towards baseline
    float decay = pImpl->decayRate * timeStepMs;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Homeostatic regulation towards target
    float homeostaticForce = pImpl->learningRate * (pImpl->target - pImpl->level);
    pImpl->level += homeostaticForce * timeStepMs;
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= std::pow(0.95f, timeStepMs);
}

void Dopamine::receiveRewardPredictionError(float error) {
    // Phasic dopamine response to reward prediction error
    // Based on Schultz et al. (1997) dopamine neuron recordings
    
    // Calculate burst or dip magnitude
    float errorMagnitude = std::abs(error);
    
    // Only respond to prediction errors above tolerance
    if (errorMagnitude > pImpl->predictionErrorTolerence) {
        // Phasic burst for positive prediction error (unexpected reward)
        // Phasic dip for negative prediction error (unexpected lack of reward)
        if (error > 0.0f) {
            // Reward prediction error: burst of dopamine
            float burstAmplitude = std::min(errorMagnitude, pImpl->maxBurst);
            pImpl->level += burstAmplitude;
            
            // Update eligibility trace for credit assignment
            pImpl->eligibilityTrace = 1.0f;  // Full eligibility for reward prediction error
        } else {
            // Omission prediction error: dip in dopamine
            float dipAmount = std::min(errorMagnitude, pImpl->maxBurst * 0.5f);
            pImpl->level = std::max(0.0f, pImpl->level - dipAmount);
            
            // Partial eligibility trace for omission errors
            pImpl->eligibilityTrace *= 0.5f;
        }
    }
    
    // Record prediction error history
    pImpl->predictionErrorHistory.push_back(error);
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    pImpl->recentPredictionError = error;
}

void Dopamine::receiveReward(float reward) {
    // Direct reward signaling (complementary to prediction error)
    // Can be used for non-prediction-error based reward processing
    
    // Smooth integration of reward signal
    float integratedReward = reward * pImpl->learningRate;
    
    // Boost dopamine level
    pImpl->level += integratedReward;
    
    // Record reward history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

} // namespace nlm
