#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Neuromodulator::Impl {
    float level;
    float baseline;
    float targetLevel;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , targetLevel(0.0f) {}
};

Neuromodulator::Neuromodulator() : pImpl(std::make_unique<Impl>()) {
    // Default implementation
}

Neuromodulator::~Neuromodulator() = default;

float Neuromodulator::getLevel() const {
    return pImpl->level;
}

void Neuromodulator::setLevel(float level) {
    pImpl->level = level;
    pImpl->targetLevel = pImpl->level;
}

float Neuromodulator::getPlasticityFactor() const {
    return 1.0f;  // Default: no modulation
}

void Neuromodulator::update(TimestepDuration dt) {
    // Default: decay towards baseline
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - 0.1f * static_cast<float>(dt));
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + 0.05f * static_cast<float>(dt));
    }
}

float Neuromodulator::getBaselineLevel() const {
    return pImpl->baseline;
}

void Neuromodulator::setTargetLevel(float target) {
    pImpl->targetLevel = target;
}

bool Neuromodulator::isActive() const {
    return pImpl->level > pImpl->baseline + 0.01f;
}

void Neuromodulator::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
}

} // namespace nlm
