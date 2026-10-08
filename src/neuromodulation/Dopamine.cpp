#include "../neuromodulation/Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;                    // Current dopamine concentration
    float baselineLevel;            // Baseline dopamine level
    float burstStrength;            // Strength of reward bursts
    float predictionError;          // Reward prediction error
    float eligibilityTrace;         // Synaptic eligibility trace
    SimulationStep lastUpdate;      // Last update time
    
    Impl()
        : level(0.0f)
        , baselineLevel(0.05f)
        , burstStrength(0.5f)
        , predictionError(0.0f)
        , eligibilityTrace(0.0f)
        , lastUpdate(0)
    {}
};

Dopamine::Dopamine()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Dopamine system initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(level, 1.0f));
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity based on reward prediction error
    // High dopamine increases plasticity for reward-predictive responses
    float mod = 1.0f;
    
    if (pImpl->predictionError > 0.1f) {
        // Positive prediction error (unexpected reward) -> increased plasticity
        mod = 1.0f + pImpl->predictionError * 2.0f;
    } else if (pImpl->predictionError < -0.1f) {
        // Negative prediction error (unexpected omission) -> decreased plasticity
        mod = 1.0f - std::abs(pImpl->predictionError) * 1.5f;
    }
    
    return mod;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= (1.0f - dt * 0.05f);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - dt * 0.01f);
}

void Dopamine::signalReward(float reward) {
    // Signal received reward - creates dopamine burst
    if (reward > 0.0f) {
        // Compute burst magnitude based on reward value
        float burst = reward * pImpl->burstStrength;
        setLevel(getLevel() + burst);
        
        NLM_LOG_INFO("Dopamine burst: reward=" + std::to_string(reward) + 
                     " level=" + std::to_string(getLevel()));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Signal reward prediction error
    pImpl->predictionError = error;
    
    // Update eligibility trace based on prediction error
    pImpl->eligibilityTrace += std::abs(error) * 0.1f;
    
    NLM_LOG_INFO("Dopamine prediction error: " + std::to_string(error) + 
                 " eligibility trace=" + std::to_string(pImpl->eligibilityTrace));
}

} // namespace nlm
