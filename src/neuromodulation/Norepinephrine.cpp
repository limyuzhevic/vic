#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
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
        , decayRate(0.1f)
        , releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and learning rate
    // Higher NE increases plasticity for novel information
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

void Norepinephrine::signalArousal(float strength) {
    // Increase arousal and alertness
    setLevel(std::min(pImpl->peak, getLevel() + strength * pImpl->releaseRate));
}

void Norepinephrine::modulateAttentionThreshold() {
    // NE lowers attention threshold for salient stimuli
    // Enables focused attention on important information
    if (pImpl->brain) {
        auto* attention = pImpl->brain->getAttention();
        if (attention) {
            attention->setExcitationStrength(1.0f + getLevel() * 0.5f);
        }
    }
}

} // namespace nlm
