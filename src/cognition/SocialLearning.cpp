#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.5f)
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
    if (!brain_) return;
    
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.5f;  // Default positive reward for observed action
    
    observedActions_.push_back(std::make_pair(observedAction, effect));
    observationCount_++;
    
    // Check if we can imitate this action
    if (!canImitate(observedAction)) {
        learnFromObservation(observedAction, observerState, resultingState);
    }
    
    // Update social knowledge
    updateSocialKnowledge(0.1f);
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    for (const auto& observed : observedActions_) {
        if (observed.first == observedAction) {
            // Check if we have experienced similar action effects
            for (const auto& experienced : observedActions_) {
                if (experienced.first == observedAction) {
                    return true;  // Already observed this action
                }
            }
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    // Find observed action that best matches current state
    ActionType bestAction = ActionType::Wait;
    float bestScore = 0.0f;
    
    for (const auto& observed : observedActions_) {
        float similarity = computeStateSimilarity(currentState, observed.second.state);
        
        if (similarity > bestScore) {
            bestScore = similarity;
            bestAction = observed.first;
        }
    }
    
    return bestAction;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
    NLM_LOG_INFO("SocialLearning: Learned communication signal with meaning " + 
                 std::to_string(signalMeaning_));
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    return computePatternSimilarity(neuralPattern, signalPattern_) > 0.7f;
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Learn from interaction outcomes
    for (auto& observed : observedActions_) {
        // Each observation contributes to social knowledge
        observed.second.reward = (observed.second.reward + interactionReward) * 0.5f;
    }
    
    // Increase imitation likelihood for high-reward actions
    for (auto& observed : observedActions_) {
        if (observed.second.reward > 0.3f) {
            // Mark action as imitable
            for (auto& other : observedActions_) {
                if (other.first == observed.first) {
                    // This action was successful in this context
                    return;
                }
            }
        }
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    observationCount_ = 0;
    signalPattern_.clear();
    signalMeaning_ = 0.5f;
}

float SocialLearning::computeStateSimilarity(const std::vector<float>& a,
                                            const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

void SocialLearning::learnFromObservation(ActionType action,
                                          const std::vector<float>& observerState,
                                          const std::vector<float>& resultingState) {
    // Create a new observation record
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.5f;  // Default positive reward
    
    observedActions_.push_back(std::make_pair(action, effect));
    observationCount_++;
    
    NLM_LOG_INFO("SocialLearning: Learned from observation of action " + 
                 std::to_string(static_cast<int>(action)));
}

float SocialLearning::computePatternSimilarity(const std::vector<float>& a,
                                              const std::vector<float>& b) const {
    return computeStateSimilarity(a, b);
}

} // namespace nlm
