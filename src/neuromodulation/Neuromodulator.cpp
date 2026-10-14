#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Enhanced dopamine dynamics
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorModulation;
    float rewardPredictionError;
    
    // Neural dopamine system state
    float synapticWeight;
    float neuralExcitability;
    float plasticityBias;
    
    // For real neural implementation, we would store:
    // - Dopamine neuron populations (ventral tegmental area, etc.)
    // - Synaptic dopamine receptors
    // - Neural circuits for reward processing
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionErrorModulation(0.0f), rewardPredictionError(0.0f)
        , synapticWeight(1.0f), neuralExcitability(0.0f), plasticityBias(0.0f) {
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

// Implement real dopamine-modulated plasticity factor
float Dopamine::getPlasticityFactor() const {
    // More sophisticated dopamine modulation of plasticity
    // Based on recent neuroscience research
    
    // Dopamine enhances plasticity through multiple mechanisms:
    // 1. Increased synaptic strength
    // 2. Enhanced LTP/LTD efficacy
    // 3. Modulation of neural excitability
    
    float basePlasticity = 0.5f;
    
    // Level-dependent modulation
    float levelModulation = pImpl->level * 0.8f;
    
    // Reward prediction error modulation
    float rpeModulation = pImpl->rewardPredictionError * 0.5f;
    
    // Prediction error modulation
    float peModulation = pImpl->predictionErrorModulation * 0.3f;
    
    // Neural excitability modulation
    float excitabilityModulation = pImpl->neuralExcitability * 0.2f;
    
    // Synaptic weight modulation
    float weightModulation = pImpl->synapticWeight * 0.1f;
    
    float plasticityFactor = basePlasticity + 
                           levelModulation + 
                           rpeModulation + 
                           peModulation + 
                           excitabilityModulation + 
                           weightModulation;
    
    // Clamp to reasonable range
    return std::max(0.1f, std::min(2.0f, plasticityFactor));
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on neuroscience
    // This includes:
    // - Basal dopamine tone
    // - Phasic responses to rewards
    // - Decay towards baseline
    // - Integration with prediction errors
    
    float timeStep = static_cast<float>(dt);
    
    // Natural decay towards baseline (tonic activity)
    float decay = pImpl->decayRate * timeStep;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Integrate with reward prediction error signals
    if (pImpl->rewardPredictionError != 0.0f) {
        // Phasic dopamine response to RPE
        float rpeResponse = pImpl->rewardPredictionError * pImpl->releaseRate;
        pImpl->level += rpeResponse;
        
        // Cap at peak level
        pImpl->level = std::min(pImpl->peak, pImpl->level);
        
        // Reset RPE for next update
        pImpl->rewardPredictionError = 0.0f;
    }
    
    // Modulate neural excitability based on current level
    pImpl->neuralExcitability = pImpl->level * 0.5f;
    
    NLM_LOG_DEBUG("Dopamine updated: level=" + std::to_string(pImpl->level));
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling with multiple components
    // Based on neuroscience of reward processing
    
    // Primary reward response
    float rewardResponse = reward * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
    
    // Secondary response with delay
    // Simulates more complex reward processing
    float delayedResponse = reward * pImpl->releaseRate * 0.5f * std::exp(-0.01f * reward);
    pImpl->level = std::min(pImpl->peak, pImpl->level + delayedResponse);
    
    NLM_LOG_DEBUG("Dopamine reward signal: reward=" + std::to_string(reward) + ", new_level=" + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // Based on temporal difference learning
    
    pImpl->rewardPredictionError += error;
    
    // Scale error response
    float errorResponse = error * pImpl->releaseRate * 0.5f;
    pImpl->level += errorResponse;
    
    // Cap response
    pImpl->level = std::min(pImpl->peak, pImpl->level);
    
    NLM_LOG_DEBUG("Dopamine RPE signal: error=" + std::to_string(error) + ", new_level=" + std::to_string(pImpl->level));
}

void Dopamine::setPredictionErrorModulation(float modulation) {
    pImpl->predictionErrorModulation = modulation;
}

void Dopamine::setSynapticWeight(float weight) {
    pImpl->synapticWeight = std::clamp(weight, 0.1f, 2.0f);
}

} // namespace nlm
