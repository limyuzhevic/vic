#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib> // For rand()

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float integrationTimeConstant;
    float predictionErrorSignal;
    float rewardHistory[10];  // Rolling window of recent rewards
    int rewardHistoryIndex;
    int rewardHistorySize;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.5f), decayRate(0.05f), 
             releaseRate(2.0f), integrationTimeConstant(0.1f),
             predictionErrorSignal(0.0f), rewardHistoryIndex(0), rewardHistorySize(0) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
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
    // Real dopamine-modulated plasticity factor based on concentration and dynamics
    // Uses asymmetric response: phasic dopamine enhances LTP, tonic dopamine modulates baseline
    float concentration = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    
    // Exponential function for more realistic plasticity modulation
    // Low dopamine: baseline plasticity with suppression
    // High dopamine: enhanced plasticity for learning
    float plasticity = 0.1f + 0.8f * std::pow(concentration, 2.0f);
    
    // Incorporate prediction error signal for adaptive modulation
    plasticity *= (1.0f + 0.3f * std::clamp(pImpl->predictionErrorSignal, -1.0f, 1.0f));
    
    return std::clamp(plasticity, 0.05f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Real dopamine dynamics: three-component model (phasic, tonic, background)
    
    // 1. Tonic decay towards baseline
    float tonicComponent = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * timeStep);
    tonicComponent += (pImpl->level - tonicComponent) * (1.0f - decayFactor);
    
    // 2. Phasic responses to rewards and prediction errors
    float phasicResponse = 0.0f;
    
    // Integrate recent rewards for prediction error computation
    if (pImpl->rewardHistorySize > 0) {
        float avgReward = 0.0f;
        for (int i = 0; i < pImpl->rewardHistorySize; ++i) {
            avgReward += pImpl->rewardHistory[i];
        }
        avgReward /= pImpl->rewardHistorySize;
        
        // Compute reward prediction error (current level vs expected)
        float expected = avgReward * 0.8f;  // Learning rate for expected value
        pImpl->predictionErrorSignal = (tonicComponent + phasicResponse) - expected;
        
        // Modulate phasic response based on prediction error
        if (std::abs(pImpl->predictionErrorSignal) > 0.01f) {
            phasicResponse += pImpl->predictionErrorSignal * pImpl->releaseRate * timeStep;
        }
    }
    
    // 3. Background dynamics and noise
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
    
    // Combine components with appropriate weights
    pImpl->level = tonicComponent + phasicResponse + noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.5f); // Allow for peak > 1.0 for phasic bursts
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic dopamine burst proportional to reward prediction error
    // Follows temporal difference learning principles
    
    // Store in reward history for prediction error computation
    pImpl->rewardHistory[pImpl->rewardHistoryIndex] = reward;
    pImpl->rewardHistoryIndex = (pImpl->rewardHistoryIndex + 1) % 10;
    if (pImpl->rewardHistorySize < 10) pImpl->rewardHistorySize++;
    
    // Compute reward prediction error relative to baseline expectation
    float expectedReward = pImpl->baseline * 0.5f; // Baseline expectation
    float predictionError = reward - expectedReward;
    
    // Phasic dopamine burst: larger for unexpected rewards
    float burstMagnitude = std::min(1.0f, std::abs(predictionError) * 2.0f);
    
    if (predictionError > 0) {
        // Positive prediction error (better than expected): dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude * pImpl->releaseRate);
    } else {
        // Negative prediction error (worse than expected): dopamine dip
        pImpl->level = std::max(0.0f, pImpl->level + burstMagnitude * pImpl->releaseRate * 0.5f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling: dopaminergic response to mismatches
    // Implements Rescorla-Wagner learning rule within dopamine dynamics
    
    pImpl->predictionErrorSignal = error;
    
    // Phasic adjustment proportional to prediction error magnitude
    float phasicAdjustment = std::clamp(error, -0.5f, 0.5f) * pImpl->releaseRate;
    
    // Update level with asymmetric response
    if (error > 0) {
        // Positive error (higher than expected): reinforcing burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + phasicAdjustment);
    } else {
        // Negative error (lower than expected): suppressive response
        pImpl->level = std::max(0.0f, pImpl->level + phasicAdjustment * 0.7f);
    }
}

} // namespace nlm
