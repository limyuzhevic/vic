#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float attention;
    float memoryConsolidation;
    float corticalActivation;
    float learningRate;
    float workingMemoryBoost;
    float encodingStrength;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), decayRate(0.08f),
             attention(0.5f), memoryConsolidation(0.5f), corticalActivation(0.5f),
             learningRate(0.5f), workingMemoryBoost(0.5f), encodingStrength(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.5f);  // ACh can go up to 1.5
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity, especially for attention-based learning
    return 0.4f + 0.6f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine dynamics: slower rise, medium decay
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention-related functions
    pImpl->attention = 0.2f + 0.8f * pImpl->level;
    pImpl->memoryConsolidation = 0.3f + 0.7f * pImpl->level;
    pImpl->corticalActivation = 0.4f + 0.6f * pImpl->level;
    pImpl->learningRate = 0.3f + 0.7f * pImpl->level;
    pImpl->workingMemoryBoost = 0.3f + 0.7f * pImpl->level;
    pImpl->encodingStrength = 0.4f + 0.6f * pImpl->level;
}

void Acetylcholine::signalAttention(float magnitude) {
    // Acetylcholine signals attention and focus
    pImpl->level = std::min(1.5f, pImpl->level + magnitude * 0.8f);
}

void Acetylcholine::signalLearning(float magnitude) {
    // Acetylcholine enhances learning and encoding
    pImpl->level = std::min(1.5f, pImpl->level + magnitude * 0.6f);
}

void Acetylcholine::signalMemoryFormation(float magnitude) {
    // Acetylcholine is crucial for memory formation and consolidation
    pImpl->level = std::min(1.5f, pImpl->level + magnitude * 0.7f);
}

void Acetylcholine::setAttentionModifier(float modifier) {
    pImpl->attention = std::clamp(modifier, 0.0f, 1.0f);
}

float Acetylcholine::getAttention() const {
    return pImpl->attention;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getCorticalActivation() const {
    return pImpl->corticalActivation;
}

float Acetylcholine::getLearningRate() const {
    return pImpl->learningRate;
}

float Acetylcholine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

float Acetylcholine::getEncodingStrength() const {
    return pImpl->encodingStrength;
}

void Acetylcholine::clearHistory() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm