#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicLevel;
    float attentionWeight;
    float memoryWeight;
    
    // State tracking
    float lastBurstTime;
    float timeSinceLastBurst;
    float eligibilityTrace;
    bool burstMode;
    
    Impl() : level(0.0f), baseline(0.03f), peak(0.8f), decayRate(0.1f), 
             releaseRate(1.5f), tonicLevel(0.03f), attentionWeight(1.0f),
             memoryWeight(1.0f), lastBurstTime(0.0f), timeSinceLastBurst(0.0f),
             eligibilityTrace(0.0f), burstMode(false) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine neuromodulation system initialized");
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->timeSinceLastBurst = 0.0f;
}

void Acetylcholine::signalAttention(float attention) {
    float burstAmount = attention * pImpl->releaseRate;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    pImpl->burstMode = true;
    
    NLM_LOG_DEBUG("Acetylcholine burst on attention: " + std::to_string(attention) + ", new level: " + std::to_string(pImpl->level));
}

void Acetylcholine::signalMemory(float memory) {
    float burstAmount = memory * pImpl->releaseRate * 0.7f;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    
    NLM_LOG_DEBUG("Acetylcholine memory signal: " + std::to_string(memory) + ", new level: " + std::to_string(pImpl->level));
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.1f);
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Acetylcholine::setTonicLevel(float tonic) {
    pImpl->tonicLevel = std::clamp(tonic, 0.0f, 0.1f);
}

float Acetylcholine::getAttentionWeight() const {
    return pImpl->attentionWeight;
}

float Acetylcholine::getMemoryWeight() const {
    return pImpl->memoryWeight;
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = std::clamp(weight, 0.5f, 2.0f);
}

void Acetylcholine::setMemoryWeight(float weight) {
    pImpl->memoryWeight = std::clamp(weight, 0.5f, 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity for attention and memory processes
    if (pImpl->level > 0) {
        float burstMultiplier = pImpl->burstMode ? 1.5f : 1.0f;
        return 0.5f + 0.5f * pImpl->level * burstMultiplier;
    } else {
        return 0.5f;
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    pImpl->timeSinceLastBurst += timeStep;
    
    if (pImpl->level > pImpl->baseline) {
        float decay = pImpl->decayRate * (2.0f - pImpl->level);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * timeStep);
    } else {
        float recovery = 0.03f;
        pImpl->level = std::min(pImpl->baseline, pImpl->level + recovery * timeStep);
    }
    
    if (pImpl->timeSinceLastBurst > 0.5f) {
        pImpl->burstMode = false;
    }
    
    if (std::abs(pImpl->eligibilityTrace) > 0.001f) {
        pImpl->eligibilityTrace *= (1.0f - pImpl->decayRate * timeStep);
    }
}

bool Acetylcholine::isBurstMode() const {
    return pImpl->burstMode && pImpl->timeSinceLastBurst < 0.3f;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->burstMode = false;
    pImpl->timeSinceLastBurst = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    NLM_LOG_INFO("Acetylcholine system reset");
}

} // namespace nlm

// Norepinephrine: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicLevel;
    float arousalWeight;
    float vigilanceWeight;
    
    // State tracking
    float lastBurstTime;
    float timeSinceLastBurst;
    float eligibilityTrace;
    bool burstMode;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.7f), decayRate(0.15f), 
             releaseRate(2.5f), tonicLevel(0.02f), arousalWeight(1.0f),
             vigilanceWeight(1.0f), lastBurstTime(0.0f), timeSinceLastBurst(0.0f),
             eligibilityTrace(0.0f), burstMode(false) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulation system initialized");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->timeSinceLastBurst = 0.0f;
}

void Norepinephrine::signalArousal(float arousal) {
    float burstAmount = arousal * pImpl->releaseRate;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    pImpl->burstMode = true;
    
    NLM_LOG_DEBUG("Norepinephrine burst on arousal: " + std::to_string(arousal) + ", new level: " + std::to_string(pImpl->level));
}

void Norepinephrine::signalVigilance(float vigilance) {
    float burstAmount = vigilance * pImpl->releaseRate * 0.8f;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    
    NLM_LOG_DEBUG("Norepinephrine vigilance signal: " + std::to_string(vigilance) + ", new level: " + std::to_string(pImpl->level));
}

void Norepinephrine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.05f);
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Norepinephrine::setTonicLevel(float tonic) {
    pImpl->tonicLevel = std::clamp(tonic, 0.0f, 0.05f);
}

float Norepinephrine::getArousalWeight() const {
    return pImpl->arousalWeight;
}

float Norepinephrine::getVigilanceWeight() const {
    return pImpl->vigilanceWeight;
}

void Norepinephrine::setArousalWeight(float weight) {
    pImpl->arousalWeight = std::clamp(weight, 0.5f, 2.0f);
}

void Norepinephrine::setVigilanceWeight(float weight) {
    pImpl->vigilanceWeight = std::clamp(weight, 0.5f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal and attention
    if (pImpl->level > 0) {
        float burstMultiplier = pImpl->burstMode ? 1.3f : 1.0f;
        return 0.5f + 0.5f * pImpl->level * burstMultiplier;
    } else {
        return 0.5f;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    pImpl->timeSinceLastBurst += timeStep;
    
    if (pImpl->level > pImpl->baseline) {
        float decay = pImpl->decayRate * (2.0f - pImpl->level);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * timeStep);
    } else {
        float recovery = 0.04f;
        pImpl->level = std::min(pImpl->baseline, pImpl->level + recovery * timeStep);
    }
    
    if (pImpl->timeSinceLastBurst > 0.5f) {
        pImpl->burstMode = false;
    }
    
    if (std::abs(pImpl->eligibilityTrace) > 0.001f) {
        pImpl->eligibilityTrace *= (1.0f - pImpl->decayRate * timeStep);
    }
}

bool Norepinephrine::isBurstMode() const {
    return pImpl->burstMode && pImpl->timeSinceLastBurst < 0.3f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->burstMode = false;
    pImpl->timeSinceLastBurst = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    NLM_LOG_INFO("Norepinephrine system reset");
}

} // namespace nlm

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float tonicLevel;
    float moodWeight;
    float impulsivityWeight;
    
    // State tracking
    float lastBurstTime;
    float timeSinceLastBurst;
    float eligibilityTrace;
    bool burstMode;
    
    Impl() : level(0.0f), baseline(0.01f), peak(0.6f), decayRate(0.08f), 
             releaseRate(2.2f), tonicLevel(0.01f), moodWeight(1.0f),
             impulsivityWeight(1.0f), lastBurstTime(0.0f), timeSinceLastBurst(0.0f),
             eligibilityTrace(0.0f), burstMode(false) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin neuromodulation system initialized");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->timeSinceLastBurst = 0.0f;
}

void Serotonin::signalMood(float mood) {
    float burstAmount = mood * pImpl->releaseRate;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    pImpl->burstMode = true;
    
    NLM_LOG_DEBUG("Serotonin burst on mood: " + std::to_string(mood) + ", new level: " + std::to_string(pImpl->level));
}

void Serotonin::signalSocial(float social) {
    float burstAmount = social * pImpl->releaseRate * 0.6f;
    float newLevel = pImpl->level + burstAmount;
    setLevel(newLevel);
    
    NLM_LOG_DEBUG("Serotonin social signal: " + std::to_string(social) + ", new level: " + std::to_string(pImpl->level));
}

void Serotonin::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.02f);
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Serotonin::setTonicLevel(float tonic) {
    pImpl->tonicLevel = std::clamp(tonic, 0.0f, 0.02f);
}

float Serotonin::getMoodWeight() const {
    return pImpl->moodWeight;
}

float Serotonin::getImpulsivityWeight() const {
    return pImpl->impulsivityWeight;
}

void Serotonin::setMoodWeight(float weight) {
    pImpl->moodWeight = std::clamp(weight, 0.5f, 2.0f);
}

void Serotonin::setImpulsivityWeight(float weight) {
    pImpl->impulsivityWeight = std::clamp(weight, 0.5f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for mood and social behavior
    if (pImpl->level > 0) {
        float burstMultiplier = pImpl->burstMode ? 1.2f : 1.0f;
        return 0.5f + 0.5f * pImpl->level * burstMultiplier;
    } else {
        return 0.5f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    pImpl->timeSinceLastBurst += timeStep;
    
    if (pImpl->level > pImpl->baseline) {
        float decay = pImpl->decayRate * (2.0f - pImpl->level);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * timeStep);
    } else {
        float recovery = 0.02f;
        pImpl->level = std::min(pImpl->baseline, pImpl->level + recovery * timeStep);
    }
    
    if (pImpl->timeSinceLastBurst > 0.5f) {
        pImpl->burstMode = false;
    }
    
    if (std::abs(pImpl->eligibilityTrace) > 0.001f) {
        pImpl->eligibilityTrace *= (1.0f - pImpl->decayRate * timeStep);
    }
}

bool Serotonin::isBurstMode() const {
    return pImpl->burstMode && pImpl->timeSinceLastBurst < 0.3f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->burstMode = false;
    pImpl->timeSinceLastBurst = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    NLM_LOG_INFO("Serotonin system reset");
}

} // namespace nlm