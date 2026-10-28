#include "SocialLearning.hpp"
#include <NLM_LOG_INFO>

namespace nlm {

struct SocialLearning::Impl {
    SocialLearning::Impl() : observationCount_(0), signalMeaning_(0.0f) {}
    
    Brain* brain_;
    size_t observationCount_;
    std::vector<std::pair<ActionType, ObservedEffect>> observedActions_;
    std::vector<float> signalPattern_;
    float signalMeaning_;
};

SocialLearning::SocialLearning() : pImpl(new Impl) {}

SocialLearning::~SocialLearning() = default;

void SocialLearning::initialize(Brain* brain) {
    pImpl->brain_ = brain;
    // NLM_LOG_INFO("SocialLearning initialized");
}

void SocialLearning::observeAgentAction(ActionType observedAction,
                                     const std::vector<float>& observerState,
                                     const std::vector<float>& resultingState) {
    // PLACEHOLDER: Simple recording for now
    ObservedEffect effect;
    effect.state = observerState;
    effect.resultingState = resultingState;
    effect.reward = 0.0f;
    
    pImpl->observedActions_.push_back(std::make_pair(observedAction, effect));
    pImpl->observationCount_++;
}

bool SocialLearning::canImitate(ActionType observedAction) const {
    return true;  // PLACEHOLDER: Always can imitate for now
}

ActionType SocialLearning::getImitationAction(const std::vector<float>& currentState) {
    return ActionType::Idle;  // PLACEHOLDER: Return idle for now
}

void SocialLearning::learnCommunicationSignal(const std::vector<float>& signalPattern,
                                           float signalReward) {
    pImpl->signalPattern_ = signalPattern;
    pImpl->signalMeaning_ = signalReward;
}

bool SocialLearning::detectSignal(const std::vector<float>& neuralPattern) const {
    return false;  // PLACEHOLDER: No signal detection for now
}

std::vector<float> SocialLearning::getSignalPattern() const {
    return pImpl->signalPattern_;
}

float SocialLearning::getSignalMeaning() const {
    return pImpl->signalMeaning_;
}

void SocialLearning::updateSocialKnowledge(float interactionReward) {
    // PLACEHOLDER: No learning for now
}

void SocialLearning::clear() {
    pImpl->observedActions_.clear();
    pImpl->signalPattern_.clear();
}

bool SocialLearning::hasSocialKnowledge() const {
    return pImpl->observationCount_ > 0;
}

size_t SocialLearning::getObservationCount() const {
    return pImpl->observationCount_;
}

} // namespace nlm