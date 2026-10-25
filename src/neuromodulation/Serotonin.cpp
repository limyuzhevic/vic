#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float moodWeight;
    float impulsivityWeight;
    float socialWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , moodWeight(0.5f)
        , impulsivityWeight(0.3f)
        , socialWeight(0.4f)
        , decayRate(0.04f) {}
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
    // Serotonin modulates plasticity for mood-related processes
    return 0.6f + 0.4f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin affects mood, motivation, and social behavior
    // In a real implementation, this would affect reward processing
    // and behavioral flexibility
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalPositiveMood(float mood) {
    // Serotonin responds to positive experiences
    pImpl->level = std::min(1.0f, pImpl->level + mood * 0.4f);
}

void Serotonin::reduceImpulsivity(float calm) {
    // Serotonin reduces impulsive behaviors
    pImpl->level = std::max(0.0f, pImpl->level - calm * 0.3f);
}

void Serotonin::enhanceSocialBehavior(float social) {
    // Serotonin supports social bonding and cooperation
    pImpl->level = std::min(1.0f, pImpl->level + social * 0.3f);
}

} // namespace nlm