#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionGain;
    float workingMemoryCapacity;
    float signalToNoiseRatio;
    float memoryEncodingEfficiency;
    float memoryConsolidationStrength;
    float memoryRetrievalAccuracy;
    float baselineAttention;
    float decayRate;
    float releaseRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , attentionGain(1.0f)
        , workingMemoryCapacity(1.0f)
        , signalToNoiseRatio(1.0f)
        , memoryEncodingEfficiency(0.8f)
        , memoryConsolidationStrength(0.7f)
        , memoryRetrievalAccuracy(0.9f)
        , baselineAttention(0.3f)
        , decayRate(0.1f)
        , releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity, particularly for memory-related processes
    return 0.5f + 0.5f * pImpl->level * 0.9f;  // More sensitive than dopamine
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineAttention, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate related functions based on current level
    pImpl->attentionGain = 1.0f + pImpl->level * 2.0f;
    pImpl->workingMemoryCapacity = 0.5f + pImpl->level * 0.5f;
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 1.5f;
    pImpl->memoryEncodingEfficiency = 0.8f + pImpl->level * 0.2f;
    pImpl->memoryConsolidationStrength = 0.7f + pImpl->level * 0.3f;
}

void Acetylcholine::signalAttention(float attentionalDemand) {
    // Enhance cholinergic response based on attentional demand
    pImpl->level = std::min(1.0f, pImpl->level + attentionalDemand * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::signalMemory(float memoryLoad) {
    // Boost memory-related functions when memory load is high
    float memoryEffect = std::min(1.0f, memoryLoad * 2.0f);
    pImpl->level = std::min(1.0f, pImpl->level + memoryEffect * pImpl->releaseRate * 0.3f);
}

void Acetylcholine::signalEncoding(float encodingRate) {
    // Facilitate encoding processes
    pImpl->level = std::min(1.0f, pImpl->level + encodingRate * pImpl->releaseRate * 0.4f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getWorkingMemoryCapacity() const {
    return pImpl->workingMemoryCapacity;
}

float Acetylcholine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoiseRatio;
}

float Acetylcholine::getMemoryEncodingEfficiency() const {
    return pImpl->memoryEncodingEfficiency;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidationStrength;
}

float Acetylcholine::getMemoryRetrievalAccuracy() const {
    return pImpl->memoryRetrievalAccuracy;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionGain = 1.0f;
    pImpl->workingMemoryCapacity = 1.0f;
    pImpl->signalToNoiseRatio = 1.0f;
    pImpl->memoryEncodingEfficiency = 0.8f;
    pImpl->memoryConsolidationStrength = 0.7f;
    pImpl->memoryRetrievalAccuracy = 0.9f;
}

} // namespace nlm