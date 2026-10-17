#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float inhibition;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), mood(0.5f), 
             inhibition(0.5f) {}
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
    // Serotonin modulates plasticity - moderate levels improve learning
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Mood and inhibition decay slightly
    pImpl->mood = std::max(0.1f, pImpl->mood - pImpl->decayRate * static_cast<float>(dt));
    pImpl->inhibition = std::max(0.1f, pImpl->inhibition - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::initialize(Brain* brain) {
    // Initialize serotonin system with brain reference
    // Reset to baseline state
    pImpl->baseline = 0.05f;
    pImpl->level = 0.05f;
    pImpl->mood = 0.5f;
    pImpl->inhibition = 0.5f;
}

void Serotonin::signalMoodChange(float moodDelta) {
    // Serotonin responds to mood changes
    pImpl->mood = std::min(1.0f, pImpl->mood + moodDelta);
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodDelta * 0.3f);
}

void Serotonin::signalInhibition(float inhibitionStrength) {
    // Serotonin directly modulates behavioral inhibition
    pImpl->inhibition = std::min(1.0f, pImpl->inhibition + inhibitionStrength);
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionStrength * 0.5f);
}

void Serotonin::signalSocialBehavior(float socialityLevel) {
    // Serotonin affects social approach/avoidance behavior
    pImpl->mood = std::min(1.0f, pImpl->mood + socialityLevel * 0.5f);
    if (socialityLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + socialityLevel * 0.3f);
    }
}

void Serotonin::applyAttentionalModulation(float attentionalBias) {
    // Serotonin modulates selective attention
    if (attentionalBias > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + attentionalBias * 0.2f);
    }
}

void Serotonin::modulateConceptFormation(float learningRate) {
    // Serotonin affects concept formation stability and learning
    if (learningRate > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + learningRate * 0.2f);
    }
}

} // namespace nlm