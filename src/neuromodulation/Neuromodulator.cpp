#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.5f), decayRate(0.1f) {}
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
    // Acetylcholine enhances memory encoding and consolidation
    float normalized = std::clamp(pImpl->level, 0.0f, 1.0f);
    return 0.5f + 0.5f * normalized;
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
}

void Acetylcholine::updateAttention(float currentLevel, float &attentionWeight) {
    // Acetylcholine modulates attentional focus and memory encoding
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    attentionWeight = 0.5f + normalized * 0.8f;
    attentionWeight = std::clamp(attentionWeight, 0.2f, 2.0f);
}

void Acetylcholine::updateWorkingMemory(float currentLevel, float &memoryStability) {
    // Acetylcholine enhances working memory
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    memoryStability = 0.6f + normalized * 0.5f;
}

void Acetylcholine::updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) {
    // Acetylcholine supports developmental learning
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    developmentalFactor = 0.5f + normalized * 0.5f;
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.3f), decayRate(0.15f) {}
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
    return 0.8f + 0.2f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
}

void Norepinephrine::updateAttention(float currentLevel, float &attentionWeight) {
    // Norepinephrine enhances arousal and vigilance
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    attentionWeight = 0.7f + normalized * 0.6f;
    attentionWeight = std::clamp(attentionWeight, 0.3f, 2.0f);
}

void Norepinephrine::updateWorkingMemory(float currentLevel, float &memoryStability) {
    // Norepinephrine modulates working memory and focus
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    memoryStability = 0.4f + normalized * 0.4f;
}

void Norepinephrine::updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) {
    // Norepinephrine supports developmental arousal regulation
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    developmentalFactor = 0.6f + normalized * 0.3f;
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.4f), decayRate(0.12f) {}
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
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeStep);
}

void Serotonin::updateAttention(float currentLevel, float &attentionWeight) {
    // Serotonin modulates mood and social behavior
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    attentionWeight = 0.6f + normalized * 0.5f;
    attentionWeight = std::clamp(attentionWeight, 0.2f, 2.0f);
}

void Serotonin::updateWorkingMemory(float currentLevel, float &memoryStability) {
    // Serotonin influences working memory and impulsivity
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    memoryStability = 0.5f + normalized * 0.4f;
}

void Serotonin::updateDevelopmentalPlasticity(float currentLevel, float &developmentalFactor) {
    // Serotonin supports developmental social and emotional regulation
    float normalized = std::clamp(currentLevel, 0.0f, 1.0f);
    developmentalFactor = 0.5f + normalized * 0.3f;
}

} // namespace nlm
