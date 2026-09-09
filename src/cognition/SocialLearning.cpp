#include "SocialLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

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
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;  // Observer doesn't know reward
    
    observedActions_.push_back({observedAction, effect});
    ++observationCount_;
    
    // Keep memory bounded
    if (observedActions_.size() > 500) {
        observedActions_.erase(observedActions_.begin());
    }
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    // Can imitate if we've seen this action before and know its effect
    for (const auto& pair : observedActions_) {
        if (pair.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    ActionType best = ActionType::Wait;
    float bestSim = 0.0f;
    
    for (const auto& pair : observedActions_) {
        float sim = computeSimilarity(currentState, pair.second.state);
        if (sim > bestSim) {
            bestSim = sim;
            best = pair.first;
        }
    }
    
    return best;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                            float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
    
    // Store signal in association memory if available
    if (brain_) {
        // In a full implementation, this would associate the signal pattern
        // with the meaning/reward
    }
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
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
    observationCount_ += static_cast<size_t>(interactionReward);
    
    // Update signal meaning based on interaction success
    if (interactionReward > 0.5f) {
        signalMeaning_ = (signalMeaning_ + interactionReward) * 0.5f;
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    observationCount_ = 0;
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
}

float SocialLearning::computeSimilarity(const std::vector<float>& a,
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

} // namespace nlm
