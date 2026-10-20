#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // 5-HT effects on neural systems
    float mood;
    float impulsivity;
    float socialBehavior;
    float anxiety;
    float sleepNeed;
    
    // For mood regulation and social behavior
    std::vector<float> moodTrace;
    std::vector<float> socialTrace;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.2f)
        , releaseRate(1.0f)
        , mood(0.0f)
        , impulsivity(0.0f)
        , socialBehavior(0.0f)
        , anxiety(0.0f)
        , sleepNeed(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);  // 5-HT can go above 1
    
    // Update downstream effects
    pImpl->mood = 0.5f * pImpl->level;  // Positive mood effects at moderate levels
    pImpl->impulsivity = pImpl->level * 0.7f;  // Increases impulsivity at high levels
    pImpl->socialBehavior = pImpl->level * 0.8f;  // Enhances social behavior
    pImpl->anxiety = std::max(0.0f, pImpl->level - 1.0f);  // Anxiety at high levels
    pImpl->sleepNeed = pImpl->level * 0.3f;  // Promotes sleep
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin has complex effects: promotes LTP at moderate levels, 
    // inhibits at very high levels (adaptive response to stress)
    if (pImpl->level < 0.8f) {
        // Moderate serotonin enhances learning and memory consolidation
        return 1.0f + pImpl->level * 0.6f;
    } else {
        // High serotonin can inhibit plasticity (stress response)
        return std::max(0.5f, 1.4f - (pImpl->level - 0.8f) * 0.3f);
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay of serotonin signals
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update downstream effects
    setLevel(pImpl->level);
}

void Serotonin::signalSocialExperience(float socialValue, float rewardMagnitude) {
    // Serotonin responds to social experiences and rewards
    float socialBurst = socialValue * (1.0f + rewardMagnitude * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialBurst * pImpl->releaseRate);
    
    // Store social trace
    pImpl->socialTrace.push_back(pImpl->level);
    if (pImpl->socialTrace.size() > 100) {
        pImpl->socialTrace.erase(pImpl->socialTrace.begin());
    }
    
    NLM_LOG_INFO("5-HT social response: value=" + std::to_string(socialValue) + 
                " reward=" + std::to_string(rewardMagnitude) + 
                " level=" + std::to_string(pImpl->level));
}

void Serotonin::signalInhibition(float punishment) {
    // Serotonin responds to punishment and signals need for behavioral inhibition
    float inhibitionBurst = punishment * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibitionBurst * pImpl->releaseRate);
    
    // Store mood trace
    pImpl->moodTrace.push_back(pImpl->level);
    if (pImpl->moodTrace.size() > 100) {
        pImpl->moodTrace.erase(pImpl->moodTrace.begin());
    }
    
    NLM_LOG_INFO("5-HT inhibition: punishment=" + std::to_string(punishment) + 
                " level=" + std::to_string(pImpl->level));
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getAnxiety() const {
    return pImpl->anxiety;
}

float Serotonin::getSleepNeed() const {
    return pImpl->sleepNeed;
}

} // namespace nlm