#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float plasticityModulation;
    
    // Memory consolidation
    std::vector<float> memoryTrace;
    float consolidationFactor;
    
    Impl()
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.05f)
        , plasticityModulation(1.0f)
        , consolidationFactor(0.9f) {}
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
    // ACh enhances memory consolidation and attention
    // Higher levels increase plasticity for memory-related processes
    pImpl->plasticityModulation = 1.0f + pImpl->level * 0.5f;
    return pImpl->plasticityModulation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics with exponential decay
    if (dt > 0.0) {
        float decayFactor = std::exp(-pImpl->decayRate * dt);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Attention modulation based on external stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * 2.0f);
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryTrace) {
    // Memory consolidation process
    pImpl->memoryTrace = memoryTrace;
    
    // Update plasticity based on memory strength
    float memoryStrength = 0.0f;
    if (!memoryTrace.empty()) {
        float sum = 0.0f;
        for (float val : memoryTrace) {
            sum += std::abs(val);
        }
        memoryStrength = sum / memoryTrace.size();
    }
    
    // Stronger memories get more consolidation
    pImpl->plasticityModulation = 1.0f + memoryStrength * 0.3f;
}

float Acetylcholine::getMemoryConsolidationValue() const {
    // Return memory consolidation value
    float memoryValue = 0.0f;
    if (!pImpl->memoryTrace.empty()) {
        float sum = 0.0f;
        for (float val : pImpl->memoryTrace) {
            sum += val;
        }
        memoryValue = sum / pImpl->memoryTrace.size();
    }
    return std::max(0.0f, memoryValue);
}

} // namespace nlm
