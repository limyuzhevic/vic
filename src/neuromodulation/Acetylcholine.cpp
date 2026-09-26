#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float learningRate;
    float memoryConsolidationStrength;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.05f), learningRate(0.1f), memoryConsolidationStrength(0.8f) {}
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
    // Acetylcholine enhances plasticity for attention and working memory
    return 1.0f + pImpl->level * pImpl->learningRate;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * static_cast<float>(dt));
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory consolidation");
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Boost level based on attention demands
    pImpl->level = std::min(1.0f, pImpl->level + attentionSignal * 0.2f);
}

void Acetylcholine::signalMemoryRehearsal() {
    // Increase level for memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

} // namespace nlm