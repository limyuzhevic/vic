#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodLevel;
    float impulsivity;
    float socialBonding;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f),
             moodLevel(0.5f), impulsivity(0.5f), socialBonding(0.5f) {
        history.clear();
    }
    
    std::vector<float> history;
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

const char* Serotonin::getEffectType() const {
    return "Mood";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    updateMoodState();
}

void Serotonin::signalPositiveMood(float mood) {
    if (mood > 0.0f) {
        float boost = mood * 0.5f;
        pImpl->level += boost;
        
        if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
        pImpl->history.push_back(pImpl->level);
    }
}

void Serotonin::signalSocialBonding(float bonding) {
    float bondingBoost = bonding * 0.4f;
    pImpl->level += bondingBoost;
    
    if (pImpl->level > pImpl->peak) pImpl->level = pImpl->peak;
    pImpl->history.push_back(pImpl->level);
    
    pImpl->socialBonding = std::min(pImpl->socialBonding, 1.0f + bonding * 0.3f);
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityFactor() const {
    return 1.0f - 0.8f * (pImpl->level / pImpl->peak);
}

float Serotonin::getSocialBondingFactor() const {
    return pImpl->socialBonding;
}

float Serotonin::getPlasticityFactor() const {
    float normalized = pImpl->level / pImpl->peak;
    
    if (normalized < 0.3f) {
        return 0.6f + 0.3f * normalized;
    } else {
        return 0.9f + 0.1f * std::exp(-2.0f * (normalized - 0.7f));
    }
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateMoodState();
    
    if (pImpl->socialBonding > 1.0f) {
        pImpl->socialBonding = std::max(1.0f, pImpl->socialBonding - 0.02f * static_cast<float>(dt));
    }
}

void Serotonin::updateMoodState() {
    float normalized = pImpl->level / pImpl->peak;
    
    if (normalized < 0.2f) {
        pImpl->moodLevel = 0.2f + 0.6f * normalized;
    } else if (normalized < 0.8f) {
        pImpl->moodLevel = 0.5f + 0.5f * normalized;
    } else {
        pImpl->moodLevel = 0.9f + 0.1f * (normalized - 0.8f);
    }
}

void Serotonin::signalNegativeExperience(float severity) {
    float reduction = severity * 0.3f;
    pImpl->level = std::max(0.0f, pImpl->level - reduction);
    pImpl->history.push_back(pImpl->level);
}

float Serotonin::getResiliencyFactor() const {
    return 0.5f + 0.5f * pImpl->baseline;
}

void Serotonin::initialize(Brain* brain) {
    pImpl->history.clear();
}

const std::vector<float>& Serotonin::getHistory() const {
    return pImpl->history;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm