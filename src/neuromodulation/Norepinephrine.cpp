#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float attentionFocus;
    float memoryConsolidation;
    
    Impl() 
        : level(0.0f), baseline(0.3f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f)
        , arousalLevel(1.0f), attentionFocus(1.0f), memoryConsolidation(1.0f) {}
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
    return 0.8f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = 0.5f + 0.5f * pImpl->level;
    pImpl->attentionFocus = 0.5f + 0.5f * pImpl->level;
    pImpl->memoryConsolidation = 0.7f + 0.6f * pImpl->level;
}

void Norepinephrine::modulateAttention(float arousal, float focus) {
    pImpl->attentionFocus = std::clamp(arousal * focus, 0.0f, 2.0f);
}

void Norepinephrine::modulateMemoryConsolidation(float novelty, float salience) {
    pImpl->memoryConsolidation = std::clamp(novelty * salience, 0.5f, 2.0f);
}

} // namespace nlm
