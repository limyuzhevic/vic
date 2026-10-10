#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionLevel;
    float focusDepth;
    float modulationStrength;
    
    Impl() 
        : level(0.0f), attentionLevel(0.5f), focusDepth(0.0f), modulationStrength(1.0f) {}
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
    return 1.0f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level = std::max(0.0f, pImpl->level - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::setAttentionLevel(float level) {
    pImpl->attentionLevel = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::enhanceFocus(float duration) {
    pImpl->focusDepth = std::min(1.0f, pImpl->focusDepth + duration * 0.1f);
    pImpl->modulationStrength = 1.0f + pImpl->focusDepth * 0.5f;
}

float Acetylcholine::getModulationStrength() const {
    return pImpl->modulationStrength;
}

} // namespace nlm