#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct SocialLearning::Impl {
    Brain* brain;
    std::vector<ObservedEffect> recentObservations;
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions;
    std::vector<float> imitationWeights;
    
    Impl() : brain(nullptr) {
        imitationWeights.resize(10, 0.0f);  // 10 action types
    }
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
{
}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                         const std::vector<float>& observerState,
                                         const std::vector<float>& resultingState) {
    // Record observation of another agent's action and effect
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 1.0f;  // Default positive reward for observation
    
    pImpl->observedActions.push_back(std::make_pair(observedAction, effect));
    pImpl->recentObservations.push_back(effect);
    observationCount_++;
    
    // Update imitation weights based on observed success
    pImpl->imitationWeights[static_cast<size_t>(observedAction)] = 
        (pImpl->imitationWeights[static_cast<size_t>(observedAction)] * 0.9f) + 1.0f;
    
    NLM_LOG_INFO("SocialLearning: Observed action " + std::to_string(static_cast<int>(observedAction)));
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've observed this action and it had positive outcomes
    for (const auto& pair : pImpl->observedActions) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    // Select action to imitate based on observation quality
    ActionType bestImitation = ActionType::Wait;
    float bestScore = -1000.0f;
    
    for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
        ActionType action = static_cast<ActionType>(i);
        
        // Check if we can imitate this action
        bool canImitate = false;
        float avgReward = 0.0f;
        size_t count = 0;
        
        for (const auto& pair : pImpl->observedActions) {
            if (pair.first == action) {
                canImitate = true;
                avgReward += pair.second.reward;
                count++;
            }
        }
        
        if (canImitate && count > 0) {
            avgReward /= count;
            
            // Get imitation weight
            float weight = pImpl->imitationWeights[static_cast<size_t>(action)];
            
            // Calculate score: reward + weight
            float score = avgReward + weight;
            
            if (score > bestScore) {
                bestScore = score;
                bestImitation = action;
            }
        }
    }
    
    return bestImitation;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                            float signalReward) {
    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
    
    NLM_LOG_INFO("SocialLearning: Learned communication signal with meaning " + 
                 std::to_string(signalMeaning_));
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    // Check if neural pattern matches learned signal
    if (signalPattern_.empty()) {
        return false;
    }
    
    // Simple pattern matching
    float similarity = 0.0f;
    for (size_t i = 0; i < neuralPattern.size() && i < signalPattern_.size(); ++i) {
        similarity += std::abs(neuralPattern[i] - signalPattern_[i]);
    }
    similarity = 1.0f - similarity;  // Convert to similarity
    
    return similarity > 0.7f;  // High threshold for signal detection
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update social knowledge based on interactions with others
    
    // Decay older imitation weights
    for (float& weight : pImpl->imitationWeights) {
        weight *= 0.95f;
    }
    
    // Boost weights for actions that led to good outcomes
    if (interactionReward > 0.0f) {
        for (size_t i = 0; i < static_cast<size_t>(ActionType::Count); ++i) {
            pImpl->imitationWeights[i] *= (1.0f + interactionReward * 0.5f);
        }
    }
    
    // Limit maximum weights
    for (float& weight : pImpl->imitationWeights) {
        weight = std::min(weight, 2.0f);
    }
}

void SocialLearning::clear() {
    pImpl->observedActions.clear();
    pImpl->recentObservations.clear();
    pImpl->imitationWeights.assign(pImpl->imitationWeights.size(), 0.0f);
    observationCount_ = 0;
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
}

bool SocialLearning::hasSocialKnowledge() const {
    return observationCount_ > 0;
}

} // namespace nlm