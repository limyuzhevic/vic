#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilanceLevel;
    float plasticityModulation;
    std::vector<float> focusWeights;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.08f)
        , arousalLevel(1.0f)
        , vigilanceLevel(0.8f)
        , plasticityModulation(1.0f) {}
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
    // Norepinephrine modulates arousal and focus
    float arousalBoost = 1.0f + pImpl->arousalLevel * pImpl->level * 0.5f;
    float vigilanceBoost = 1.0f + pImpl->vigilanceLevel * pImpl->level * 0.3f;
    return arousalBoost * vigilanceBoost;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float event salience, float novelty) {
    // Boost norepinephrine in response to significant events
    if (salience > 0.5f) {
        float boost = salience * novelty;
        pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.5f);
    }
}

void Norepinephrine::enhanceFocus(NeuronId neuronId, float priority, float timeSensitivity) {
    // Enhance firing in focused neuronal populations
    if (pImpl->level > 0.1f) {
        // Would modulate specific neural populations in a real implementation
        float focusEffect = priority * timeSensitivity * pImpl->level;
        if (pImpl->brain) {
            NLM_LOG_INFO("NE enhancing focus with effect " + std::to_string(focusEffect));
        }
    }
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm