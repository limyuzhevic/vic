#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionBoost;
    float memoryBoost;
    float cholineStore;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.1f)
        , attentionBoost(2.0f)
        , memoryBoost(1.5f)
        , cholineStore(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionFactor() const {
    return 1.0f + pImpl->level * (pImpl->attentionBoost - 1.0f);
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return 1.0f + pImpl->level * (pImpl->memoryBoost - 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances LTP (long-term potentiation) but reduces LTD (long-term depression)
    return 1.2f - 0.2f * pImpl->level;
}

void Acetylcholine::signalCholinergicBurst() {
    // TODO PHASE 2: Implement real cholinergic signaling
    // PLACEHOLDER: Burst of acetylcholine on attention or learning events
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline, slow recovery
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->cholineStore = std::max(0.0f, pImpl->cholineStore - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention() {
    if (pImpl->brain && pImpl->brain->getAttention()) {
        auto* attention = pImpl->brain->getAttention();
        attention->boostPriority(0.5f);
    }
}

void Acetylcholine::boostMemoryConsolidation() {
    if (pImpl->brain && pImpl->brain->getEpisodicMemory()) {
        auto* episodicMemory = pImpl->brain->getEpisodicMemory();
        episodicMemory->boostConsolidation(0.3f);
    }
}

} // namespace nlm