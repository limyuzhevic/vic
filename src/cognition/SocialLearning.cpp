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
{
    NLM_LOG_INFO("SocialLearning created");
}

SocialLearning::~SocialLearning() {
    NLM_LOG_INFO("SocialLearning destroyed");
}

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
    effect.reward = 0.5f;  // Default reward for observing
    
    observedActions_.push_back(std::make_pair(observedAction, effect));
    observationCount_++;
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    for (const auto& actionEffect : observedActions_) {
        if (actionEffect.first == observedAction) {
            return true;
        }
    }
    return false;
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    if (observedActions_.empty()) {
        return ActionType::Wait;
    }
    
    // Find the most similar observation
    size_t bestIndex = 0;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < observedActions_.size(); ++i) {
        float similarity = 0.0f;
        for (size_t j = 0; j < currentState.size() && j < observedActions_[i].second.state.size(); ++j) {
            similarity += std::abs(currentState[j] - observedActions_[i].second.state[j]);
        }
        similarity = 1.0f - std::min(1.0f, similarity);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIndex = i;
        }
    }
    
    return observedActions_[bestIndex].first;
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                            float signalReward) {
    signalPattern_ = signalPattern;
    signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    if (signalPattern_.empty()) return false;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < neuralPattern.size() && i < signalPattern_.size(); ++i) {
        similarity += std::abs(neuralPattern[i] - signalPattern_[i]);
    }
    similarity = 1.0f - std::min(1.0f, similarity);
    
    return similarity > 0.7f;  // Detection threshold
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // Update observed actions based on interaction reward
    for (auto& actionEffect : observedActions_) {
        if (interactionReward > actionEffect.second.reward) {
            actionEffect.second.reward = interactionReward;
        }
    }
}

void SocialLearning::clear() {
    observedActions_.clear();
    observationCount_ = 0;
    signalPattern_.clear();
    signalMeaning_ = 0.0f;
}

} // namespace nlm