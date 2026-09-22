#pragma once

#include "Neuromodulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

namespace nlm {

// Dopamine implementation with full dynamics (phasic/tonic, eligibility traces, reward-modulated STDP)
struct Dopamine::Impl {
    float level;                    // Current tonic level
    float phasicBurst;              // Current phasic burst
    float baseline;                 // Baseline dopamine level
    float peak;                     // Peak response level
    float decayRate;                // Tonic decay rate
    float phasicDecay;              // Phasic decay rate
    float eligibilityTrace;         // Reward-modulated STDP eligibility trace
    float learningRate;             // Dopamine-dependent learning rate
    float stdpLtpModulation;        // Modulation of LTP by dopamine
    float stdpLtdModulation;        // Modulation of LTD by dopamine
    std::queue<float> rewardHistory; // Recent rewards for eligibility trace updates
    SimulationStep lastRewardStep;   // When last reward was received
    float predictionError;          // Reward prediction error
    
    Impl() : level(0.0f), phasicBurst(0.0f), baseline(0.1f), peak(1.5f), 
             decayRate(0.05f), phasicDecay(0.2f), eligibilityTrace(0.0f),
             learningRate(0.01f), stdpLtpModulation(1.0f), stdpLtdModulation(1.0f),
             lastRewardStep(0), predictionError(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() {
    delete pImpl;
}

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
    // Combined effect of tonic and phasic dopamine on plasticity
    float tonicEffect = 0.5f + 0.5f * pImpl->level;  // Tonic modulates baseline plasticity
    float phasicEffect = 1.0f + 2.0f * pImpl->phasicBurst; // Phasic strongly enhances plasticity
    return tonicEffect * phasicEffect;
}

void Dopamine::update(TimestepDuration dt) {
    // Update tonic level with decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay phasic burst
    pImpl->phasicBurst = std::max(0.0f, pImpl->phasicBurst - pImpl->phasicDecay * static_cast<float>(dt));
    
    // Update eligibility trace (decays over time)
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - 0.01f * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Phasic burst on reward delivery
    pImpl->phasicBurst = std::min(pImpl->peak, pImpl->phasicBurst + reward * pImpl->phasicDecay);
    
    // Increase tonic level slightly
    pImpl->level = std::min(1.0f, pImpl->level + reward * 0.1f);
    
    // Update eligibility trace for reward-modulated STDP
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + reward * 0.5f);
    
    // Store reward for eligibility trace updates
    pImpl->rewardHistory.push(reward);
    if (pImpl->rewardHistory.size() > 10) {
        pImpl->rewardHistory.pop();
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic response to prediction error (positive for surprise, negative for disappointment)
    pImpl->phasicBurst = std::min(pImpl->peak, pImpl->phasicBurst + std::abs(error) * pImpl->phasicDecay);
    
    // Adjust tonic level based on error magnitude
    if (error > 0) { // Better than expected - increase tonic level
        pImpl->level = std::min(1.0f, pImpl->level + error * 0.05f);
    } else { // Worse than expected - decrease tonic level
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * 0.1f);
    }
    
    // Update eligibility trace for reward prediction error
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace + error * 0.3f);
    pImpl->predictionError = error;
}

void Dopamine::updateSTDPWithEligibility(float error) {
    // Reward-modulated STDP eligibility trace implementation
    // This would update synaptic eligibility traces based on prediction errors
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace + error * 0.2f);
}

} // namespace nlm
