#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Dopamine dynamics
    float lastUpdateTime;
    float integrationFactor;
    float burstFactor;
    float adaptationFactor;
    
    // Learning signal components
    float rewardPredictionError;
    float noveltySignal;
    float curiositySignal;
    
    // Plasticity modulation
    float plasticityBaseline;
    float plasticityMax;
    float plasticityExponent;
    float eligibilityTrace;
    
    // Temporal dynamics
    float phasicLevel;
    float tonicLevel;
    float historyAlpha;  // Smoothing factor for history
    
    // State tracking for dynamic updates
    float targetLevel;
    float timeToTarget;
    float minInterval;
    float maxInterval;
    
    Impl() : 
        level(0.0f), baseline(0.2f), peak(1.0f), 
        decayRate(0.1f), releaseRate(1.0f),
        lastUpdateTime(0.0f), integrationFactor(0.9f), burstFactor(2.0f),
        adaptationFactor(0.05f), rewardPredictionError(0.0f),
        noveltySignal(0.0f), curiositySignal(0.0f),
        plasticityBaseline(1.0f), plasticityMax(2.0f), plasticityExponent(1.5f),
        eligibilityTrace(0.0f), phasicLevel(0.0f), tonicLevel(baseline),
        historyAlpha(0.9f), targetLevel(baseline), timeToTarget(0.0f),
        minInterval(0.01f), maxInterval(10.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components
    float weightedLevel = tonicLevel * 0.7f + phasicLevel * 0.3f;
    
    // Apply eligibility trace modulation
    weightedLevel *= (1.0f + eligibilityTrace * 0.1f);
    
    // Clamp to reasonable range
    return std::clamp(weightedLevel, 0.0f, 1.0f);
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = level;
    pImpl->timeToTarget = maxInterval * 0.1f; // Quick transition
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Based on mean field theory and biological findings
    
    float dopamineLevel = getLevel();
    
    // Nonlinear relationship: bell-shaped curve
    // Low dopamine (<0.3): reduced plasticity
    // Optimal dopamine (~0.5-0.7): enhanced plasticity
    // High dopamine (>0.8): reduced plasticity due to saturation
    
    float optimalCenter = 0.6f;
    float optimalWidth = 0.2f;
    
    float plasticity = plasticityBaseline;
    if (dopamineLevel > 0.3f && dopamineLevel < 0.9f) {
        float distanceFromOptimal = std::abs(dopamineLevel - optimalCenter);
        float optimalBoost = std::exp(-distanceFromOptimal * distanceFromOptimal / (optimalWidth * optimalWidth));
        plasticity = plasticityBaseline + (plasticityMax - plasticityBaseline) * optimalBoost;
    }
    
    // Apply eligibility trace modulation
    float eligibilityMod = 1.0f + eligibilityTrace * 0.2f;
    plasticity *= eligibilityMod;
    
    // Apply phasic burst modulation
    float burstMod = 1.0f + phasicLevel * 0.5f;
    plasticity *= burstMod;
    
    return std::clamp(plasticity, 0.1f, 10.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    
    // Time-dependent update
    pImpl->lastUpdateTime += dt;
    
    // Decay towards baseline with biological realism
    float timeSinceLastUpdate = pImpl->lastUpdateTime - pImpl->timeToTarget;
    if (timeSinceLastUpdate > 0.0f) {
        // Exponential decay with time constant
        float decayFactor = std::exp(-pImpl->decayRate * timeSinceLastUpdate);
        tonicLevel = baseline + (tonicLevel - baseline) * decayFactor;
        
        // Smooth the decay
        tonicLevel = tonicLevel * pImpl->historyAlpha + pImpl->targetLevel * (1.0f - pImpl->historyAlpha);
    }
    
    // Phasic component dynamics
    if (phasicLevel > 0.0f) {
        // Phasic bursts decay faster
        float burstDecay = std::exp(-2.0f * pImpl->decayRate * dt);
        phasicLevel *= burstDecay;
    }
    
    // Update eligibility trace (slow decay)
    eligibilityTrace *= std::exp(-0.01f * dt);
    
    // Apply reward prediction error
    if (pImpl->rewardPredictionError != 0.0f) {
        // Phasic dopamine response to RPE
        float rpeBurst = std::clamp(std::abs(pImpl->rewardPredictionError) * 2.0f, 0.0f, 1.0f);
        
        if (pImpl->rewardPredictionError > 0.0f) {
            // Positive RPE: dopamine burst
            phasicLevel += rpeBurst * burstFactor;
            phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
        } else {
            // Negative RPE: dopamine dip
            phasicLevel -= rpeBurst * 0.5f;
            phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
        }
    }
    
    // Apply novelty and curiosity signals
    if (pImpl->noveltySignal > 0.0f) {
        phasicLevel += pImpl->noveltySignal * 0.5f;
        phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
    }
    
    if (pImpl->curiositySignal > 0.0f) {
        phasicLevel += pImpl->curiositySignal * 0.3f;
        phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
    }
    
    // Integrate tonic and phasic components
    level = getLevel();
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics
    // Based on midbrain dopamine neuron responses
    
    // Reward magnitude coding
    float rewardMagnitude = std::tanh(reward * 2.0f); // Normalize reward
    
    // Phasic burst proportional to reward magnitude
    float burstSize = std::min(rewardMagnitude * burstFactor, 1.0f);
    
    // Add to phasic component
    phasicLevel += burstSize;
    phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
    
    // Update eligibility trace for reinforcement learning
    eligibilityTrace = std::min(eligibilityTrace + rewardMagnitude * 0.1f, 1.0f);
    
    // Record for RPE computation
    rewardPredictionError = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Core component of temporal difference learning
    
    // RPE coding: larger magnitude for prediction errors
    float rpeMagnitude = std::tanh(error * 2.0f);
    
    // RPE has stronger phasic effect than reward alone
    float rpeEffect = rpeMagnitude * burstFactor * 1.5f;
    
    if (error > 0.0f) {
        // Positive RPE: dopamine burst (learning signal)
        phasicLevel += rpeEffect;
        phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
    } else {
        // Negative RPE: temporary suppression (prediction error)
        phasicLevel -= rpeEffect * 0.5f;
        phasicLevel = std::clamp(phasicLevel, 0.0f, 1.0f);
    }
    
    // Update internal RPE state
    rewardPredictionError = error;
}

} // namespace nlm
