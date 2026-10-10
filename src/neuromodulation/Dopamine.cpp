#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float rewardHistory;
    float predictionErrorHistory;
    float decayRate;
    float learningRate;
    float noveltySignal;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , rewardHistory(0.0f)
        , predictionErrorHistory(0.0f)
        , decayRate(0.05f)
        , learningRate(0.1f)
        , noveltySignal(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = level;
}

float Dopamine::getPlasticityFactor() const {
    // Higher dopamine increases plasticity factor
    return 1.0f + pImpl->level * 0.5f;
}

float Dopamine::getAttentionModulator() const {
    // Moderate dopamine enhances attention
    return 1.0f + pImpl->level * 0.2f;
}

float Dopamine::getMemoryModulator() const {
    // Higher dopamine enhances memory encoding
    return 1.0f + pImpl->level * 0.3f;
}

float Dopamine::getLearningRateModulator() const {
    // Dopamine modulates learning rate
    return 1.0f + pImpl->level * 0.4f;
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update novelty signal contribution
    float noveltyContribution = pImpl->noveltySignal * 0.3f;
    pImpl->level += noveltyContribution - noveltyContribution * 0.1f;  // Smooth transition
    
    // Limit level to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    pImpl->rewardHistory = reward;
    pImpl->level += reward * 0.5f;  // Stronger response to larger rewards
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->predictionErrorHistory = error;
    
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    pImpl->level += error * 0.3f;
}

void Dopamine::recordOutcome(float outcomeValue) {
    // Integrate outcome value into learning
    pImpl->level += (outcomeValue - pImpl->rewardHistory) * 0.2f;
}

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

void Dopamine::onLearning(float learningSignal) {
    pImpl->level += learningSignal * 0.1f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
