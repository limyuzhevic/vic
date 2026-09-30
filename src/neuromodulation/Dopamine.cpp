// Dopamine.cpp
#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    Brain* brain;
    
    float level;
    float plasticityFactor;
    
    Impl() : brain(nullptr), level(0.0f), plasticityFactor(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Simple dopamine dynamics - level changes based on reward prediction error
    // In a real implementation, this would be more complex
    pImpl->level *= (1.0f - dt * 0.01f);  // Decay over time
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Plasticity factor affects learning rate
    pImpl->plasticityFactor = 1.0f - pImpl->level * 0.3f;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->plasticityFactor = 1.0f;
}

void Dopamine::applyEffects() {
    // Apply dopamine effects to neural excitability and plasticity
    if (pImpl->brain) {
        // Dopamine modulates neural excitability
        // Higher dopamine increases excitability (lower effective threshold)
        // This is implemented in the Brain::step() method
    }
}

} // namespace nlm