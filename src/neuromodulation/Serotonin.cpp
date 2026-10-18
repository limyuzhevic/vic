#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float recoveryRate;
    float moodState;
    float impulsivity;
    std::string socialContext;
    std::vector<float> moodHistory;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.2f)
        , maxLevel(1.0f)
        , decayRate(0.15f)
        , recoveryRate(0.1f)
        , moodState(0.5f)
        , impulsivity(0.5f)
        , socialContext("neutral")
        , moodHistory() {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin (mood/social) system initialized");
}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, pImpl->baseline, pImpl->maxLevel);
    // Update related states based on serotonin level
    pImpl->moodState = (pImpl->level - pImpl->baseline) / (pImpl->maxLevel - pImpl->baseline);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for emotional learning
    float moodFactor = 1.0f + pImpl->moodState * 0.3f;
    return moodFactor * (1.0f - pImpl->impulsivity * 0.2f);
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay and recovery
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood state slowly recovers
    pImpl->moodState = pImpl->moodState * 0.95f + 0.5f * 0.05f;
    pImpl->moodHistory.push_back(pImpl->moodState);
}

void Serotonin::modulateMood(float moodState) {
    pImpl->moodState = std::clamp(moodState, 0.0f, 1.0f);
    setLevel(pImpl->baseline + pImpl->moodState * (pImpl->maxLevel - pImpl->baseline));
}

void Serotonin::adjustImpulsivity(float level) {
    pImpl->impulsivity = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::encodeSocialContext(const std::string& context) {
    pImpl->socialContext = context;
}

const std::string& Serotonin::getSocialContext() const {
    return pImpl->socialContext;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodState = 0.5f;
    pImpl->impulsivity = 0.5f;
    pImpl->socialContext = "neutral";
    pImpl->moodHistory.clear();
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Serotonin::getBaseline() const {
    return pImpl->baseline;
}

void Serotonin::setReactivity(float reactivity) {
    pImpl->decayRate = reactivity;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

} // namespace nlm