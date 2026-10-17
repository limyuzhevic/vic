#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Social and mood parameters
    float moodLevel;
    float socialCohesion;
    float impulsivity;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             moodLevel(1.0f), socialCohesion(1.0f), impulsivity(1.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    float moodModulation = 1.0f;
    
    // 5-HT stabilizes neural activity and reduces impulsive actions
    moodModulation *= (1.0f + pImpl->level * 0.3f);
    
    // High serotonin reduces impulsivity and promotes persistence
    return std::clamp(moodModulation, 0.5f, 2.0f);
}

void Serotonin::update(TimestepDuration dt) {
    // Implement serotonergic dynamics
    // 5-HT fluctuates with social interactions and mood states
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add social interaction fluctuations
    float socialMod = std::sin(static_cast<float>(pImpl->rng.generate()) * 1.5f) * 0.06f;
    pImpl->level += socialMod;
}

void Serotonin::signalSocialBonding(float socialBonding) {
    // Signal social bonding and connection with serotonergic responses
    float bondingResponse = socialBonding * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingResponse);
    
    // Apply to social learning systems
    if (pImpl->brain) {
        auto* socialLearning = pImpl->brain->getSocialLearning();
        if (socialLearning) {
            // 5-HT enhances social learning and imitation
            socialLearning->setSocialLearningStrength(socialLearning->getSocialLearningStrength() + pImpl->level * 0.3f);
        }
    }
}

void Serotonin::signalMood(float mood) {
    // Signal mood states with serotonergic modulation
    float moodResponse = mood * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodResponse);
    
    // Enhance behavioral flexibility and persistence
    if (pImpl->brain) {
        auto* socialLearning = pImpl->brain->getSocialLearning();
        if (socialLearning) {
            socialLearning->setPersistenceFactor(socialLearning->getPersistenceFactor() + pImpl->level * 0.2f);
        }
    }
}

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized");
}

float Serotonin::getMoodLevel() const {
    return 1.0f + pImpl->level * pImpl->moodLevel;
}

float Serotonin::getSocialCohesionLevel() const {
    return 1.0f + pImpl->level * pImpl->socialCohesion;
}

float Serotonin::getImpulsivityLevel() const {
    return 1.0f - pImpl->level * (1.0f - pImpl->impulsivity);  // High 5-HT = low impulsivity
}

} // namespace nlm
