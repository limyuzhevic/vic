#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float workingMemoryModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(0.5f)
        , decayRate(0.05f)
        , attentionGain(2.0f)
        , workingMemoryModulation(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACH";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention and working memory
    // Higher ACh increases attention and working memory capacity
    return 0.8f + 0.4f * pImpl->level;  // 0.8 to 1.2 range
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost(float boostAmount) {
    // Boost attention level
    pImpl->level = std::min(pImpl->peak, pImpl->level + boostAmount);
}

void Acetylcholine::signalMemoryConsolidation(float consolidationStrength) {
    // Enhance memory consolidation
    // ACh is critical for working memory to long-term memory transfer
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationStrength * 0.3f);
}

void Acetylcholine::signalSleepState() {
    // Reduce ACh during sleep/wake transitions
    pImpl->level *= 0.1f;
}

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalGain;
    float locusCoeruleusModulation;
    float tonicPhasicMode;  // 0 = tonic, 1 = phasic
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , arousalGain(1.5f)
        , locusCoeruleusModulation(0.8f)
        , tonicPhasicMode(0.7f) {}
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
    // NE modulates arousal and locus coeruleus
    // Higher NE increases arousal and can trigger phasic bursts
    if (pImpl->tonicPhasicMode > 0.5f) {
        // Phasic mode: strong but transient effects
        return 0.5f + 0.8f * pImpl->level;  // 0.5 to 1.3 range
    } else {
        // Tonic mode: sustained baseline effects
        return 0.8f + 0.3f * pImpl->level;  // 0.8 to 1.1 range
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    if (pImpl->tonicPhasicMode > 0.5f) {
        // Phasic mode: faster decay
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * 2.0f * static_cast<float>(dt));
    } else {
        // Tonic mode: slower decay
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
}

void Norepinephrine::signalArousal(float arousalAmount) {
    // Increase arousal level
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalAmount);
    // Shift to phasic mode for attention demands
    pImpl->tonicPhasicMode = std::max(pImpl->tonicPhasicMode, 0.6f);
}

void Norepinephrine::signalAlertness(float alertness) {
    // Boost alertness for unexpected events
    pImpl->level = std::min(pImpl->peak, pImpl->level + alertness * 0.5f);
}

void Norepinephrine::signalSleep() {
    // Reduce NE during sleep
    pImpl->level *= 0.05f;
    pImpl->tonicPhasicMode = 0.3f;  // Shift to tonic
}

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodGain;
    float impulsivityModulation;
    float rewardProcessing;
    
    Impl() 
        : brain(nullptr)
        , level(0.2f)
        , baseline(0.1f)
        , peak(0.8f)
        , decayRate(0.08f)
        , moodGain(1.0f)
        , impulsivityModulation(0.7f)
        , rewardProcessing(0.9f) {}
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
    // 5-HT modulates mood, impulsivity, and reward processing
    // Optimal serotonin promotes balanced behavior
    if (pImpl->level > 0.6f) {
        // High serotonin: stable, reflective processing
        return 0.7f + 0.2f * (1.0f - pImpl->level);  // 0.7 to 0.9 range
    } else if (pImpl->level < 0.3f) {
        // Low serotonin: high impulsivity
        return 0.9f + 0.3f * (1.0f - pImpl->level);  // 0.9 to 1.2 range
    } else {
        // Optimal serotonin: balanced processing
        return 0.8f;  // Steady baseline
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Stable, slow dynamics
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMoodBoost(float moodBoost) {
    // Improve mood state
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodBoost);
}

void Serotonin::signalRewardProcessing(float rewardSignal) {
    // Enhance reward processing and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal * 0.3f);
}

void Serotonin::signalInhibition(float inhibition) {
    // Increase behavioral inhibition
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibition * 0.2f);
}

void Serotonin::signalSleep() {
    // Reduce serotonin during sleep
    pImpl->level *= 0.2f;
}

} // namespace nlm
