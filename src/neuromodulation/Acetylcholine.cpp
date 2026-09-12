#include "Neuromodulator.hpp"
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
    float learningModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , learningModulation(1.0f) {}
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

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (ACh) system initialized - Attention and memory consolidation");
}

void Acetylcholine::signalAttention(const class Observation& observation) {
    // ACh increases with attention-grabbing stimuli
    float attentionSignal = 1.0f; // Placeholder for actual attention signal computation
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::signalLearning(const std::vector<float>& pattern, float reward) {
    // ACh signals for learning opportunities
    // Higher reward + novel pattern = stronger ACh signal
    float novelty = 1.0f; // Should come from novelty detection
    float learningSignal = (reward + novelty) * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + learningSignal * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation() {
    // ACh is important for memory consolidation during awake states
    // Placeholder: gradual increase during memory-related activities
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates learning rate - higher ACh = higher plasticity for attention-based learning
    float attentionBoost = 1.0f + pImpl->level * 0.5f; // Up to 1.5x normal plasticity
    float memoryBoost = 1.0f + pImpl->level * 0.3f;   // Memory consolidation boost
    
    // ACh particularly enhances Hebbian plasticity for attended stimuli
    return attentionBoost * memoryBoost;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Integrate with brain's attention system
    if (pImpl->brain && pImpl->brain->getAttention()) {
        // ACh modulates attentional selection
        float attentionLevel = pImpl->level;
        pImpl->brain->getAttention()->setAChLevel(attentionLevel);
    }
}
