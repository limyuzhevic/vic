#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float mood;
    float sociality;
    
    Impl() : level(0.2f), baseline(0.1f), peak(1.0f), decayRate(0.04f), releaseRate(0.8f),
             modulationFactor(0.7f), mood(0.5f), sociality(0.5f) {}
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
    // Serotonin regulates mood and social behavior
    // Affects risk-taking, impulsivity, and learning
    float mood = pImpl->level;
    
    // Good mood improves learning and reduces avoidance
    float plasticity = 0.5f + 0.5f * mood;
    
    // High sociality enhances plasticity for social learning
    plasticity *= (1.0f + pImpl->sociality * 0.3f);
    
    return plasticity * pImpl->modulationFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add noise for emotional variability
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.08f, 0.08f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
    
    // Boost serotonin for positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.3f);
}

void Serotonin::signalSocialBehavior(float sociality) {
    pImpl->sociality = std::clamp(sociality, 0.0f, 1.0f);
    
    // Social interactions boost serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + sociality * 0.2f);
}

} // namespace nlm