#include "Acetylcholine.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryConsolidation;
    float corticalPlasticity;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)  // Baseline ACh level ~5%
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionGain(1.0f)
        , memoryConsolidation(0.0f)
        {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - modulating attention and memory");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived state
    updateDerivedState();
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update memory consolidation based on ACh level
    pImpl->memoryConsolidation = pImpl->level * 0.8f;  // Strong correlation with memory
    
    // Log level changes for debugging
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Acetylcholine::updateDerivedState() {
    // Update cortical plasticity based on ACh level
    pImpl->corticalPlasticity = pImpl->level * 0.7f;
    
    // Modulate attention in the brain
    if (pImpl->brain) {
        pImpl->attentionGain = 0.5f + pImpl->level * 1.5f;  // Range [0.5, 2.0]
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances both Hebbian and STDP plasticity
    // Modulates synaptic strengthening during learning
    return 0.7f + pImpl->level * 0.6f;  // Range [0.7, 1.3]
}

void Acetylcholine::signalAttentionEvent() {
    // Burst of ACh on attention-grabbing events
    setLevel(std::min(pImpl->peak, pImpl->level + 0.8f * pImpl->releaseRate));
}

void Acetylcholine::signalMemoryEvent() {
    // Sustained increase for memory consolidation
    setLevel(std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate));
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 3.0f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getCorticalPlasticity() const {
    return pImpl->corticalPlasticity;
}

const std::vector<float>& Acetylcholine::getHistory() const {
    return pImpl->history;
}

void Acetylcholine::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
