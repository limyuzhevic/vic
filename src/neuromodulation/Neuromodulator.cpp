#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float predictionErrorWeight;
    
    // Eligibility trace for plasticity
    float eligibilityTrace;
    float traceDecayRate;
    
    // Reward history for prediction error computation
    std::vector<float> rewardHistory;
    float expectedReward;
    float predictionError;
    
    Impl() : 
        level(0.0f),
        baseline(0.02f),           // Minimal tonic dopamine
        peak(1.5f),                // Max phasic response
        decayRate(0.05f),          // Decay constant
        releaseRate(2.0f),         // Release rate on reward
        learningRate(0.1f),        // Plasticity modulation strength
        predictionErrorWeight(0.8f), // How much prediction error affects DA
        eligibilityTrace(0.0f),
        traceDecayRate(0.01f),
        expectedReward(0.0f),
        predictionError(0.0f)
    {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor based on RL theory
    // Tonic dopamine affects baseline exploration, phasic dopamine modulates learning
    float tonicComponent = pImpl->baseline;
    float phasicComponent = (pImpl->level - tonicComponent) * pImpl->learningRate;
    
    // Dopamine also influences exploration vs exploitation
    float explorationFactor = tonicComponent + phasicComponent * 0.5f;
    
    // More complex interaction: dopamine level affects learning rate
    return explorationFactor * (1.0f + std::tanh(pImpl->level * 2.0f - 1.0f));
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience
    // Two-compartment model: tonic and phasic components
    
    float dtFloat = static_cast<float>(dt);
    
    // Update eligibility trace
    pImpl->eligibilityTrace *= std::exp(-pImpl->traceDecayRate * dtFloat);
    
    // Tonic dopamine dynamics (slow, background)
    float tonicTarget = pImpl->baseline;
    float tonicChange = (tonicTarget - pImpl->level) * pImpl->decayRate;
    pImpl->level += tonicChange * dtFloat;
    
    // Handle prediction error integration
    if (std::abs(pImpl->predictionError) > 1e-6f) {
        // Prediction error signals modulate dopamine level
        pImpl->level += pImpl->predictionErrorWeight * pImpl->predictionError * dtFloat;
        
        // Update expected reward with weighted prediction error
        pImpl->expectedReward += pImpl->predictionError * 0.1f;
        
        // Reset prediction error after processing
        pImpl->predictionError = 0.0f;
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on dopamine burst-pause dynamics
    // phasic dopamine burst proportional to reward prediction error
    
    float predictionError = reward - pImpl->expectedReward;
    
    // Dopamine burst magnitude depends on prediction error
    float burst = std::max(0.0f, predictionError) * pImpl->releaseRate;
    
    // Update eligibility trace for credit assignment
    pImpl->eligibilityTrace += burst * 0.1f;
    
    // Apply burst to dopamine level with saturation
    pImpl->level += burst;
    if (pImpl->level > pImpl->peak) {
        pImpl->level = pImpl->peak;
    }
    
    // Store reward for future prediction error computation
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Update expected reward using simple moving average
    if (!pImpl->rewardHistory.empty()) {
        float sum = std::accumulate(pImpl->rewardHistory.begin(), pImpl->rewardHistory.end(), 0.0f);
        pImpl->expectedReward = sum / static_cast<float>(pImpl->rewardHistory.size());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    pImpl->predictionError = error * pImpl->predictionErrorWeight;
    
    // Also contribute to eligibility trace
    pImpl->eligibilityTrace += std::abs(error) * 0.05f;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

void Dopamine::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

float Dopamine::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

} // namespace nlm