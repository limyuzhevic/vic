#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

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

NeuromodulatorType Norepinephrine::getType() const {
    return NeuromodulatorType::Norepinephrine;
}

float Norepinephrine::getPlasticityFactor() const {
    if (pImpl->level < 0.3f) {
        return 0.3f;
    } else if (pImpl->level < 0.7f) {
        return 0.6f + pImpl->level * 0.2f;
    } else {
        return 0.8f;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    float decay = 0.02f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Norepinephrine::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    neuron->injectCurrent(factor * pImpl->focusEnhancement);
}

void Norepinephrine::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    if (pImpl->level > 0.3f) {
        ltpWeight *= 1.2f;
        ltdWeight *= 0.9f;
    }
}

void Norepinephrine::increaseArousal() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.15f);
}

void Norepinephrine::enhanceFocus() {
    pImpl->focusEnhancement = 1.5f;
}

void Norepinephrine::modulateLiking() {}

void Norepinephrine::improveWorkingMemory(float capacityIncrease) {
    pImpl->workingMemoryCapacity = std::min(2.0f, pImpl->workingMemoryCapacity + capacityIncrease);
}

} // namespace nlm
