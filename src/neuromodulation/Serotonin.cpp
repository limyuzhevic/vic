#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodWeight;
    float socialWeight;
    float decayRate;
    float baseline;
    
    Impl() 
        : brain(nullptr)
        , level(0.5f)  // Baseline mood level
        , moodWeight(0.6f)
        , socialWeight(0.4f)
        , decayRate(0.05f)
        , baseline(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood and social regulation");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(float moodState, float socialInteraction, TimestepDuration dt) {
    // Target level based on mood state and social interactions
    float target = pImpl->moodWeight * moodState + 
                   pImpl->socialWeight * socialInteraction;
    
    // Smooth update towards target
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Natural decay towards baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * pImpl->decayRate * static_cast<float>(dt);
    
    // Keep within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Serotonin::getImpulsivityEffect() const {
    // Higher serotonin reduces impulsivity (inverted U-curve)
    return 1.0f - pImpl->level;  // Range [0, 1], higher = less impulsive
}

float Serotonin::getSocialBondingEffect() const {
    // Higher serotonin increases social bonding
    return pImpl->level;  // Range [0, 1], higher = stronger bonding
}

void Serotonin::setMoodWeight(float weight) {
    pImpl->moodWeight = weight;
}

void Serotonin::setSocialWeight(float weight) {
    pImpl->socialWeight = weight;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

// Norepinephrine implementation

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousalWeight;
    float attentionWeight;
    float decayRate;
    float baseline;
    
    Impl() 
        : brain(nullptr)
        , level(0.3f)  // Baseline arousal level
        , arousalWeight(0.5f)
        , attentionWeight(0.5f)
        , decayRate(0.05f)
        , baseline(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(float arousalLevel, float attentionDemands, TimestepDuration dt) {
    // Target level based on arousal and attention demands
    float target = pImpl->arousalWeight * arousalLevel + 
                   pImpl->attentionWeight * attentionDemands;
    
    // Smooth update towards target
    pImpl->level += (target - pImpl->level) * 0.15f;
    
    // Natural decay towards baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * pImpl->decayRate * static_cast<float>(dt);
    
    // Keep within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Norepinephrine::getSignalDetectionEffect() const {
    // Inverted U-curve: moderate NE optimal for signal detection
    float optimal = 0.5f;
    float distance = std::abs(pImpl->level - optimal);
    return std::max(0.0f, 1.0f - 2.0f * distance);  // Range [0, 1]
}

float Norepinephrine::getWorkingMemoryEffect() const {
    // Inverted U-curve: moderate NE optimal for working memory
    float optimal = 0.5f;
    float distance = std::abs(pImpl->level - optimal);
    return std::max(0.0f, 1.0f - 2.0f * distance);  // Range [0, 1]
}

void Norepinephrine::setArousalWeight(float weight) {
    pImpl->arousalWeight = weight;
}

void Norepinephrine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = weight;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

// Acetylcholine implementation

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float memoryWeight;
    float decayRate;
    float baseline;
    
    Impl() 
        : brain(nullptr)
        , level(0.4f)  // Baseline attention level
        , attentionWeight(0.6f)
        , memoryWeight(0.4f)
        , decayRate(0.05f)
        , baseline(0.4f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory encoding");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(float attentionDemands, float memoryEncodingNeeds, TimestepDuration dt) {
    // Target level based on attention and memory encoding needs
    float target = pImpl->attentionWeight * attentionDemands + 
                   pImpl->memoryWeight * memoryEncodingNeeds;
    
    // Smooth update towards target
    pImpl->level += (target - pImpl->level) * 0.12f;
    
    // Natural decay towards baseline
    pImpl->level += (pImpl->baseline - pImpl->level) * pImpl->decayRate * static_cast<float>(dt);
    
    // Keep within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Acetylcholine::getCorticalActivationEffect() const {
    // Higher ACh increases cortical activation
    return pImpl->level;  // Range [0, 1]
}

float Acetylcholine::getMemoryEncodingEffect() const {
    // Higher ACh improves memory encoding
    return pImpl->level;  // Range [0, 1]
}

float Acetylcholine::getAttentionFilteringEffect() const {
    // Inverted U-curve: moderate ACh optimal for attention filtering
    float optimal = 0.5f;
    float distance = std::abs(pImpl->level - optimal);
    return std::max(0.0f, 1.0f - 2.0f * distance);  // Range [0, 1]
}

void Acetylcholine::setAttentionWeight(float weight) {
    pImpl->attentionWeight = weight;
}

void Acetylcholine::setMemoryWeight(float weight) {
    pImpl->memoryWeight = weight;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
