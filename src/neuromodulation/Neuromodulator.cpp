#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // RPE tracking
    float rewardPredictionError;
    float expectedReward;
    float learningRate;
    
    // Temporal dynamics
    float burstDuration;
    float burstElapsed;
    
    // Exploration-exploitation balance
    float explorationBias;
    float timeSinceLastReward;
    
    // Time constants
    float decayTimeConstant;
    float burstTimeConstant;
    float recoveryTimeConstant;
    
    Impl() : level(0.0f), baseline(0.05f), peak(2.0f), 
             decayRate(0.1f), releaseRate(1.5f),
             rewardPredictionError(0.0f), expectedReward(0.0f),
             learningRate(0.01f), burstDuration(0.5f), burstElapsed(0.0f),
             explorationBias(0.5f), timeSinceLastReward(0.0f),
             decayTimeConstant(1.0f), burstTimeConstant(0.1f),
             recoveryTimeConstant(2.0f) {}
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
    // Based on biological evidence: DA modulates synaptic plasticity
    // LTP at synapses with strong Ca2+ influx, LTD at weak Ca2+ signals
    // Plasticity factor increases with moderate DA levels, decreases at high levels
    float normalizedLevel = pImpl->level / pImpl->peak;
    // Inverted U-shaped relationship typical of dopamine effects
    float plasticity = 0.3f + 0.7f * std::exp(-std::pow(normalizedLevel - 0.5f, 2.0f) / 0.1f);
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics with temporal integration
    float floatDt = static_cast<float>(dt);
    
    // Update burst elapsed time
    pImpl->burstElapsed += floatDt;
    
    // Time since last reward increases
    pImpl->timeSinceLastReward += floatDt;
    
    // Primary decay towards baseline with exponential decay
    float decay = pImpl->decayRate * std::exp(-floatDt / pImpl->decayTimeConstant);
    pImpl->level -= decay;
    
    // Ensure level doesn't go below baseline
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
    
    // Recovery dynamics - gradually restore from baseline
    if (pImpl->level < pImpl->baseline) {
        float recovery = (pImpl->baseline - pImpl->level) * floatDt / pImpl->recoveryTimeConstant;
        pImpl->level += recovery;
    }
    
    // Update RPE tracking for next cycle
    pImpl->rewardPredictionError = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Implement reward prediction error dynamics
    // RPE = reward - expectedReward
    float rpe = reward - pImpl->expectedReward;
    
    // Update expected reward for next prediction using learning
    pImpl->expectedReward += pImpl->learningRate * rpe;
    
    // Store RPE for reference
    pImpl->rewardPredictionError = rpe;
    
    // Burst dynamics - immediate phasic response
    pImpl->burstElapsed = 0.0f;
    
    // Compute new level based on RPE and current level
    float burstComponent = std::max(0.0f, rpe) * pImpl->releaseRate;
    
    // Dopamine neurons show phasic firing for positive RPE (reward prediction error)
    // and dip for negative RPE (prediction error)
    if (rpe > 0.0f) {
        // Positive RPE causes burst firing
        float burst = burstComponent * std::exp(-pImpl->burstElapsed / pImpl->burstTimeConstant);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Update exploration bias - positive RPE reduces exploration
        pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - 0.05f);
    } else {
        // Negative RPE causes dip in activity
        float dip = -std::min(0.0f, rpe) * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level + dip);
        
        // Update exploration bias - negative RPE increases exploration
        pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + 0.05f);
    }
    
    // Reset time since last reward
    pImpl->timeSinceLastReward = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // Directly set RPE for external prediction errors
    pImpl->rewardPredictionError = error;
    
    // Compute dopamine response to prediction error
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        float burst = error * pImpl->releaseRate * 2.0f;  // Stronger response
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - 0.05f);
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected)
        float dip = -error * pImpl->releaseRate * 0.8f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dip);
        pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + 0.05f);
    }
    
    // Update expected reward based on prediction error
    pImpl->expectedReward += error;  // Direct update from prediction error
}

} // namespace nlm
