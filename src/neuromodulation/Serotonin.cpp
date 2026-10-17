#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float patience;
    float socialAttraction;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             moodLevel(0.5f), patience(1.0f), socialAttraction(1.0f) {}
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
    // Serotonin modulates mood and affects learning and memory
    // Higher serotonin generally improves mood and can enhance certain types of learning
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate mood and behavioral state based on level
    // Serotonin has complex effects on decision making and social behavior
}

void Serotonin::elevateMood(float level) {
    // Increase mood state
    pImpl->moodLevel = std::clamp(pImpl->moodLevel + level, 0.5f, 1.5f);
    
    // This effect would be implemented in Brain integration
    // Better mood can improve learning and motivation
}

void Serotonin::reduceImpulsivity(float strength) {
    // Reduce impulsive behavior
    pImpl->patience = std::clamp(pImpl->patience + strength, 0.5f, 3.0f);
    
    // This effect would be implemented in Brain integration
    // Higher patience leads to better long-term planning
}

void Serotonin::promoteSocialBehavior(float strength) {
    // Promote social and cooperative behavior
    pImpl->socialAttraction = std::clamp(pImpl->socialAttraction + strength, 0.5f, 2.0f);
    
    // This effect would be implemented in Brain integration
    // Enhanced social motivation can improve learning from social interactions
}

void Serotonin::enhancePatience(float strength) {
    // Increase patience and delay of gratification
    pImpl->patience = std::clamp(pImpl->patience + strength, 0.5f, 3.0f);
    
    // This effect would be implemented in Brain integration
    // Better patience allows for more sophisticated planning and self-control
}

void Serotonin::reduceAggression(float strength) {
    // Reduce aggressive responses
    pImpl->moodLevel = std::min(pImpl->moodLevel + strength * 0.5f, 1.5f);
    
    // This effect would be implemented in Brain integration
    // Reduced aggression can improve social learning and cooperation
}

void Serotonin::promoteCooperation(float strength) {
    // Promote cooperative and helpful behavior
    pImpl->socialAttraction = std::clamp(pImpl->socialAttraction + strength * 2.0f, 0.5f, 2.0f);
    
    // This effect would be implemented in Brain integration
    // Enhanced cooperation supports social learning and knowledge sharing
}

void Serotonin::improveDecisionMaking(float confidence) {
    // Enhance decision making quality
    // Higher serotonin can improve cognitive flexibility and decision quality
    // This effect would be implemented in Brain integration
}

} // namespace nlm