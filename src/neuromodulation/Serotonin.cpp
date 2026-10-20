// Serotonin implementation
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float moodLevel;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f), moodLevel(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update mood level based on 5-HT level
    pImpl->moodLevel = pImpl->level * 1.2f;  // Range: 0.0 to 1.2
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.2f;  // Range: 1.0 to 1.2
}

float Serotonin::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay 5-HT level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
}

void Serotonin::modulateMood(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Modulate mood and emotional state
    float moodEffect = pImpl->moodLevel * 0.3f;
    
    // Stabilize firing rates and reduce stress responses
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Apply mood modulation
                neuron->injectCurrent(moodEffect * 0.2f);
                
                // Enhance certain plasticity types
                auto& flags = neuron->getPlasticityFlags();
                flags.hebbian = true;  // 5-HT promotes Hebbian learning
            }
        }
    }
}

void Serotonin::regulateImpulsivity(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Reduce impulsive responses and increase self-control
    float impulseControl = pImpl->level * 0.7f;
    
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Apply impulse control modulation
                float controlEffect = impulseControl * 0.4f;
                neuron->injectCurrent(controlEffect);
                
                // Stabilize firing patterns
                float currentPot = neuron->getMembranePotential();
                if (currentPot > -50.0f) {
                    neuron->setMembranePotential(currentPot * 0.95f);
                }
            }
        }
    }
}

} // namespace nlm