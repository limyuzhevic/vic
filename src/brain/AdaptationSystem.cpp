#include "AdaptationSystem.hpp"

namespace nlm {

AdaptationSystem::AdaptationSystem() = default;

void AdaptationSystem::updateAdaptationVariable(float& adaptationVariable, float decayRate) const {
    if (adaptationVariable > 0.0f) {
        adaptationVariable *= (1.0f - decayRate);
        if (adaptationVariable < 0.001f) {
            adaptationVariable = 0.0f;
        }
    }
}

void AdaptationSystem::applyAdaptationToPotential(MembranePotential& V, float adaptationVariable, float strength) const {
    if (adaptationVariable > 0.0f) {
        V -= adaptationVariable * strength;
    }
}

void AdaptationSystem::resetAdaptationVariable(float& adaptationVariable) const {
    adaptationVariable = 0.0f;
}

void AdaptationSystem::incrementAdaptation(float& adaptationVariable, float increment) const {
    adaptationVariable += increment;
}

} // namespace nlm