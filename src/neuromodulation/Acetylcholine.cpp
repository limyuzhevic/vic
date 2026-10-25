#include "Acetylcholine.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;           // Current acetylcholine level
    float attention;       // Attention modulation (0.0 to 1.0)
    float memoryBoost;     // Memory consolidation boost
    
    Impl() : level(0.0f), attention(0.0f), memoryBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    // Clamp level to reasonable range
    pImpl->level = std::clamp(level, 0.0f, 10.0f);
    
    // Update attention based on level
    pImpl->attention = std::min(pImpl->level / 10.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory encoding and consolidation
    // Higher levels increase attention and memory strength
    return 1.0f + pImpl->attention * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Gradually decay toward baseline
    pImpl->level += (0.1f - pImpl->level) * dt * 0.01f;
    pImpl->attention = std::min(pImpl->level / 10.0f, 1.0f);
}

void Acetylcholine::enhanceMemory(NeuralRegion* region) {
    if (!region) return;
    
    // Boost memory strength in the region
    pImpl->memoryBoost = std::min(pImpl->memoryBoost + pImpl->attention * 0.1f, 1.0f);
    
    // In a real implementation, this would strengthen synaptic connections
    // based on recent activity patterns
}

} // namespace nlm