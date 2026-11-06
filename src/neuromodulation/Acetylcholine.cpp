#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionGain;
    float memoryConsolidationRate;
    float decayRate;
    float baseline;
    float peak;
    float cholinergicTone;
    
    Impl() : brain(nullptr), level(0.0f), attentionGain(1.0f), memoryConsolidationRate(1.0f), 
             decayRate(0.05f), baseline(0.0f), peak(2.0f), cholinergicTone(0.5f) {}
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
    // ACh enhances plasticity for learning new information
    // High ACh increases learning rate, especially for novel stimuli
    return 0.5f + pImpl->level * 1.5f;  // 0.5 to 2.0x modulation
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention gain based on current level
    // ACh increases signal-to-noise ratio in cortical areas
    // Stronger ACh = better filtering of relevant information
}

void Acetylcholine::enhanceAttention(float predictionError, float novelty) {
    // ACh responds to prediction errors and novel information
    // This is the "alertness signal" mechanism
    float attentionSignal = predictionError * 0.7f + novelty * 0.3f;
    
    // Update level based on attention signal
    if (attentionSignal > pImpl->level) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * 0.5f);
    }
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh peaks during learning and promotes memory consolidation
    // This is the "learning signal" that determines what gets stored
    pImpl->level *= 1.2f;  // Enhance cholinergic tone during consolidation
}

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory modulation ready");
}

} // namespace nlm