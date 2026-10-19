#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <string>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood and social behavior state
    float moodLevel;
    float impulsivityLevel;
    float rewardSensitivity;
    float socialMotivation;
    float anxietyLevel;
    
    //phasic vs tonic dynamics
    float tonicLevel;
    float phasicRelease;
    
    // Social learning and memory
    std::unordered_map<std::string, float> socialRewardWeights;
    std::unordered_map<std::string, float> socialPunishmentWeights;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f)
        , moodLevel(0.0f), impulsivityLevel(0.0f), rewardSensitivity(1.0f)
        , socialMotivation(0.0f), anxietyLevel(0.0f), tonicLevel(0.0f), phasicRelease(0.0f) {
        std::random_device rd;
        rng.seed(rd());
        randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        
        // Initialize social reward weights
        socialRewardWeights["cooperation"] = 1.0f;
        socialRewardWeights["helping"] = 0.8f;
        socialRewardWeights["sharing"] = 0.7f;
        socialRewardWeights["competition"] = 0.5f;
        
        socialPunishmentWeights["hostility"] = 1.0f;
        socialPunishmentWeights["cheating"] = 0.9f;
        socialPunishmentWeights["aggression"] = 0.8f;
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
    float oldLevel = pImpl->level;
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update mood and social behavior based on serotonin level
    // High serotonin typically associated with better mood and social behavior
    pImpl->tonicLevel = oldLevel + (pImpl->level - oldLevel) * 0.1f; // Slower tonic changes
    pImpl->phasicRelease = std::max(0.0f, pImpl->level - oldLevel); // Phasic bursts
    
    // Serotonin effects on behavior
    pImpl->moodLevel = 0.5f + 0.5f * pImpl->level;
    pImpl->impulsivityLevel = 1.0f - 0.7f * pImpl->level; // Lower impulsivity with higher serotonin
    pImpl->rewardSensitivity = 1.0f + 0.3f * pImpl->level;
    pImpl->socialMotivation = 0.3f + 0.5f * pImpl->level;
    pImpl->anxietyLevel = std::max(0.0f, 1.0f - pImpl->level); // Lower anxiety with higher serotonin
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects behavioral flexibility and learning:
    // Moderate serotonin promotes exploration, very high promotes avoidance
    float normalizedLevel = pImpl->level;
    return 0.7f + 0.3f * (1.0f - std::abs(normalizedLevel - 0.5f));
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline but with stochastic bursts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Occasional phasic bursts (social reward signaling)
    float burstChance = 0.001f * static_cast<float>(dt);
    if (pImpl->randDist(pImpl->rng) < burstChance && pImpl->level < 0.8f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.4f * pImpl->releaseRate);
    }
    
    // Update social reward weights based on current serotonin level
    // This simulates serotonin modulation of social decision making
    for (auto& pair : pImpl->socialRewardWeights) {
        pair.second = 1.0f - 0.5f * pImpl->level; // Higher serotonin = stronger social reward weights
    }
}

} // namespace nlm