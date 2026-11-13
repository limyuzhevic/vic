// Acetylcholine: Attention and memory consolidation
// PHASE 2 FIXED: Real ACh dynamics for attention

#include "Neuromodulator.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

struct Acetylcholine::Impl {
    // Current ACh level (tonal component)
    float level;
    
    // Attention state
    bool isAttending;
    std::chrono::steady_clock::time_point attentionStart;
    float attentionDuration;
    
    // Memory enhancement factors
    float memoryEnhancement;
    float consolidationRate;
    
    // Time constants
    static constexpr float ATTENTION_DECAY_TIME = 200.0f;     // ms - attention decay
    static constexpr float CONSOLIDIDATION_TIME = 500.0f;    // ms - memory consolidation
    static constexpr float MAX_ATTENTION = 2.0f;            // Maximum attention level
    
    Impl() : 
        level(0.1f),  // Baseline attention
        isAttending(false),
        attentionStart(),
        attentionDuration(0.0f),
        memoryEnhancement(1.0f),
        consolidationRate(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, Impl::MAX_ATTENTION);
}

float Acetylcholine::getPlasticityFactor() const {
    // PHASE 2 FIX: Real ACh modulation of plasticity
    // ACh enhances plasticity during attention and memory consolidation
    
    float plasticityFactor = 1.0f;
    
    // During attention, significantly enhance plasticity
    if (pImpl->isAttending) {
        plasticityFactor *= 1.5f;
    }
    
    // Memory consolidation phase enhances plasticity
    if (pImpl->memoryEnhancement > 1.0f) {
        plasticityFactor *= 1.2f;
    }
    
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // PHASE 2 FIX: Real ACh dynamics
    // Natural decay and attention management
    
    float timeFactor = static_cast<float>(dt) / 0.001f;
    
    // Natural decay of ACh level
    float decayRate = 1.0f - std::exp(-timeFactor / Impl::ATTENTION_DECAY_TIME);
    pImpl->level *= (1.0f - decayRate * 0.05f);
    
    // Update attention duration if attending
    if (pImpl->isAttending) {
        pImpl->attentionDuration += timeFactor;
        if (pImpl->attentionDuration >= 1000.0f) {  // 1 second max attention
            stopAttention();
        }
    }
    
    // Decay memory enhancement
    if (pImpl->memoryEnhancement > 1.0f) {
        pImpl->memoryEnhancement *= (1.0f - timeFactor / Impl::CONSOLIDIDATION_TIME);
        if (pImpl->memoryEnhancement < 1.0f) {
            pImpl->memoryEnhancement = 1.0f;
        }
    }
}

float Acetylcholine::getExcitabilityModulator() const {
    // PHASE 2 FIX: ACh modulates neural excitability
    // During attention, increase signal-to-noise ratio
    float modulator = 1.0f;
    
    if (pImpl->isAttending) {
        modulator *= 1.3f;
    }
    
    return modulator;
}

void Acetylcholine::startAttention(float duration) {
    pImpl->isAttending = true;
    pImpl->attentionStart = std::chrono::steady_clock::now();
    pImpl->attentionDuration = 0.0f;
    pImpl->attentionDuration = duration;
}

void Acetylcholine::stopAttention() {
    pImpl->isAttending = false;
    // End attention phase enhances memory consolidation
    pImpl->memoryEnhancement = 1.5f;
}

bool Acetylcholine::isAttending() const {
    return pImpl->isAttending;
}

} // namespace nlm