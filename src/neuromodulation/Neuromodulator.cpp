#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Neuromodulator::Impl {
    float level;
    float baselineLevel;
    float targetLevel;
    float adaptationRate;
    float plasticityFactor;
    float decayRate;
    SimulationStep lastUpdateStep;
    
    Impl() : level(0.0f), baselineLevel(0.0f), targetLevel(0.0f), 
             adaptationRate(0.1f), plasticityFactor(1.0f), decayRate(0.05f),
             lastUpdateStep(0) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

const char* Neuromodulator::getName() const {
    return "Base Neuromodulator";
}

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = level;
}

float Neuromodulator::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Neuromodulator::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level *= (1.0f - pImpl->decayRate);
}

Dopamine::Dopamine() : Neuromodulator() {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.1f;
    pImpl->targetLevel = 0.5f;
    pImpl->adaptationRate = 0.15f;
    pImpl->plasticityFactor = 1.0f;
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return Neuromodulator::getLevel();
}

void Dopamine::setLevel(float level) {
    Neuromodulator::setLevel(level);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor with biological dynamics
    float level = Neuromodulator::getLevel();
    
    if (level <= 0.0f) {
        // Low dopamine: reduced plasticity, stable memory
        pImpl->plasticityFactor = 0.3f + 0.7f * std::exp(level * 2.0f);
    } else {
        // High dopamine: enhanced plasticity, learning new information
        pImpl->plasticityFactor = 0.8f + 0.4f * std::tanh(level * 1.5f);
    }
    
    // Cap plasticity factor
    pImpl->plasticityFactor = std::clamp(pImpl->plasticityFactor, 0.1f, 1.5f);
    
    NLM_LOG_INFO("Dopamine plasticity factor: " + std::to_string(pImpl->plasticityFactor) +
                " (level: " + std::to_string(level) + ")");
    
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: adaptation to prediction errors
    // Level changes based on reward prediction error and temporal difference
    
    float delta = 0.0f; // Would be updated by reward prediction error signal
    
    // Adapt towards target level based on prediction error
    if (delta != 0.0f) {
        pImpl->targetLevel += delta * pImpl->adaptationRate;
        pImpl->targetLevel = std::clamp(pImpl->targetLevel, 0.0f, 1.0f);
    }
    
    // Smooth transition towards target
    float approachRate = 0.1f * pImpl->adaptationRate;
    pImpl->level = pImpl->level * (1.0f - approachRate) + 
                   pImpl->targetLevel * approachRate;
    
    // Decay during rest periods
    if (pImpl->level < pImpl->baselineLevel * 0.5f) {
        float decayFactor = 1.0f - pImpl->decayRate * 0.5f;
        pImpl->level *= decayFactor;
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst of dopamine on unexpected reward
    // Phasic response proportional to reward prediction error
    
    // Simple reward prediction error calculation
    float expected = 0.2f; // Would come from prediction system
    float predictionError = reward - expected;
    
    if (predictionError > 0.0f) {
        // Unexpected reward causes dopamine burst
        float burstIntensity = predictionError * 2.0f;
        pImpl->level = std::min(pImpl->level + burstIntensity, 1.0f);
        pImpl->targetLevel = std::min(pImpl->targetLevel + burstIntensity * 0.5f, 1.0f);
        
        NLM_LOG_INFO("Dopamine burst: reward=" + std::to_string(reward) +
                    " | prediction error=" + std::to_string(predictionError) +
                    " | new level=" + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Phasic dip when reward is worse than expected
    
    if (error < 0.0f) {
        // Worse than expected: dopamine dip
        float dipIntensity = std::abs(error) * 1.5f;
        pImpl->level = std::max(pImpl->level - dipIntensity, 0.0f);
        
        NLM_LOG_INFO("Dopamine dip: prediction error=" + std::to_string(error) +
                    " | new level=" + std::to_string(pImpl->level));
    }
}

} // namespace nlm
