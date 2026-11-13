#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float moodModulation;
    float impulseControl;
    float socialLearning;
    float baseline;
    float peak;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , decayRate(0.05f)
        , moodModulation(1.2f)
        , impulseControl(1.1f)
        , socialLearning(1.3f)
        , baseline(0.0f)
        , peak(1.0f) {}
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
    // 5-HT regulates mood, impulsivity, and social behavior
    return 1.0f - 0.2f * pImpl->level;  // Higher 5-HT reduces impulsivity
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // Increase serotonin level based on mood state
    setLevel(std::min(1.0f, pImpl->level + mood * 0.2f));
}

void Serotonin::reduceImpulsivity(const std::vector<NeuronId>& neurons, float strength) {
    // Reduce firing rates of impulsive neurons (social inhibition)
    if (pImpl->brain) {
        for (const auto& neuronId : neurons) {
            // Apply inhibitory current
            pImpl->brain->injectCurrent(neuronId, -strength * pImpl->impulseControl);
        }
    }
}

void Serotonin::enableSocialLearning(Brain* socialPartner) {
    // Enable social learning mode (imitation, observational learning)
    pImpl->brain = socialPartner;
    // Would normally enhance associative memory connections
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->brain = nullptr;
}

} // namespace nlm