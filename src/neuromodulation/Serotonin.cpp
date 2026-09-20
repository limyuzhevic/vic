#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRateModulation;
    float behavioralInhibition;
    float mood;
    
    Impl() 
        : level(0.0f), baseline(0.5f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , learningRateModulation(1.0f), behavioralInhibition(0.0f), mood(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
}

void Serotonin::modulateLearningRate(float rate) {
    pImpl->learningRateModulation = std::clamp(rate, 0.0f, 2.0f);
}

float Serotonin::getLearningRate() const {
    return pImpl->learningRateModulation;
}

void Serotonin::modulateBehavioralInhibition(float inhibition) {
    pImpl->behavioralInhibition = std::clamp(inhibition, 0.0f, 1.0f);
}

float Serotonin::getBehavioralInhibition() const {
    return pImpl->behavioralInhibition;
}

void Serotonin::updateMood(float rewardHistory, float socialContext) {
    pImpl->mood = std::clamp(0.5f * rewardHistory + 0.5f * socialContext, 0.0f, 1.0f);
    
    if (pImpl->mood > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
    }
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

} // namespace nlm
