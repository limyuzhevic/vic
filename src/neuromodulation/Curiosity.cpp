#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    Brain* brain;
    float level;
    float decayRate;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.01f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {
    NLM_LOG_INFO("Curiosity system initialized");
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    reset();
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    // Natural decay over time
    pImpl->level *= (1.0f - pImpl->decayRate * dt);
}

void Curiosity::increaseCuriosity(float amount) {
    pImpl->level += amount;
    if (pImpl->level > 1.0f) pImpl->level = 1.0f;
}

void Curiosity::decreaseCuriosity(float amount) {
    pImpl->level -= amount;
    if (pImpl->level < 0.0f) pImpl->level = 0.0f;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
