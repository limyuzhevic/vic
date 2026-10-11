#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float eligibilityTrace;
    float synapticModulation;
    float adaptiveResilience;
    float predictionErrorHistory;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             eligibilityTrace(0.0f), synapticModulation(1.0f), adaptiveResilience(0.0f),
             predictionErrorHistory(0.0f) {}
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
    // PHASE 2: Implement real dopamine-modulated plasticity factor
    // More realistic: Use adaptive resilience and eligibility traces
    
    // Base plasticity factor
    float plasticityFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply adaptive resilience - learning adapts over time
    plasticityFactor *= (1.0f + pImpl->adaptiveResilience * 0.2f);
    
    // Apply synaptic modulation from eligibility traces
    plasticityFactor *= pImpl->synapticModulation;
    
    // Clamp to reasonable range
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2: Implement real dopamine dynamics with more biological realism
    
    float dtf = static_cast<float>(dt);
    
    // Decay towards baseline (longer decay time)
    float decayAmount = pImpl->decayRate * dtf;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - pImpl->decayRate * dtf);
    
    // Decay synaptic modulation
    pImpl->synapticModulation *= (1.0f - pImpl->decayRate * 0.5f * dtf);
    
    // Update adaptive resilience (slower adaptation)
    pImpl->adaptiveResilience *= (1.0f - 0.01f * dtf);
    
    NLM_LOG_DEBUG("Dopamine update: level=" + std::to_string(pImpl->level) + 
                  ", eligibility=" + std::to_string(pImpl->eligibilityTrace));
}

void Dopamine::signalReward(float reward) {
    // PHASE 2: Implement real reward signaling with eligibility traces
    
    // Immediate reward response
    float immediateResponse = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->level = immediateResponse;
    
    // Create eligibility trace for future synaptic modulation
    // Eligibility traces mark synapses that are ready for plasticity
    pImpl->eligibilityTrace = 0.5f * (1.0f + std::tanh(reward * 2.0f));
    
    // Apply synaptic modulation for reward prediction error
    pImpl->synapticModulation = 0.8f + 0.2f * std::abs(reward);
    
    // Update adaptive resilience (faster adaptation to new rewards)
    pImpl->adaptiveResilience = std::min(1.0f, pImpl->adaptiveResilience + 0.1f * reward);
    
    NLM_LOG_INFO("Dopamine reward signal: reward=" + std::to_string(reward) + 
                 ", level=" + std::to_string(pImpl->level) + 
                 ", eligibility=" + std::to_string(pImpl->eligibilityTrace));
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2: Implement reward prediction error signaling with biological realism
    
    // Reward prediction error follows dopamine dynamics from neuroscience
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    
    // Scale error to realistic range
    float scaledError = error * 0.5f;  // Scale to [-0.5, 0.5]
    
    // Compute change based on prediction error
    float errorEffect = scaledError * pImpl->releaseRate * 1.5f;
    
    // Update dopamine level with error
    pImpl->level = std::max(0.0f, std::min(pImpl->peak, 
                                          pImpl->level + errorEffect));
    
    // Update eligibility trace based on prediction error
    pImpl->eligibilityTrace = 0.3f * (1.0f + std::tanh(error * 3.0f));
    
    // Update synaptic modulation
    pImpl->synapticModulation = 0.6f + 0.4f * (0.5f + 0.5f * std::tanh(error));
    
    // Store prediction error for future learning
    pImpl->predictionErrorHistory = 0.9f * pImpl->predictionErrorHistory + 
                                   0.1f * error;
    
    // Adapt baseline towards recent prediction errors
    pImpl->baseline = 0.95f * pImpl->baseline + 0.05f * std::abs(error);
    
    NLM_LOG_INFO("Dopamine prediction error: error=" + std::to_string(error) + 
                 ", level=" + std::to_string(pImpl->level) + 
                 ", eligibility=" + std::to_string(pImpl->eligibilityTrace));
}

} // namespace nlm
