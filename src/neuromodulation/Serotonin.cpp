#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;  // Serotonin's effect on attentional focus
    float memoryConsolidation;  // Serotonin's effect on memory consolidation
    float socialDrive;  // Serotonin's effect on social motivation
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , attentionModulation(1.0f)
        , memoryConsolidation(1.0f)
        , socialDrive(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update derived effects based on serotonin level
    pImpl->attentionModulation = 1.0f - pImpl->level * 0.5f;  // High serotonin reduces attentional scope
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 0.3f;   // High serotonin enhances memory consolidation
    pImpl->socialDrive = 0.5f + pImpl->level * 0.5f;            // High serotonin increases social motivation
}

float Serotonin::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Serotonin::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidation;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay toward baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived effects
    pImpl->attentionModulation = 1.0f - pImpl->level * 0.5f;
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 0.3f;
    pImpl->socialDrive = 0.5f + pImpl->level * 0.5f;
}

void Serotonin::signalSocialReward(float reward) {
    // Serotonin increases with positive social experiences
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + reward * pImpl->releaseRate);
}

void Serotonin::signalSocialPunishment(float punishment) {
    // Serotonin decreases with negative social experiences
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - punishment * pImpl->releaseRate);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionModulation = 1.0f;
    pImpl->memoryConsolidation = 1.0f;
    pImpl->socialDrive = 0.5f;
}

} // namespace nlm