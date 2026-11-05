#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;        // Current mood state
    float impulseControl;   // Behavioral inhibition strength
    float socialBonding;    // Social behavior facilitation
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f),
          decayRate(0.08f), releaseRate(0.8f),
          moodLevel(1.0f), impulseControl(1.0f), socialBonding(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin regulates mood and behavioral control
    // Higher 5-HT increases plasticity for mood-related and inhibitory processes
    return 0.3f + 0.7f * pImpl->level * pImpl->impulseControl;
}