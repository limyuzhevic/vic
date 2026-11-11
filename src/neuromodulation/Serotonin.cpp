#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float adaptationRate;
    float moodFactor;
    float impulsivity;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), adaptationRate(0.1f), moodFactor(0.6f), impulsivity(1.0f), socialBehavior(1.0f) {}
};

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

float Serotonin::getPlasticityFactor() const {
    // TODO PHASE 2: Implement 5-HT-modulated plasticity factor
    // PLACEHOLDER: 5-HT promotes stable plasticity and mood regulation
    return 1.0f + pImpl->level * 0.2f;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: 5-HT modulates mood and social behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->adaptationRate * static_cast<float>(dt));
}

void Serotonin::applyToBehavior(float &motorCommandProbability, float &explorationBias) {
    // TODO PHASE 2: Implement behavioral effects
    // PLACEHOLDER: 5-HT reduces impulsivity and increases social focus
    motorCommandProbability *= (1.0f - pImpl->level * 0.3f);
    explorationBias -= pImpl->level * 0.5f;
}

void Serotonin::applyToMemoryConsolidation(float &consolidationProbability, float &replayFrequency) {
    // TODO PHASE 2: Implement memory consolidation effects
    // PLACEHOLDER: 5-HT promotes memory stability
    consolidationProbability += pImpl->level * 0.4f;
    replayFrequency += pImpl->level * 0.3f;
}

} // namespace nlm