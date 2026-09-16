#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float plasticityFactor;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , plasticityFactor(1.0f)
        , arousalLevel(0.0f)
        , vigilanceLevel(0.0f)
        , stressLevel(0.0f) {
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine (NE)";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE modulates arousal, vigilance, and stress response
    pImpl->arousalLevel = pImpl->level * 2.0f;  // Up to 2x arousal
    pImpl->vigilanceLevel = pImpl->level * 1.5f;  // Up to 1.5x vigilance
    pImpl->stressLevel = pImpl->level * 1.0f;  // Up to 1x stress
    
    // Plasticity modulation (NE enhances LTP during stress, LTD during calm)
    if (pImpl->stressLevel > 0.5f) {
        pImpl->plasticityFactor = 1.0f + pImpl->level * 0.4f;  // Stress enhances learning
    } else {
        pImpl->plasticityFactor = 1.0f + pImpl->level * 0.2f;  // Calm state stabilizes learning
    }
    
    // Natural decay
    pImpl->level *= (1.0f - dt * 0.015f);
    
    NLM_LOG_INFO(std::string("Norepinephrine level: ") + std::to_string(pImpl->level) +
                 std::string(" | Arousal: ") + std::to_string(pImpl->arousalLevel) +
                 std::string(" | Vigilance: ") + std::to_string(pImpl->vigilanceLevel));
}

} // namespace nlm
