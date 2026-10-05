#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float behavioralInhibition;
    float decayRate;
    float burstRate;
    std::vector<float> history;
    std::vector<float> recentRewards;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.5f)
        , behavioralInhibition(0.0f)
        , decayRate(0.05f)
        , burstRate(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood regulation and behavioral inhibition");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->mood = std::clamp(level, 0.0f, 1.0f);
    // High serotonin = good mood, low behavioral inhibition
    // Low serotonin = poor mood, high behavioral inhibition
    pImpl->behavioralInhibition = 1.0f - level;
}

void Serotonin::signalReward(float reward) {
    if (reward > 0.0f) {
        // Positive reward increases serotonin
        pImpl->level = std::min(1.0f, pImpl->level + reward * pImpl->burstRate);
    } else if (reward < 0.0f) {
        // Negative reward decreases serotonin
        pImpl->level = std::max(0.0f, pImpl->level + reward * pImpl->burstRate);
    }
    
    // Record reward for mood computation
    pImpl->recentRewards.push_back(reward);
    if (pImpl->recentRewards.size() > 100) {
        pImpl->recentRewards.erase(pImpl->recentRewards.begin());
    }
}

void Serotonin::signalPredictionError(float error) {
    // Prediction error affects serotonin
    pImpl->level = std::max(0.0f, pImpl->level + error * 0.5f);
}

void Serotonin::updateMood(float predictedReward, float actualReward) {
    float rewardError = actualReward - predictedReward;
    
    if (rewardError > 0.1f) {
        // Positive prediction error (better than expected) -> boost mood/serotonin
        pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
    } else if (rewardError < -0.1f) {
        // Negative prediction error (worse than expected) -> reduce serotonin/mood
        pImpl->level = std::max(0.0f, pImpl->level - 0.2f);
    }
    
    // Update behavioral inhibition based on mood
    pImpl->behavioralInhibition = 1.0f - pImpl->level;
}

float Serotonin::getPlasticityFactor() const {
    // Moderate serotonin enhances working memory consolidation
    // but too much can impair new learning
    return 0.7f + 0.6f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline (0.5 mood)
    float targetLevel = 0.5f;
    pImpl->level += (targetLevel - pImpl->level) * 0.05f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update behavioral inhibition
    pImpl->behavioralInhibition = 1.0f - pImpl->level;
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getBehavioralInhibition() const {
    return pImpl->behavioralInhibition;
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::addToHistory(float value) {
    pImpl->history.push_back(value);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Serotonin::reset() {
    pImpl->level = 0.5f;
    pImpl->mood = 0.5f;
    pImpl->behavioralInhibition = 0.5f;
    pImpl->recentRewards.clear();
    pImpl->history.clear();
}

} // namespace nlm