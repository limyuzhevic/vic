#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;
    float memoryModulator;
    float cholinergicNoise;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.8f), decayRate(0.08f), releaseRate(1.5f),
             attentionModulator(1.0f), memoryModulator(1.0f), cholinergicNoise(0.0f) {}
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
    // Update attention and memory modulators based on level
    pImpl->attentionModulator = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryModulator = 1.0f + pImpl->level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity and attention
    return 0.8f * (1.0f + pImpl->level * 0.4f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // More realistic acetylcholine dynamics
    float timeFactor = std::min(1.0f, static_cast<float>(dt) / 0.1f);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeFactor);
    
    // Add small amount of noise to simulate biological variability
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::normal_distribution<float> noiseDist(0.0f, 0.01f);
    pImpl->cholinergicNoise = noiseDist(gen);
    pImpl->level += pImpl->cholinergicNoise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

// Norepinephrine: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    Timestamp lastStressEvent;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f), stressLevel(0.0f), lastStressEvent(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal and vigilance based on level
    pImpl->arousalLevel = 1.0f + pImpl->level * 0.6f;
    pImpl->vigilanceLevel = 1.0f + pImpl->level * 0.3f;
    pImpl->stressLevel = pImpl->level * 0.4f;
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity based on arousal
    float arousalMod = pImpl->arousalLevel * 0.5f;
    float stressMod = std::min(1.0f, pImpl->stressLevel * 2.0f);
    return 0.7f + arousalMod - stressMod * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // More realistic norepinephrine dynamics with stress response
    float timeFactor = std::min(1.0f, static_cast<float>(dt) / 0.1f);
    
    // Decay with stress-dependent recovery
    float recoveryRate = pImpl->decayRate * (1.0f + pImpl->stressLevel * 0.5f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - recoveryRate * timeFactor);
}

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorLevel;
    float satietyLevel;
    
    Impl() : level(0.0f), baseline(0.03f), peak(0.9f), decayRate(0.12f), releaseRate(1.2f),
             moodLevel(1.0f), impulsivityLevel(1.0f), socialBehaviorLevel(1.0f), satietyLevel(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update mood and behavioral modulators based on level
    pImpl->moodLevel = 1.0f + pImpl->level * 0.7f;
    pImpl->impulsivityLevel = std::max(0.1f, 1.0f - pImpl->level * 0.5f);
    pImpl->socialBehaviorLevel = 1.0f + pImpl->level * 0.4f;
    pImpl->satietyLevel = pImpl->level * 0.3f;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and impulsivity-related plasticity
    float moodMod = pImpl->moodLevel * 0.3f;
    float impulsivityMod = (1.0f - pImpl->impulsivityLevel) * 0.2f;
    return 0.9f + moodMod - impulsivityMod * 0.1f;
}

void Serotonin::update(TimestepDuration dt) {
    // More realistic serotonin dynamics with satiety feedback
    float timeFactor = std::min(1.0f, static_cast<float>(dt) / 0.1f);
    
    // Decay with satiety-dependent recovery
    float recoveryRate = pImpl->decayRate * (1.0f + pImpl->satietyLevel * 0.3f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - recoveryRate * timeFactor);
}

} // namespace nlm
