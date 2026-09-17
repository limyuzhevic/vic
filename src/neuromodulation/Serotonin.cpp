#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

NeuromodulatorType Serotonin::getType() const {
    return NeuromodulatorType::Serotonin;
}

float Serotonin::getPlasticityFactor() const {
    if (pImpl->level < 0.3f) {
        return 1.3f;
    } else {
        return 0.7f;
    }
}

void Serotonin::update(TimestepDuration dt) {
    float decay = 0.01f * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Serotonin::applyToNeuron(Neuron* neuron, float factor) {
    if (!neuron) return;
    neuron->injectCurrent(factor * pImpl->moodEnhancement);
}

void Serotonin::applyToPlasticity(float currentLevel, float& ltpWeight, float& ltdWeight) {
    if (pImpl->level > 0.5f) {
        ltdWeight *= 1.2f;
        ltpWeight *= 0.9f;
    }
}

void Serotonin::improveMood() {
    pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
}

void Serotonin::decreaseImpulsivity() {
    pImpl->impulsivityReduction = 0.7f;
}

void Serotonin::modulateSocialBehavior(float sociality) {
    pImpl->socialityBoost = 1.0f + sociality;
}

void Serotonin::facilitateSensoryGating() {}

} // namespace nlm
