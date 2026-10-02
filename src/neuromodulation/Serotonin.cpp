#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float socialBehavior;
    float anxiety;
    
    Impl() 
        : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.12f), releaseRate(1.0f)
        , mood(0.5f), impulsivity(0.0f), socialBehavior(0.5f), anxiety(0.0f) {}
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
    // Serotonin modulates plasticity for mood and social behavior
    // Higher serotonin generally reduces plasticity for emotional stability
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Inverse relationship: more serotonin = less emotional plasticity
    float plasticityFactor = 1.2f - 0.4f * normalizedLevel;
    
    // Modulate with anxiety level
    plasticityFactor *= (1.0f - pImpl->anxiety * 0.2f);
    
    // Modulate with social behavior
    if (pImpl->socialBehavior > 0.7f) {
        plasticityFactor *= 1.1f;  // Enhanced plasticity in social contexts
    }
    
    return std::clamp(plasticityFactor, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

void Serotonin::signalMood(float mood) {
    // Real serotonin response to mood states
    // Phasic changes for emotional processing
    
    // Determine response based on mood direction
    float moodResponse = mood * pImpl->releaseRate;
    
    // Modulate with current mood state
    if (mood > 0.5f) {
        // Positive mood: increase serotonin with dampening to prevent runaway excitation
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodResponse * 0.7f);
    } else {
        // Negative mood: increase serotonin more strongly for regulatory effect
        pImpl->level = std::min(pImpl->peak, pImpl->level + moodResponse * 1.2f);
    }
    
    // Update associated state variables
    pImpl->mood = mood;
    
    // Anxiety modulation based on mood
    pImpl->anxiety = std::max(0.0f, 0.5f - mood);
    
    // Social behavior modulation
    if (mood > 0.7f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.1f);
    } else if (mood < 0.3f) {
        pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - 0.05f);
    }
}

} // namespace nlm