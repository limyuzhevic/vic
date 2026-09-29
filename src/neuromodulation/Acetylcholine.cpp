#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionBias;
    float memoryConsolidationRate;
    
    // For memory consolidation tracking
    std::vector<float> memoryTraceStabilities;
    
    Impl() : level(0.0f), attentionBias(0.0f), memoryConsolidationRate(0.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // ACh affects attention and memory
    pImpl->attentionBias = level * 2.0f;
    pImpl->memoryConsolidationRate = 0.3f + level * 0.7f;
    
    NLM_LOG_INFO(std::string("Acetylcholine: Level = ") + std::to_string(level) + 
                " Attention bias = " + std::to_string(pImpl->attentionBias));
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances both LTP and LTD
    return std::clamp(1.0f + pImpl->level * 0.3f, 0.8f, 1.8f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level *= 0.9f;
    
    // Update attention bias
    pImpl->attentionBias = pImpl->level * 2.0f;
}

} // namespace nlm