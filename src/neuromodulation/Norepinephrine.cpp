// Norepinephrine implementation
#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float arousalLevel;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f), arousalLevel(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal level based on NE level
    pImpl->arousalLevel = pImpl->level * 1.5f;  // Range: 0.0 to 1.5
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.3f;  // Range: 1.0 to 1.3
}

float Norepinephrine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay NE level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
}

void Norepinephrine::increaseArousal(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Increase overall arousal and alertness
    float arousalEffect = pImpl->arousalLevel * 0.4f;
    
    // Apply to all neurons - increases responsiveness
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Increase firing threshold sensitivity
                neuron->injectCurrent(arousalEffect * 0.5f);
                
                // Enhance plasticity
                auto& flags = neuron->getPlasticityFlags();
                flags.stdp = true;  // NE strongly promotes STDP
            }
        }
    }
}

void Norepinephrine::enhanceSignalDetection(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Enhance signal detection - increase response to small inputs
    float detectionBoost = pImpl->level * 0.6f;
    
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Boost responses to weak inputs
                float boost = neuron->getMembranePotential() * 0.1f + detectionBoost * 0.3f;
                neuron->injectCurrent(boost);
            }
        }
    }
}

} // namespace nlm