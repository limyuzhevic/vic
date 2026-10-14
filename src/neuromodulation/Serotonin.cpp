#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float resetRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), resetRate(0.01f) {}
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
    // Serotonin modulates plasticity: higher 5-HT generally decreases plasticity
    // High serotonin is associated with learned behavior, decreased exploration
    return std::max(0.1f, 1.0f - 0.5f * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: slow decay, resets after experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalExperience(const std::vector<float>& rewards) {
    // Serotonin responds to experiences, especially unexpected rewards
    float totalExperience = 0.0f;
    for (float reward : rewards) {
        totalExperience += reward;
    }
    pImpl->level = std::min(pImpl->peak, pImpl->level + totalExperience * 0.1f);
}

} // namespace nlm