#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float maxLevel;
    float decayRate;
    float potentiationRate;
    std::vector<float> memoryTrace;
    float attentionBoost;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , maxLevel(1.0f)
        , decayRate(0.1f)
        , potentiationRate(0.3f)
        , memoryTrace()
        , attentionBoost(1.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (attention) system initialized");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory consolidation
    return 1.0f + pImpl->level * pImpl->potentiationRate;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay of ACh signaling
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float amount) {
    setLevel(getLevel() + amount * pImpl->attentionBoost);
}

void Acetylcholine::storeMemoryTrace(const std::vector<float>& trace) {
    pImpl->memoryTrace = trace;
    // ACh bursts during memory encoding
    if (!trace.empty()) {
        float avgTrace = std::accumulate(trace.begin(), trace.end(), 0.0f) / trace.size();
        setLevel(avgTrace * pImpl->potentiationRate);
    }
}

const std::vector<float>& Acetylcholine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->memoryTrace.clear();
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setAttentionBoost(float boost) {
    pImpl->attentionBoost = boost;
}

} // namespace nlm