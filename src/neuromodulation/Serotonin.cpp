#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float mood;
    float socialEngagement;
    float impulsivity;
    float rewardSensitivity;
    float decayRate;
    float tonicLevel;
    float phasicResponse;
    std::vector<float> socialHistory;
    
    Impl() : level(0.0f), mood(0.0f), socialEngagement(0.0f), impulsivity(0.0f),
             rewardSensitivity(0.0f), decayRate(0.06f), tonicLevel(0.15f), phasicResponse(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    NLM_LOG_INFO("Serotonin system initialized");
}

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
    // Serotonin modulates mood and social behavior
    // High serotonin promotes stable, social behaviors
    // Low serotonin increases impulsivity and risk-taking
    if (pImpl->level > 0.5f) {
        // High serotonin: stable, reflective learning
        return 0.7f + 0.3f * pImpl->level;
    } else {
        // Low serotonin: impulsive, exploratory learning
        return 1.2f - 0.7f * pImpl->level;
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards tonic level
    if (pImpl->level > pImpl->tonicLevel) {
        pImpl->level = std::max(pImpl->tonicLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        // Recovery towards tonic level
        pImpl->level = std::min(pImpl->tonicLevel, pImpl->level + pImpl->decayRate * static_cast<float>(dt) * 0.7f);
    }
    
    // Calculate mood effects
    pImpl->mood = pImpl->level * 0.8f;
    pImpl->socialEngagement = pImpl->level * 0.6f;
    
    // Calculate impulsivity (inversely related to serotonin)
    pImpl->impulsivity = (1.0f - pImpl->level) * 0.7f + pImpl->phasicResponse * 0.3f;
    
    if (pImpl->level > 0.0f) {
        NLM_LOG_DEBUG("Serotonin: mood=" + std::to_string(pImpl->mood) + 
                     ", social=" + std::to_string(pImpl->socialEngagement) + 
                     ", impulsivity=" + std::to_string(pImpl->impulsivity) + 
                     ", phasic=" + std::to_string(pImpl->phasicResponse));
    }
}

void Serotonin::signalPositiveSocialExperience(float reward, float socialCohesion) {
    // Social reward increases serotonin
    float response = reward * socialCohesion * 0.6f;
    pImpl->level = std::min(1.0f, pImpl->level + response);
    pImpl->phasicResponse = response;
    pImpl->socialHistory.push_back(pImpl->level);
    if (pImpl->socialHistory.size() > 20) {
        pImpl->socialHistory.erase(pImpl->socialHistory.begin());
    }
    
    NLM_LOG_DEBUG("Serotonin: positive social experience - reward=" + std::to_string(reward) + 
                 ", cohesion=" + std::to_string(socialCohesion) + 
                 ", level=" + std::to_string(pImpl->level));
}

void Serotonin::signalSocialConflict(float isolation, float punishment) {
    // Negative social experiences decrease serotonin
    float response = -isolation * punishment * 0.5f;
    pImpl->level = std::max(0.0f, pImpl->level + response);
    pImpl->phasicResponse = std::max(pImpl->phasicResponse, -response);
    
    NLM_LOG_DEBUG("Serotonin: social conflict - isolation=" + std::to_string(isolation) + 
                 ", punishment=" + std::to_string(punishment) + 
                 ", level=" + std::to_string(pImpl->level));
}

float Serotonin::getMoodLevel() const {
    return pImpl->mood;
}

float Serotonin::getSocialEngagementLevel() const {
    return pImpl->socialEngagement;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivity;
}

const std::vector<float>& Serotonin::getSocialHistory() const {
    return pImpl->socialHistory;
}

float Serotonin::getTotalSignal() const {
    return pImpl->level + pImpl->mood * 0.3f + pImpl->socialEngagement * 0.3f;
}

void Serotonin::reset() {
    pImpl->level = pImpl->tonicLevel;
    pImpl->mood = pImpl->level * 0.8f;
    pImpl->socialEngagement = pImpl->level * 0.6f;
    pImpl->impulsivity = (1.0f - pImpl->level) * 0.7f;
    pImpl->phasicResponse = 0.0f;
    pImpl->socialHistory.clear();
}

} // namespace nlm