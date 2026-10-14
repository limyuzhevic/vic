#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Reward::Impl {
    // Reward state
    float currentValue;
    float accumulatedReward;
    float prediction;
    float predictionError;
    float baseline;
    
    // Dynamics
    float decayRate;
    float learningRate;
    float maxLevel;
    
    // History
    std::vector<float> history;
    std::vector<float> predictionHistory;
    
    // Integration state
    class Brain* brain;
    class Dopamine* dopamine;
    class AttentionalSelection* attention;
    
    Impl()
        : currentValue(0.0f)
        , accumulatedReward(0.0f)
        , prediction(0.0f)
        , predictionError(0.0f)
        , baseline(0.0f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , maxLevel(1.0f)
        , brain(nullptr)
        , dopamine(nullptr)
        , attention(nullptr)
    {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

void Reward::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize Reward: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    
    // Get dopamine system reference
    pImpl->dopamine = brain->getDopamine();
    
    // Get attention system reference
    pImpl->attention = brain->getAttention();
    
    // Initialize reward with baseline from dopamine
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        setValue(dopamineLevel * 0.1f);  // Start with small reward based on dopamine
    }
    
    NLM_LOG_INFO("Reward system initialized");
}

const char* Reward::getName() const {
    return "R";
}

float Reward::getLevel() const {
    return pImpl->currentValue;
}

void Reward::setLevel(float level) {
    pImpl->currentValue = std::clamp(level, 0.0f, pImpl->maxLevel);
}

float Reward::getPlasticityFactor() const {
    // Reward modulates plasticity based on prediction error
    return 1.0f + std::abs(pImpl->predictionError) * 0.5f;
}

void Reward::update(TimestepDuration dt) {
    // Decay current reward towards baseline
    if (dt > 0.0) {
        float decay = pImpl->decayRate * static_cast<float>(dt);
        pImpl->currentValue = std::max(pImpl->baseline, 
                                      pImpl->currentValue - decay * pImpl->currentValue);
    }
    
    // Store history
    pImpl->history.push_back(pImpl->currentValue);
    
    // Keep history manageable
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Reward::computeReward(const Observation& observation) const {
    // Compute reward based on environmental state
    // This is a simplified implementation - real rewards would depend on
    // specific task objectives and environmental dynamics
    
    float reward = 0.0f;
    
    // Get sensory input for reward computation
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    if (!sensoryInput) return reward;
    
    const auto& data = sensoryInput->getData();
    
    // Simple reward heuristic based on sensory patterns
    // In a real implementation, this would be task-specific
    if (!data.empty()) {
        // Example: positive reward for high magnitude inputs
        // This is a placeholder for task-specific reward computation
        for (float value : data) {
            reward += std::max(0.0f, value) * 0.01f;
        }
    }
    
    // Apply temporal smoothing
    reward = std::min(pImpl->maxLevel, reward);
    
    return reward;
}

void Reward::computePredictionError(float expectedReward) {
    // Compute reward prediction error
    pImpl->predictionError = pImpl->currentValue - expectedReward;
    
    // Update prediction for next step
    pImpl->prediction = pImpl->currentValue;
    
    // Store prediction history
    pImpl->predictionHistory.push_back(pImpl->prediction);
    if (pImpl->predictionHistory.size() > 1000) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    // Update accumulated reward with prediction error
    // Positive prediction error (received more than expected) -> learning
    if (pImpl->predictionError > 0.0f) {
        add(pImpl->predictionError * pImpl->learningRate);
    }
}

float Reward::getRewardPredictionError() const {
    return pImpl->predictionError;
}

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    Neuromodulator::setLevel(value);
}

void Reward::add(float delta) {
    if (delta != 0.0f) {
        pImpl->accumulatedReward += delta;
        setValue(getValue() + delta);
        
        // Signal to dopamine system
        if (pImpl->dopamine) {
            signalToDopamine(pImpl->dopamine);
        }
        
        // Store in memory system if available
        if (pImpl->brain) {
            integrateWithMemory(pImpl->brain);
        }
        
        // Apply to attention system
        if (pImpl->attention) {
            applyToAttention(pImpl->attention);
        }
    }
}

void Reward::reset() {
    setValue(0.0f);
    pImpl->accumulatedReward = 0.0f;
    pImpl->prediction = 0.0f;
    pImpl->predictionError = 0.0f;
    clearHistory();
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
    pImpl->predictionHistory.clear();
}

void Reward::integrateWithMemory(class Brain* brain) {
    if (!brain || !pImpl->brain) return;
    
    pImpl->brain = brain;
    
    // Store reward in episodic memory
    // This would create an episodic memory item containing the reward signal
    NLM_LOG_INFO("Storing reward " + std::to_string(getValue()) + " in episodic memory");
}

void Reward::applyToAttention(class AttentionalSelection* attention) const {
    if (!attention) return;
    
    pImpl->attention = attention;
    
    // Modulate attention based on reward magnitude
    // Higher reward should increase attention to related stimuli
    float attentionModulation = getValue() * 0.5f;
    
    NLM_LOG_INFO("Modulating attention with reward-based signal: " + 
                 std::to_string(attentionModulation));
}

void Reward::signalToDopamine(class Dopamine* dopamine) const {
    if (!dopamine) return;
    
    pImpl->dopamine = dopamine;
    
    // Signal reward value to dopamine system
    dopamine->signalReward(getValue());
    
    // Signal prediction error to dopamine system
    dopamine->signalRewardPredictionError(getRewardPredictionError());
    
    NLM_LOG_INFO("Signaled reward " + std::to_string(getValue()) + " and error " + 
                 std::to_string(getRewardPredictionError()) + " to dopamine system");
}

} // namespace nlm
