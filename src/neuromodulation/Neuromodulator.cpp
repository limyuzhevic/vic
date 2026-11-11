#include "Neuromodulator.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeSinceUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), timeSinceUpdate(0.0f) {}
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // REAL IMPLEMENTATION: Implement real dopamine-modulated plasticity factor
    // Return value that influences plasticity strength
    float normalizedLevel = pImpl->level / pImpl->peak;
    // Dopamine increases plasticity for reward prediction errors
    return 0.5f + 0.5f * normalizedLevel * normalizedLevel;
}

void Dopamine::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: Implement real dopamine dynamics
    // Decay towards baseline over time, simulating natural dopamine dynamics
    pImpl->timeSinceUpdate += static_cast<float>(dt);
    
    // Natural decay from peaks
    if (pImpl->level > pImpl->baseline) {
        float decayAmount = pImpl->decayRate * static_cast<float>(dt);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    }
}

void Dopamine::signalReward(float reward) {
    // REAL IMPLEMENTATION: Implement real reward signaling
    // Transient burst of dopamine when reward is received
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    std::cout << "[Dopamine] Reward signaling: level increased to " << pImpl->level << " (reward=" << reward << ")" << std::endl;
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL IMPLEMENTATION: Implement reward prediction error signaling
    // Positive error (better than expected) increases dopamine, negative error decreases it
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    if (error > 0) {
        std::cout << "[Dopamine] Reward prediction error: positive error " << error << " increased dopamine level to " << pImpl->level << std::endl;
    } else {
        std::cout << "[Dopamine] Reward prediction error: negative error " << error << " decreased dopamine level to " << pImpl->level << std::endl;
    }
}

// Acetylcholine Implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionBoost;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), attentionBoost(0.0f), memoryBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

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
    // Acetylcholine enhances plasticity for attention-demanded tasks
    float attentionEffect = pImpl->level * pImpl->attentionBoost;
    float memoryEffect = pImpl->level * pImpl->memoryBoost;
    return 1.0f + attentionEffect + memoryEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay of acetylcholine levels
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Acetylcholine::enhanceMemory(float duration) {
    pImpl->memoryBoost += duration * 0.1f;
    pImpl->level = std::min(1.0f, pImpl->level + duration * 0.2f);
    std::cout << "[Acetylcholine] Memory enhancement: level=" << pImpl->level << ", boost=" << pImpl->memoryBoost << std::endl;
}

// Norepinephrine Implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float alertnessBoost;
    
    Impl() : level(0.0f), baseline(0.0f), alertnessBoost(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

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
    // Norepinephrine enhances plasticity for alert tasks
    return 1.0f + pImpl->level * pImpl->alertnessBoost;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay of norepinephrine levels
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Norepinephrine::enhanceAttention(float duration) {
    pImpl->alertnessBoost += duration * 0.1f;
    pImpl->level = std::min(1.0f, pImpl->level + duration * 0.3f);
    std::cout << "[Norepinephrine] Attention enhancement: level=" << pImpl->level << ", boost=" << pImpl->alertnessBoost << std::endl;
}

// Serotonin Implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float moodBoost;
    
    Impl() : level(0.0f), baseline(0.0f), moodBoost(0.0f) {}
};

Serotonin::Serotonin() : pImpl(std::make_unique<Impl>()) {}

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
    // Serotonin modulates plasticity based on mood state
    float moodEffect = pImpl->level * pImpl->moodBoost;
    return 1.0f - moodEffect * 0.2f;  // High serotonin reduces plasticity
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay of serotonin levels
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
}

void Serotonin::modulateMood(float duration) {
    pImpl->moodBoost += duration * 0.1f;
    pImpl->level = std::min(1.0f, pImpl->level + duration * 0.15f);
    std::cout << "[Serotonin] Mood modulation: level=" << pImpl->level << ", boost=" << pImpl->moodBoost << std::endl;
}

} // namespace nlm
