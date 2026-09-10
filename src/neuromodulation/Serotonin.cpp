#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Mood and behavioral components
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModulation;
    
    // Internal dynamics
    float moodChange;
    float satiationLevel;
    float socialContext;
    
    // Neurotransmitter cycling
    float synthesisRate;
    float reuptakeRate;
    float metabolismRate;
    
    // History tracking
    float moodHistory[10]; // Last 10 mood levels
    int moodHistoryIndex;
    
    Impl() : level(0.5f), baseline(0.3f), peak(2.0f), decayRate(0.03f),
             moodLevel(0.5f), impulsivityLevel(0.5f), socialBehaviorModulation(0.5f),
             moodChange(0.0f), satiationLevel(0.5f), socialContext(0.5f),
             synthesisRate(0.05f), reuptakeRate(0.02f), metabolismRate(0.01f),
             moodHistoryIndex(0) {
        for (int i = 0; i < 10; ++i) moodHistory[i] = 0.5f;
    }
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity based on mood and impulsivity
    // High serotonin (good mood) increases stable plasticity
    // Low serotonin (low mood) increases exploratory plasticity
    float moodEffect = pImpl->moodLevel * 0.6f; // Good mood promotes stable learning
    float impulsivityPenalty = pImpl->impulsivityLevel * 0.4f; // High impulsivity reduces stable learning
    float socialEffect = pImpl->socialBehaviorModulation * 0.2f; // Social context modulates plasticity
    
    // Serotonin itself provides baseline stability
    float levelEffect = pImpl->level * 0.3f;
    
    return std::max(0.2f, std::min(2.0f, 0.3f + moodEffect - impulsivityPenalty + socialEffect + levelEffect));
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    float dtFloat = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Update mood components
    pImpl->moodLevel = std::max(0.1f, pImpl->moodLevel - pImpl->decayRate * dtFloat * 0.5f);
    pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel + pImpl->decayRate * dtFloat * 0.2f);
    pImpl->socialBehaviorModulation = std::max(0.2f, pImpl->socialBehaviorModulation - pImpl->decayRate * dtFloat * 0.3f);
    
    // Update internal states
    pImpl->moodChange = pImpl->moodChange * 0.95f - pImpl->decayRate * dtFloat * 0.1f;
    pImpl->satiationLevel = std::max(0.2f, pImpl->satiationLevel - pImpl->decayRate * dtFloat * 0.2f);
    pImpl->socialContext = std::max(0.2f, pImpl->socialContext - pImpl->decayRate * dtFloat * 0.3f);
    
    // Track mood history
    pImpl->moodHistory[pImpl->moodHistoryIndex] = pImpl->moodLevel;
    pImpl->moodHistoryIndex = (pImpl->moodHistoryIndex + 1) % 10;
}

void Serotonin::signalMoodChange(float mood) {
    // Mood input (positive or negative emotional states)
    pImpl->moodChange = mood;
    pImpl->moodLevel = std::clamp(pImpl->moodLevel + mood * 0.3f, 0.1f, 1.0f);
    
    // Good mood increases serotonin
    if (mood > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.4f);
    }
    
    // Bad mood decreases serotonin
    else if (mood < 0) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + mood * 0.2f);
        pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel - mood * 0.1f);
    }
    
    NLM_LOG_INFO("Serotonin: Mood changed by " + std::to_string(mood) + " (level: " + std::to_string(pImpl->level) + ")");
}

void Serotonin::signalSatiation(float level) {
    // Satiation signal (food, water, reward - reduces drive)
    pImpl->satiationLevel = std::min(1.0f, pImpl->satiationLevel + level * 0.3f);
    
    // Satiation reduces impulsivity and stabilizes mood
    pImpl->impulsivityLevel = std::max(0.2f, pImpl->impulsivityLevel - level * 0.4f);
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + level * 0.2f);
    
    // Moderate increase in serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + level * 0.2f);
    
    NLM_LOG_INFO("Serotonin: Satiation level increased to " + std::to_string(pImpl->satiationLevel));
}

void Serotonin::signalSocialContext(float context) {
    // Social context (social interaction, observation, norms)
    pImpl->socialContext = context;
    pImpl->socialBehaviorModulation = std::min(1.0f, pImpl->socialBehaviorModulation + context * 0.3f);
    
    // Good social context improves mood
    if (context > 0) {
        pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + context * 0.2f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + context * 0.3f);
    }
    
    // Poor social context reduces serotonin
    else {
        pImpl->moodLevel = std::max(0.2f, pImpl->moodLevel + context * 0.1f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level + context * 0.2f);
    }
    
    NLM_LOG_INFO("Serotonin: Social context modulated with value " + std::to_string(context));
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialBehaviorModulation() const {
    return pImpl->socialBehaviorModulation;
}

} // namespace nlm