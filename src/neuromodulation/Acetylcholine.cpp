#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f) {}
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
    // Acetylcholine modulates plasticity for memory consolidation
    // Higher ACh enhances LTP for strong memories, LTD for weak ones
    return 0.7f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

void Acetylcholine::signalAttention(float strength) {
    // Boost attention and memory encoding
    setLevel(std::min(pImpl->peak, getLevel() + strength * pImpl->releaseRate));
}

void Acetylcholine::enhanceMemoryConsolidation() {
    // Enhance synaptic strengthening for recently active pathways
    if (pImpl->brain) {
        auto* workingMem = pImpl->brain->getWorkingMemory();
        if (workingMem) {
            workingMem->boostSynapticStrength();
        }
    }
}

} // namespace nlm
