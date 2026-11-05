#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Enhanced reward system state
    float accumulatedReward;
    float predictionError;
    float explorationBonus;
    float learningRate;
    std::vector<float> rewardHistory;
    
    Impl() : 
        level(0.0f), 
        baseline(0.1f), 
        peak(1.0f), 
        decayRate(0.05f), 
        releaseRate(1.0f),
        accumulatedReward(0.0f),
        predictionError(0.0f),
        explorationBonus(0.0f),
        learningRate(0.3f) {}
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
    // Enhanced dopamine-modulated plasticity factor
    // Phase 6: Real neuromodulation effects on learning
    
    // Base plasticity plus modulation by current dopamine level
    // Higher dopamine increases learning rate but with diminishing returns
    float basePlasticity = 0.3f;
    float dopamineModulation = 0.7f * pImpl->level;
    
    // Include prediction error in plasticity factor
    float peModulation = 0.2f * std::abs(pImpl->predictionError);
    
    // Apply exploration bonus
    float explorationModulation = 0.3f * pImpl->explorationBonus;
    
    return basePlasticity + dopamineModulation + peModulation + explorationModulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with homeostatic regulation
    // Phase 6: Neurotransmitter dynamics with realistic temporal properties
    
    // Convert dt to seconds
    float dtSeconds = static_cast<float>(dt) * 0.001f;  // Assuming ms timestep
    
    // Exponential decay towards baseline
    float decayFactor = std::exp(-pImpl->decayRate * dtSeconds);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Integrate accumulated reward
    pImpl->accumulatedReward += pImpl->level * dtSeconds * 0.1f;
    
    // Update reward history
    pImpl->rewardHistory.push_back(pImpl->level);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with prediction error integration
    // Phase 6: Real dopamine response to rewards with temporal dynamics
    
    // Calculate reward prediction error
    float predictionError = reward - (pImpl->level - pImpl->baseline);
    pImpl->predictionError = predictionError;
    
    // Update dopamine level with reward response
    // Phasic response includes both baseline and reward magnitude
    float phasicResponse = reward * pImpl->releaseRate;
    
    // Integrate with tonic response (baseline level)
    pImpl->level = pImpl->baseline + phasicResponse;
    
    // Clamp to peak level
    pImpl->level = std::min(pImpl->peak, pImpl->level);
    
    // Add to accumulated reward
    pImpl->accumulatedReward += reward;
    
    // Store in history for analysis
    pImpl->rewardHistory.push_back(pImpl->level);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling for reinforcement learning
    // Phase 6: TD learning implementation with credit assignment
    
    // Prediction error modulates dopamine level directly
    pImpl->predictionError = error;
    
    // Signaled prediction error affects learning
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    float peEffect = error * pImpl->releaseRate * 0.5f;
    
    // Update dopamine level with prediction error
    pImpl->level = std::max(0.0f, pImpl->level + peEffect);
    
    // Prediction error also affects exploration bonus
    pImpl->explorationBonus = std::max(0.0f, pImpl->explorationBonus + error * 0.1f);
}

void Dopamine::setExplorationBonus(float bonus) {
    // Set exploration bonus for novel behavior
    pImpl->explorationBonus = std::clamp(bonus, 0.0f, 1.0f);
}

float Dopamine::getAccumulatedReward() const {
    // Get accumulated reward over time
    return pImpl->accumulatedReward;
}

float Dopamine::getPredictionError() const {
    // Get current prediction error
    return pImpl->predictionError;
}

} // namespace nlm
