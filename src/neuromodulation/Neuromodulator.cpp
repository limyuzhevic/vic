#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float effectiveLearningRate;
    std::unique_ptr<RandomGenerator> rng;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , effectiveLearningRate(0.0f), rng(nullptr) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize random generator for biological noise
    uint64_t seed = 42;  // Deterministic but biologically variable
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

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
    // Enhanced dopamine-modulated plasticity factor
    // Based on biological research: optimal learning occurs at moderate dopamine levels
    // Low DA = exploration, High DA = exploitation, Moderate DA = balanced learning
    
    if (pImpl->level <= 0.3f) {
        // Low dopamine enhances exploratory plasticity
        return 1.0f + 2.0f * pImpl->level;  // 1.0 to 1.6
    } else if (pImpl->level <= 0.7f) {
        // Moderate dopamine optimizes learning
        return 2.5f;  // Peak learning efficiency
    } else {
        // High dopamine reduces plasticity (stabilization phase)
        return 1.5f + 0.5f * (1.0f - pImpl->level);  // 1.5 to 1.0
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with reward prediction error integration
    // More biologically realistic model based on dopaminergic research
    
    // Primary decay mechanism
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add biological noise for exploration (simulating natural variability)
    // This mimics neural noise in the dopaminergic system
    float noise = 0.02f * (pImpl->rng ? pImpl->rng->getFloat(0.0f, 2.0f) - 1.0f : (pImpl->rng = std::make_unique<RandomGenerator>(42), 0.0f));
    noise = noise * 0.005f;  // Scale down for stability
    pImpl->level += noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with temporal dynamics
    // Based on dopamine burst/phasic responses
    
    // Immediate burst response (phasic dopamine)
    float burstAmplitude = std::min(0.8f, reward * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmplitude * pImpl->releaseRate);
    
    // Set baseline for future rewards
    if (reward > 0.5f) {
        pImpl->baseline = std::max(pImpl->baseline, pImpl->level * 0.8f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced reward prediction error signaling
    // Phasic dopamine responses to prediction errors
    
    if (error > 0.1f) {
        // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    } else if (error < -0.1f) {
        // Negative prediction error (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.8f);
    }
}

void Dopamine::integrateWithLearning(float learningRate, float predictionError) {
    // Enhanced integration with neural learning mechanisms
    // Modulates synaptic plasticity based on dopamine levels
    
    float plasticityMod = getPlasticityFactor();
    
    // Adapt baseline based on prediction error
    if (predictionError > 0.0f) {
        pImpl->baseline = std::min(pImpl->baseline + predictionError * 0.05f, 0.5f);
    } else {
        pImpl->baseline = std::max(pImpl->baseline + predictionError * 0.1f, 0.0f);
    }
    
    // Apply dopamine modulation to learning
    float effectiveLearningRate = learningRate * plasticityMod;
    
    // This can be used by external plasticity systems
    // Store for external use
    pImpl->effectiveLearningRate = effectiveLearningRate;
}

} // namespace nlm
