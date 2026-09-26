#include "ActionConsequencePredictor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void ActionConsequencePredictor::recordExperience(ActionType action,
                                                  const std::vector<float>& beforeState,
                                                  const std::vector<float>& afterState,
                                                  float reward) {
    if (!beforeState.empty() && !afterState.empty()) {
        pImpl->experiences.emplace_back(action, beforeState, afterState, reward);
        
        // Update action quality (average expected reward)
        bool found = false;
        for (auto& quality : pImpl->actionQualities) {
            if (quality.first == action) {
                quality.second = (quality.second + reward) * 0.5f;
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->actionQualities.emplace_back(action, reward);
        }
        
        NLM_LOG_INFO("ActionConsequencePredictor recorded experience: action=" + 
                   std::to_string(static_cast<int>(action)) + ", reward=" + std::to_string(reward));
    }
}

std::vector<float> ActionConsequencePredictor::predictConsequence(ActionType action,
                                                                 const std::vector<float>& currentState) {
    std::vector<float> predictedConsequence;
    
    // Find best matching experience
    float bestScore = 0.0f;
    size_t bestIndex = 0;
    
    for (size_t i = 0; i < pImpl->experiences.size(); ++i) {
        ActionType learnedAction = std::get<0>(pImpl->experiences[i]);
        std::vector<float> beforeState = std::get<1>(pImpl->experiences[i]);
        std::vector<float> afterState = std::get<2>(pImpl->experiences[i]);
        
        if (learnedAction == action && !beforeState.empty()) {
            float similarity = 0.0f;
            if (currentState.size() == beforeState.size() && currentState.size() > 0) {
                similarity = 0.0f;
                for (size_t j = 0; j < currentState.size(); ++j) {
                    similarity += std::abs(currentState[j] - beforeState[j]);
                }
                similarity = 1.0f - similarity / currentState.size();
            }
            
            if (similarity > bestScore) {
                bestScore = similarity;
                bestIndex = i;
                predictedConsequence = afterState;
            }
        }
    }
    
    if (bestScore > 0.0f) {
        NLM_LOG_INFO("ActionConsequencePredictor predicted consequence with confidence " + 
                   std::to_string(bestScore));
    }
    
    return predictedConsequence;
}

float ActionConsequencePredictor::getConsequenceConfidence(ActionType action,
                                                         const std::vector<float>& currentState) const {
    float confidence = 0.0f;
    size_t matchingExperiences = 0;
    
    for (const auto& exp : pImpl->experiences) {
        ActionType learnedAction = std::get<0>(exp);
        std::vector<float> beforeState = std::get<1>(exp);
        
        if (learnedAction == action && !beforeState.empty()) {
            matchingExperiences++;
            
            if (currentState.size() == beforeState.size()) {
                float similarity = 0.0f;
                for (size_t i = 0; i < currentState.size(); ++i) {
                    similarity += std::abs(currentState[i] - beforeState[i]);
                }
                similarity = 1.0f - similarity / currentState.size();
                confidence += similarity;
            }
        }
    }
    
    if (matchingExperiences > 0) {
        confidence /= matchingExperiences;
    }
    
    return confidence;
}

void ActionConsequencePredictor::updatePrediction(ActionType action,
                                                 const std::vector<float>& predicted,
                                                 const std::vector<float>& actual,
                                                 float reward) {
    // Record new experience
    recordExperience(action, predicted, actual, reward);
    
    NLM_LOG_INFO("ActionConsequencePredictor updated prediction for action " + 
               std::to_string(static_cast<int>(action)) + ", actual reward=" + std::to_string(reward));
}

float ActionConsequencePredictor::getActionQuality(ActionType action) const {
    for (const auto& quality : pImpl->actionQualities) {
        if (quality.first == action) {
            return quality.second;
        }
    }
    return 0.0f;
}

void ActionConsequencePredictor::clear() {
    pImpl->experiences.clear();
    pImpl->actionQualities.clear();
    NLM_LOG_INFO("ActionConsequencePredictor cleared all learned experiences");
}

} // namespace nlm