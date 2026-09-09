#include "Serotonin.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodState;
    float impulsivityControl;
    float socialBehavior;
    float resetThreshold;
    std::vector<float> history;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.03f)  // Baseline 5-HT level ~3%
        , peak(1.0f)
        , decayRate(0.02f)
        , releaseRate(1.5f)
        , moodState(0.5f)
        , impulsivityControl(0.5f)
        , socialBehavior(0.5f)
        , resetThreshold(0.5f)
        , dist(0.0f, 1.0f)
    {
        std::random_device rd;
        rng.seed(rd());
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - modulating mood and social behavior");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived state
    updateDerivedState();
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Spontaneous fluctuations in serotonin level
    if (pImpl->level > pImpl->resetThreshold) {
        float fluctuation = pImpl->dist(pImpl->rng);
        pImpl->level = std::max(pImpl->baseline, pImpl->level + (fluctuation - 0.5f) * 0.2f * pImpl->releaseRate);
    }
    
    // Update mood and social behavior based on 5-HT level
    pImpl->moodState = 0.3f + pImpl->level * 0.8f;  // Range [0.3, 1.1]
    pImpl->socialBehavior = 0.5f + pImpl->level * 0.6f;  // Range [0.5, 1.1]
    pImpl->impulsivityControl = 0.5f + pImpl->level * 0.7f;  // Range [0.5, 1.2]
    
    // Log level changes
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Serotonin::updateDerivedState() {
    // Update mood state based on 5-HT level
    pImpl->moodState = 0.3f + pImpl->level * 0.8f;
    
    // Modulate social behavior in the brain
    if (pImpl->brain) {
        pImpl->socialBehavior = 0.5f + pImpl->level * 0.6f;
        pImpl->impulsivityControl = 0.5f + pImpl->level * 0.7f;
    }
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin reduces impulsive actions and stabilizes learning
    // High 5-HT reduces plasticity for impulsive responses but enhances for thoughtful ones
    float factor = 0.7f + pImpl->level * 0.5f;  // Base reduction for impulsivity control
    return factor;
}

void Serotonin::signalPositiveSocialEvent() {
    // Burst of 5-HT on positive social interactions
    setLevel(std::min(pImpl->peak, pImpl->level + 0.9f * pImpl->releaseRate));
}

void Serotonin::signalNegativeSocialEvent() {
    // Decrease in 5-HT on negative social events
    setLevel(std::max(pImpl->baseline, pImpl->level * 0.6f));
}

void Serotonin::signalSocialFeedback() {
    // Fluctuation in 5-HT based on social feedback
    float fluctuation = pImpl->dist(pImpl->rng) * 2.0f - 1.0f;  // [-1.0, 1.0]
    setLevel(std::clamp(pImpl->level + fluctuation * 0.3f, pImpl->baseline, pImpl->peak));
}

void Serotonin::setMoodState(float mood) {
    pImpl->moodState = std::clamp(mood, 0.0f, 2.0f);
}

float Serotonin::getMoodState() const {
    return pImpl->moodState;
}

float Serotonin::getImpulsivityControl() const {
    return pImpl->impulsivityControl;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
