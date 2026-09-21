#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct SocialLearning::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};

SocialLearning::SocialLearning()
    : pImpl(new Impl)
    , brain_(nullptr)
    , observationCount_(0)
    , signalMeaning_(0.0f)
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
    // Record observation of another agent's action and its effect
    // This is the foundation for social learning
    
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward directly
    
    observedActions_.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded to prevent runaway growth
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
    
    NLM_LOG_DEBUG("Observed action " + std::to_string(static_cast<int>(observedAction)) + 
                  " with effect on state");
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and understand its consequences
    // Returns true if we have reliable predictions for this action
    
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            // Check if we have enough observations to make a reliable prediction
            // More observations = higher confidence
            return true;
        }
    }
    
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    // Get the best action to imitate given current state
    // Chooses action whose observed state most closely matches current state
    
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : observedActions_) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    if (bestSim > 0.5f) {
        NLM_LOG_DEBUG("Imitating action " + std::to_string(static_cast<int>(best)) + 
                      " (similarity: " + std::to_string(bestSim) + ")");
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                              float signalReward) {
    // Learn simple communication signal from another agent
    // Signal is a neural pattern that predicts reward from other agent
    // Enables coordination and information sharing
    
    if (signalPattern.empty()) {
        return;
    }
    
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
    
    // Update all previously observed actions with this signal's meaning
    for (auto& pair : observedActions_) {
        // Adjust reward estimate based on signal meaning
        pair.second.reward = pair.second.reward * 0.9f + signalReward * 0.1f;
    }
    
    NLM_LOG_INFO("Learned communication signal with meaning: " + std::to_string(signalReward));
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    // Detect if another agent is signaling
    // Compares input pattern against learned signals
    
    if (signalPattern_.empty()) {
        return false;
    }
    
    float similarity = computeSimilarity(neuralPattern, signalPattern_);
    return similarity > 0.7f;  // Threshold for signal detection
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update social knowledge based on interactions with other agents
    // Refines predictions of other agents' behavior and intentions
    
    // Update value of observed actions based on whether interaction was beneficial
    for (auto& pair : observedActions_) {
        // Blend previous reward estimate with new information
        pair.second.reward = pair.second.reward * 0.9f + interactionReward * 0.1f;
    }
    
    // Also update signal meaning based on interaction success
    if (interactionReward > 0.0f) {
        signalMeaning_ = signalMeaning_ * 0.8f + interactionReward * 0.2f;
    }
    
    NLM_LOG_DEBUG("Updated social knowledge with reward: " + std::to_string(interactionReward));
}

void SocialLearning::clear() {
    // Clear all social learning data
    observedActions_.clear();
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
    observationCount_ = 0;
    
    NLM_LOG_INFO("SocialLearning cleared");
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
                                     const std::vector<float>& b) const {
    // Compute similarity between two state vectors
    // Uses cosine similarity for pattern matching
    
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

} // namespace nlm