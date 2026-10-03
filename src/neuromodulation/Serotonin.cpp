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
    float moodModulator;         // How much 5-HT affects mood
    float impulsivityModulator; // How much 5-HT affects impulsivity
    float socialModulator;       // How much 5-HT affects social behavior
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(0.8f),  // Slower decay than other neuromodulators
             moodModulator(1.0f), impulsivityModulator(1.0f), socialModulator(1.0f) {}
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
    // 5-HT modulates mood, impulsivity, and social behavior
    // Higher 5-HT generally reduces impulsivity and promotes calm behavior
    // Affects reward learning and behavioral flexibility
    return 1.0f - 0.3f * pImpl->level;  // 5-HT reduces plasticity (stabilizes behavior)
}

void Serotonin::update(TimestepDuration dt) {
    // Slower decay than other neuromodulators
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodChange(float moodChange) {
    // 5-HT signals mood change
    // Affects motivation and behavioral approach/avoidance
    if (moodChange > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodChange * pImpl->releaseRate);
        pImpl->moodModulator = std::min(2.0f, 1.0f + moodChange * 0.5f);
    } else if (moodChange < 0.0f) {
        pImpl->level = std::max(0.0f, pImpl->level + moodChange * pImpl->releaseRate);
        pImpl->moodModulator = std::max(0.5f, 1.0f + moodChange * 0.3f);
    }
}

void Serotonin::signalImpulsivityChange(float impulsivityChange) {
    // 5-HT affects impulsivity
    // Higher 5-HT reduces impulsive behavior
    pImpl->impulsivityModulator = std::max(0.5f, 1.0f - impulsivityChange * 0.5f);
}

void Serotonin::signalSocialReward(float socialReward) {
    // 5-HT involved in social reward processing
    // Promotes social bonding and affiliation
    pImpl->socialModulator = std::min(2.0f, 1.0f + socialReward * 1.5f);
}

void Serotonin::signalLearningDiscrepancy(float discrepancy) {
    // 5-HT responds to prediction errors in learning
    // Helps maintain behavioral stability through error correction
    if (discrepancy > 0.0f) {
        // Positive discrepancy (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + discrepancy * 0.3f);
    } else {
        // Negative discrepancy (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level + discrepancy * 0.2f);
    }
}

} // namespace nlm