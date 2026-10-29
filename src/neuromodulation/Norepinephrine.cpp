#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float vigilanceBoost;
    float locusCoeruleusInputs;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.15f)
        , arousalBoost(1.5f)
        , vigilanceBoost(2.0f)
        , locusCoeruleusInputs(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getArousalFactor() const {
    return 1.0f + pImpl->level * (pImpl->arousalBoost - 1.0f);
}

float Norepinephrine::getVigilanceFactor() const {
    return 1.0f + pImpl->level * (pImpl->vigilanceBoost - 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances attention-modulated plasticity
    return 1.1f + 0.3f * pImpl->level;
}

void Norepinephrine::signalStressOrAttention(float input) {
    // TODO PHASE 2: Implement real NE dynamics
    // PLACEHOLDER: NE responds to stress, novelty, attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + input * pImpl->releaseRate);
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real NE dynamics
    // PLACEHOLDER: NE decays with feedback loops
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->locusCoeruleusInputs = std::max(0.0f, pImpl->locusCoeruleusInputs - 0.05f * static_cast<float>(dt));
}

void Norepinephrine::increaseArousal() {
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // NE increases neural excitability via alpha-2 receptor modulation
                    neuron->injectCurrent(pImpl->level * 0.3f);
                }
            }
        }
    }
}

void Norepinephrine::boostVigilance() {
    if (pImpl->brain && pImpl->brain->getAttention()) {
        auto* attention = pImpl->brain->getAttention();
        attention->boostVigilance(0.3f);
    }
}

} // namespace nlm