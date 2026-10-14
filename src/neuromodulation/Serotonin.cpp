#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float behavioralFlexibility;
    float moodLevel;
    float negativeMemoryPriority;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             behavioralFlexibility(0.0f), moodLevel(0.0f), negativeMemoryPriority(0.0f) {}
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
    
    void Serotonin::setBaseline(float baseline) {
        pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    }
    
    float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for behavioral flexibility
    return 0.5f + 0.5f * pImpl->level * 0.8f;  // Weaker modulation
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Decay modulation factors
    pImpl->behavioralFlexibility *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->moodLevel *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->negativeMemoryPriority *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::modulateBehavioralFlexibility(float flexibility) {
    pImpl->behavioralFlexibility = std::min(1.0f, flexibility * pImpl->level);
}

void Serotonin::influenceActionSelection(float actionStrength) {
    // 5-HT influences action selection and decision making
    if (actionStrength > 0.0f) {
        // Modify action selection probabilities
    }
}

void Serotonin::affectDecisionMaking(float decisionBias) {
    // 5-HT affects decision bias and risk aversion
    if (decisionBias != 0.0f) {
        // Adjust decision-making processes
    }
}

void Serotonin::influenceMemoryConsolidation(float consolidationFactor) {
    // 5-HT influences memory consolidation, especially emotional memories
    if (consolidationFactor > 0.0f) {
        // Enhance consolidation processes
    }
}

void Serotonin::prioritizeNegativeMemoryConsolidation(float negativePriority) {
    pImpl->negativeMemoryPriority = std::min(1.0f, negativePriority * pImpl->level);
}

void Serotonin::modulateMood(float moodLevel) {
    pImpl->moodLevel = std::min(1.0f, moodLevel * pImpl->level);
}

void Serotonin::influenceSocialBehavior(float socialFactor) {
    // 5-HT influences social behavior and impulsivity
    if (socialFactor != 0.0f) {
        // Modify social behavior responses
    }
}

} // namespace nlm