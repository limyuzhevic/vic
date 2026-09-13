#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics
    float level;                    // Current concentration (0-1 normalized)
    float tonic;                    // Tonic firing baseline level
    float phasic;                   // Phasic response component
    float predictionError;          // Reward prediction error (RPE)
    float referenceSignal;          // Expected reward (reference)
    float baseline;                 // Baseline concentration
    float peak;                     // Maximum concentration
    float decayRate;                // Decay rate (tonic)
    float riseRate;                 // Rise rate (phasic)
    float adaptationRate;           // Adaptation to maintain homeostasis
    
    // Temporal difference learning components
    float eligibilityTrace;         // TD learning eligibility trace
    float learningRate;             // TD learning rate (α)
    float discountFactor;           // Temporal discount factor (γ)
    
    // Integration state
    float intensity;                // Current intensity for plasticity
    float plasticityFactor;         // Modulated plasticity factor
    
    // History for computation
    std::vector<float> rewardHistory;
    std::vector<float> predictionHistory;
    std::vector<float> errorHistory;
    
    // Time tracking
    float timeSinceLastReward;
    float timeSinceLastPrediction;
    
    Impl() 
        : level(0.0f), tonic(0.05f), phasic(0.0f), predictionError(0.0f), referenceSignal(0.0f)
        , baseline(0.05f), peak(1.0f), decayRate(0.05f), riseRate(1.0f), adaptationRate(0.01f)
        , eligibilityTrace(0.0f), learningRate(0.1f), discountFactor(0.95f)
        , intensity(0.0f), plasticityFactor(1.0f)
        , timeSinceLastReward(0.0f), timeSinceLastPrediction(0.0f) {}
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
    // Biological model: Dopamine modulates plasticity through concentration-dependent effects
    // Based on: Diamond & Ambler (2005), "Neurotransmitters in the regulation of synaptic
    // plasticity in CNS and PNS" - D1/D5 receptors increase cAMP -> L-LTP
    // D2/D3 receptors decrease cAMP -> LTD
    
    // Normalize concentration to effect range [0.1, 2.0]
    float normalizedLevel = pImpl->level * 2.0f - 0.5f;
    normalizedLevel = std::clamp(normalizedLevel, 0.1f, 2.0f);
    
    // Apply sigmoidal transformation for saturation effects
    float sigmoid = 1.0f / (1.0f + std::exp(-10.0f * (normalizedLevel - 1.0f)));
    
    // Base plasticity + dopamine modulation
    float factor = 1.0f + 0.8f * sigmoid;
    
    // Incorporate prediction error magnitude for adaptive modulation
    float errorMagnitude = std::abs(pImpl->predictionError);
    factor *= (1.0f + 0.5f * errorMagnitude);
    
    // Bounds check
    factor = std::clamp(factor, 0.1f, 2.0f);
    
    pImpl->plasticityFactor = factor;
    return factor;
}

void Dopamine::update(TimestepDuration dt) {
    // Biological dopamine dynamics: Combines tonic baseline with phasic bursts
    // Model inspired by: Schultz et al. (1997) - dopamine neurons in reward prediction
    
    // Update time since last events
    pImpl->timeSinceLastReward += static_cast<float>(dt);
    pImpl->timeSinceLastPrediction += static_cast<float>(dt);
    
    // Tonic decay (slow, automatic) - tonic represents baseline firing rate
    if (pImpl->timeSinceLastReward > 1000.0f) {  // 1 second threshold
        // Homeostatic adaptation towards baseline
        pImpl->tonic = std::max(pImpl->baseline, pImpl->tonic - pImpl->adaptationRate * static_cast<float>(dt));
    }
    
    // Phasic decay (fast, burst-related)
    pImpl->phasic = std::max(0.0f, pImpl->phasic - pImpl->decayRate * static_cast<float>(dt));
    
    // Total dopamine concentration = tonic + phasic
    // Clamp to physiological bounds [0.0, 1.0]
    pImpl->level = std::clamp(pImpl->tonic + pImpl->phasic, 0.0f, 1.0f);
    
    // Update eligibility trace for TD learning (decay with time)
    if (pImpl->eligibilityTrace > 0.0f) {
        pImpl->eligibilityTrace *= (1.0f - 0.001f * static_cast<float>(dt));  // exponential decay
    }
}

void Dopamine::signalReward(float reward) {
    // Biological reward signaling: Phasic burst proportional to reward prediction error
    // Model inspired by: Schultz (1998) - dopamine neurons respond to unexpected rewards
    // 
    // Compute reward prediction error: RPE = actual_reward - expected_reward
    float rewardPredictionError = reward - pImpl->referenceSignal;
    pImpl->predictionError = rewardPredictionError;
    
    // Update time since last reward
    pImpl->timeSinceLastReward = 0.0f;
    
    // Phasic response magnitude (closer to true RPE, with saturation)
    pImpl->phasic = std::clamp(rewardPredictionError * pImpl->riseRate, 0.0f, pImpl->peak);
    
    // Add to reward history for tracking
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Learning: Update expected reward (temporal difference learning)
    pImpl->referenceSignal = pImpl->referenceSignal + pImpl->learningRate * rewardPredictionError;
    pImpl->referenceSignal = std::clamp(pImpl->referenceSignal, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Biological reward prediction error signaling
    // Model inspired by: Sutton & Barto (1998) - Temporal Difference Learning
    // Dopamine neurons encode reward prediction errors (RPE): RPE = actual - predicted
    // 
    // Error can be positive (better than expected) or negative (worse than expected)
    pImpl->predictionError = error;
    
    // Update time since last prediction
    pImpl->timeSinceLastPrediction = 0.0f;
    
    // Phasic response to prediction error (RPE is what drives dopamine)
    // Magnitude scaled by confidence and learning rate
    float magnitude = std::abs(error) * pImpl->learningRate;
    pImpl->phasic = std::clamp(error * magnitude, -pImpl->peak, pImpl->peak);
    
    // Add to error history
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update eligibility trace for TD learning: trace += state * learning_rate
    pImpl->eligibilityTrace = pImpl->eligibilityTrace + error * pImpl->learningRate * static_cast<float>(0.1);
    
    // Apply homeostatic adaptation to maintain dopamine levels within physiological bounds
    if (std::abs(error) > 0.5f) {
        // Strong error triggers homeostatic adaptation
        pImpl->adaptationRate = std::min(0.1f, pImpl->adaptationRate * 1.1f);
    } else {
        // Weak error promotes stability
        pImpl->adaptationRate = std::max(0.001f, pImpl->adaptationRate * 0.9f);
    }
}

} // namespace nlm
