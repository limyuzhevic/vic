#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;               // Current dopamine level
    float baseline;            // Tonic baseline level
    float peak;                // Maximum level for phasic responses
    float decayRate;           // Rate of decay toward baseline
    float recoveryRate;        // Rate of recovery after depletion
    float phasicGain;          // Sensitivity to prediction errors
    float adaptationTimeConstant;  // Time constant for adaptation
    float predictedValue;      // Predicted reward value
    float predictionError;     // Reward prediction error (RPE)
    
    // Time tracking
    Timestamp lastPhasicEvent;
    Timestamp lastRPEvent;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(2.0f)
        , decayRate(0.2f)
        , recoveryRate(0.1f)
        , phasicGain(0.8f)
        , adaptationTimeConstant(1.0f)
        , predictedValue(0.0f)
        , predictionError(0.0f)
        , lastPhasicEvent(-1.0f)
        , lastRPEvent(-1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak * 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on Phasic dopamine
    // Phasic dopamine enhances plasticity for learning, tonic modulates baseline
    float phasicComponent = pImpl->predictionError * pImpl->phasicGain;
    float tonicComponent = pImpl->level * 0.1f;  // Tonic modulates baseline plasticity
    
    // Combined effect: prediction errors strongly influence plasticity
    // Tonic dopamine provides baseline modulation
    float plasticity = 0.5f + tonicComponent + phasicComponent * 0.8f;
    
    // Clamp to reasonable range
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    Timestamp currentTime = pImpl->lastRPEvent + dt;
    
    // Decay toward baseline (tonic activity)
    float decayTowardsBaseline = pImpl->baseline * pImpl->decayRate;
    float currentDecay = pImpl->level * (1.0f - pImpl->decayRate);
    
    // Recovery from depletion
    float recoveryTowardsBaseline = pImpl->baseline * pImpl->recoveryRate;
    float currentRecovery = (pImpl->baseline - pImpl->level) * pImpl->recoveryRate;
    
    // Combined update
    pImpl->level += currentRecovery - currentDecay;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak * 2.0f);
    
    // Adaptation of predicted value based on recent prediction errors
    float adaptation = pImpl->predictionError * (1.0f / pImpl->adaptationTimeConstant);
    pImpl->predictedValue += adaptation * static_cast<float>(dt);
    
    pImpl->lastPhasicEvent = currentTime;
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst for unexpected reward (positive RPE)
    float predictionError = std::max(0.0f, reward - pImpl->predictedValue);
    
    if (predictionError > 0.0f) {
        // Phasic burst proportional to prediction error
        float burst = predictionError * pImpl->phasicGain;
        
        // Exponential growth to peak
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Record the event
        pImpl->lastRPEvent = pImpl->lastPhasicEvent;
        pImpl->predictionError = predictionError;
        
        NLM_LOG_INFO(std::string("Dopamine phasic burst: ") + std::to_string(burst));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic response to reward prediction errors
    pImpl->predictionError = error;
    
    if (std::abs(error) > 0.0f) {
        // Phasic response: burst for positive error, dip for negative error
        float response = error * pImpl->phasicGain;
        
        if (error > 0.0f) {
            // Positive RPE: dopamine burst
            pImpl->level = std::min(pImpl->peak, pImpl->level + response * 0.5f);
        } else {
            // Negative RPE: temporary dip below baseline
            pImpl->level = std::max(0.0f, pImpl->level + response * 0.3f);
        }
        
        pImpl->lastRPEvent = pImpl->lastPhasicEvent;
        NLM_LOG_INFO(std::string("Dopamine RPE response: ") + std::to_string(response));
    }
}

} // namespace nlm
