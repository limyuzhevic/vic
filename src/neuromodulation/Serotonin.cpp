#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Effects on impulsivity
    float impulsivityLevel;
    float impulsivityThreshold;
    
    // Effects on social behavior
    float socialMotivation;
    float socialThreshold;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f),
             impulsivityLevel(1.0f), impulsivityThreshold(0.5f),
             socialMotivation(1.0f), socialThreshold(0.5f) {}
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
    // TODO PHASE 2: Implement real 5-HT-modulated plasticity factor
    // Serotonin typically reduces impulsivity and stabilizes learning
    return 0.7f - 0.2f * pImpl->level;  // High 5-HT = less impulsivity, stable learning
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocial(float social) {
    // TODO PHASE 2: Implement social signaling
    // 5-HT increases with social experience
    pImpl->level = std::min(pImpl->peak, pImpl->level + social * pImpl->releaseRate);
}

void Serotonin::signalMood(float mood) {
    // TODO PHASE 2: Implement mood signaling
    // 5-HT increases with positive mood and well-being
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

} // namespace nlm