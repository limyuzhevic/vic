#include "InverseModel.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// InverseModel implementation
ActionType InverseModel::predictAction(const std::vector<float>& currentState,
                                      const std::vector<float>& desiredNextState,
                                      const std::vector<float>& actionProbabilities) {
    if (currentState.size() != stateSize_ || desiredNextState.size() != stateSize_) {
        return ActionType::Wait;
    }
    
    // Combine states as input
    std::vector<float> input;
    input.reserve(stateSize_ * 2);
    input.insert(input.end(), currentState.begin(), currentState.end());
    input.insert(input.end(), desiredNextState.begin(), desiredNextState.end());
    
    // Forward pass
    std::vector<float> current = input;
    for (auto& layer : layers_) {
        current = layer.forward(current);
    }
    
    // Get action probabilities
    std::vector<float> probs = softmax(current);
    
    // Select action based on preferences and prediction
    return selectActionFromProbabilities(probs, actionProbabilities);
}

ActionType InverseModel::selectActionFromProbabilities(
    const std::vector<float>& predictionProbs,
    const std::vector<float>& existingProbs) const {
    if (predictionProbs.empty() || existingProbs.empty()) {
        return ActionType::Wait;
    }
    
    // Combine prediction with existing preferences
    std::vector<float> combinedProbs(predictionProbs.size(), 0.0f);
    
    for (size_t i = 0; i < predictionProbs.size(); ++i) {
        // Blend prediction with existing policy
        float weight = 0.7f;  // Prediction weight
        combinedProbs[i] = weight * predictionProbs[i] + 
                          (1.0f - weight) * existingProbs[i % existingProbs.size()];
    }
    
    // Add intrinsic motivation bonus
    float intrinsicBonus = getActionIntrinsicBonus(combinedProbs);
    for (size_t i = 0; i < combinedProbs.size(); ++i) {
        combinedProbs[i] += intrinsicBonus * 0.1f;
    }
    
    // Normalize
    float sum = 0.0f;
    for (float p : combinedProbs) sum += p;
    if (sum > 0.0f) {
        for (auto& p : combinedProbs) p /= sum;
    }
    
    // Select action
    return getActionFromProbabilities(combinedProbs);
}

ActionType InverseModel::getActionFromProbabilities(const std::vector<float>& probs) const {
    if (probs.empty()) return ActionType::Wait;
    
    // Use policy network for exploration when uncertain
    if (hasPolicyNetwork_) {
        float maxProb = *std::max_element(probs.begin(), probs.end());
        if (maxProb < 0.5f) {  // High uncertainty
            return policyNetwork_->getActionWithExploration(std::vector<float>(), 0.3f);
        }
    }
    
    // Select deterministic action
    float cumulative = 0.0f;
    float randVal = getRandomFloat(0.0f, 1.0f);
    
    for (size_t i = 0; i < probs.size(); ++i) {
        cumulative += probs[i];
        if (randVal <= cumulative) {
            return static_cast<ActionType>(i);
        }
    }
    
    return ActionType::Wait;
}

float InverseModel::getActionIntrinsicBonus(const std::vector<float>& probs) const {
    float maxProb = *std::max_element(probs.begin(), probs.end());
    return (maxProb - 0.5f) * 2.0f; // Bonus for confidence
}

float InverseModel::getRandomFloat(float min, float max) {
    if (!brain_) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    return brain_->getRandomGenerator()->uniformReal(min, max);
}

} // namespace nlm