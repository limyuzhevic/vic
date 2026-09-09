// Implementation of Acetylcholine neuromodulator
#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float memoryConsolidationBoost;
    float attentionBoost;
    float rehearsalBoost;
    
    Impl() : level(0.0f), baseline(0.05f), memoryConsolidationBoost(2.0f), 
             attentionBoost(1.5f), rehearsalBoost(1.3f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh increases plasticity for memory consolidation
    return 1.0f + pImpl->memoryConsolidationBoost * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f * static_cast<float>(dt));
}

void Acetylcholine::promoteMemoryConsolidation() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.3f);
}

void Acetylcholine::enhanceRehearsal() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

} // namespace nlm

#include "Norepinephrine.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalBoost;
    float alertnessBoost;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.03f), arousalBoost(1.8f), 
             alertnessBoost(1.4f), attentionGain(2.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates learning rate based on arousal
    return 1.0f + pImpl->attentionGain * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.08f * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.25f);
}

void Norepinephrine::enhanceAlertness() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.15f);
}

} // namespace nlm

#include "Serotonin.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float socialLearningBoost;
    float impulsivityReduction;
    float moodEnhancement;
    
    Impl() : level(0.0f), baseline(0.02f), socialLearningBoost(1.5f), 
             impulsivityReduction(0.8f), moodEnhancement(1.2f) {}
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
    // 5-HT promotes stable learning
    return 0.9f + pImpl->moodEnhancement * pImpl->level * 0.1f;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.06f * static_cast<float>(dt));
}

void Serotonin::enhanceSocialLearning() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.2f);
}

void Serotonin::reduceImpulsivity() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

} // namespace nlm