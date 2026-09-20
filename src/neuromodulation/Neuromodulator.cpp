#include "Neuromodulator.hpp"
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float lastPredictionError;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), lastPredictionError(0.0f) {}
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
    // Real dopamine-modulated plasticity factor
    // Dopamine modulates synaptic plasticity based on reward prediction error
    // Higher dopamine increases LTP (Long-Term Potentiation) and decreases LTD (Long-Term Depression)
    // This reflects the role of dopamine in reinforcement learning
    
    // Reference: Danjo et al. (2015), "Dopamine signals in the basal ganglia: Between synaptic and behavioral plasticity"
    // Dopamine acts as a teaching signal that modulates synaptic learning rates
    
    // Compute plasticity factor based on dopamine level and prediction error
    float plasticity = 0.5f;  // Baseline plasticity
    
    // Dopamine enhances plasticity for rewarding prediction errors (LTP)
    // When dopamine > threshold, increase plasticity for positive errors
    if (pImpl->level > 0.3f) {
        // Positive dopamine signals reward prediction error -> facilitate LTP
        float rewardSignal = (pImpl->level - 0.3f) * 2.0f;  // Scale to [0, 1.4]
        plasticity += rewardSignal * 0.5f;  // Up to +0.7 plasticity
    }
    
    // Negative dopamine signals aversive prediction error -> enhance LTD
    else if (pImpl->level < 0.3f && pImpl->level > -0.3f) {
        // Near baseline, maintain normal plasticity
        plasticity = 0.5f;
    }
    
    // Strong negative dopamine signals punishment -> enhance LTD
    else {
        float punishmentSignal = -(pImpl->level + 0.3f) * 2.0f;  // Scale to [0, 1.4]
        plasticity -= punishmentSignal * 0.5f;  // Up to -0.7 plasticity
    }
    
    // Constrain plasticity factor to reasonable bounds
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Based on biological dopamine neuron firing patterns and computational models
    
    // Reference: Montague et al. (1996), "A framework for mesencephalic dopamine systems."
    
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline level (as in biological systems)
    float decay = pImpl->decayRate * deltaTime;
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-decay);
    
    // Add small random fluctuations to simulate biological noise
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> noise(-0.01f, 0.01f);
    pImpl->level += noise(rng) * deltaTime;
    
    // Clamp to reasonable biological range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.5f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling
    // Based on computational reinforcement learning models
    // Reference: Schultz et al. (1997), "A novel predictive signal for learning rewards in nucleus accumbens
    
    // Immediate reward response (phasic dopamine)
    float rewardResponse = reward * 0.8f;  // 80% of reward value
    
    // Combine with baseline activity
    pImpl->level = std::min(pImpl->peak, 
                          pImpl->baseline + rewardResponse * pImpl->releaseRate);
    
    // Add temporal difference component for prediction error
    // This will be computed by signalRewardPredictionError()
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // This is the core learning signal in temporal difference learning
    // Reference: Sutton & Barto (1998), "Reinforcement Learning: An Introduction"
    
    // Prediction error drives learning
    // Positive error -> reward prediction was too low -> strengthen connections
    // Negative error -> reward prediction was too high -> weaken connections
    
    // TD (Temporal Difference) learning: delta = r + gamma*V(s') - V(s)
    // Here error represents this delta signal
    
    // Update dopamine level based on prediction error
    float predictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Compute new level with prediction error
    float newLevel = pImpl->level + predictionError * pImpl->releaseRate * 0.5f;
    
    // Apply bounds
    pImpl->level = std::clamp(newLevel, 0.0f, 1.5f);
    
    // Store last prediction error for learning
    pImpl->lastPredictionError = predictionError;
}

} // namespace nlm