#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class Brain;

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation parameters
    float attentionGain;
    float memoryConsolidationBoost;
    
    // State tracking
    bool isAwake;
    float lastAttentionUpdate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionGain(1.5f)
        , memoryConsolidationBoost(2.0f)
        , isAwake(false)
        , lastAttentionUpdate(0.0f) {}
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
    // Update arousal state
    pImpl->isAwake = (pImpl->level > 0.1f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attended items
    return 1.0f + pImpl->memoryConsolidationBoost * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention if brain is available
    if (pImpl->brain) {
        updateAttention(*pImpl->brain);
    }
}

void Acetylcholine::signalAttention(float salience, float predictionError) {
    // Increase level based on stimulus salience and prediction error
    float boost = salience * 0.5f + std::abs(predictionError) * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    pImpl->lastAttentionUpdate = static_cast<float>(getCurrentTime());
}

void Acetylcholine::signalMemoryEvent(float strength, TimestepDuration dt) {
    // Boost level for memory consolidation events
    if (strength > 0.5f) {
        float boost = strength * 0.3f + memoryConsolidationBoost;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    }
}

void Acetylcholine::setArousalState(bool isAwake) {
    pImpl->isAwake = isAwake;
    if (!isAwake) {
        // Reduce level when going to sleep
        pImpl->level = pImpl->baseline * 0.5f;
    }
}

bool Acetylcholine::isAwake() const {
    return pImpl->isAwake;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain + pImpl->level;
}

void Acetylcholine::updateAttention(Brain& brain) {
    // Apply attention modulation to working memory
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        // Increase priority of recently active memories
        auto& traces = workingMemory->getActiveTraces();
        for (auto& trace : traces) {
            trace.importance *= getAttentionGain();
        }
    }
}

void Acetylcholine::registerAttentionHandler(void (Brain::*handler)()) {
    // Placeholder for attention signal registration
}

float Acetylcholine::getCurrentTime() const {
    // This should be replaced with actual time from simulation clock
    return 0.0f;
}

} // namespace nlm
