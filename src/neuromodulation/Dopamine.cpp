#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    Brain* brain;
    float level;
    float plasticityFactor;
    float predictionError;
    SimulationStep lastRewardStep;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), predictionError(0.0f), lastRewardStep(0) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay dopamine level over time
    pImpl->level *= 0.99f;
    
    // Update plasticity factor based on current level
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::max(0.0f, std::min(1.0f, level));
    pImpl->predictionError = pImpl->level;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->plasticityFactor = 1.0f;
}

} // namespace nlm