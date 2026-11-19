#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float facilitationRate;
    
    // 5-HT effects on mood and behavioral control
    float moodLevel;
    float impulsivityLevel;
    float socialBehaviorModifier;
    
    // Track serotonergic neurons
    std::unordered_map<uint64_t, float> serotonergicNeurons;
    std::unordered_map<uint64_t, float> moodHistory;
    
    // For emotional regulation
    float emotionalStability;
    float rewardSensitivity;
    
    // Behavioral state: exploration vs exploitation
    float explorationBias;
    float exploitationBias;
    
    Impl() : 
        level(0.0f), 
        baseline(0.0f), 
        peak(1.0f), 
        decayRate(0.12f), 
        releaseRate(1.2f), 
        facilitationRate(0.3f),
        moodLevel(0.5f),
        impulsivityLevel(0.5f),
        socialBehaviorModifier(1.0f),
        emotionalStability(0.5f),
        rewardSensitivity(0.5f),
        explorationBias(0.5f),
        exploitationBias(0.5f) {}
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
    // 5-HT modulates plasticity: low levels increase risky exploration plasticity,
    // high levels suppress plasticity for stability
    if (pImpl->level < 0.4f) {
        return 1.0f + pImpl->level * 0.8f;  // Encourage exploration under low 5-HT
    } else {
        return std::max(0.4f, 1.2f - pImpl->level * 0.8f);  // Stabilize under high 5-HT
    }
}

void Serotonin::update(TimestepDuration dt) {
    // 5-HT dynamics modeling mood regulation
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Facilitation from emotional state
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->facilitationRate * pImpl->moodLevel);
    
    // Update mood and behavioral biases
    pImpl->moodLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->impulsivityLevel = 1.0f - 0.8f * pImpl->level;  // Low 5-HT = high impulsivity
    pImpl->explorationBias = std::max(0.1f, 0.9f - pImpl->level * 0.8f);
    pImpl->exploitationBias = 1.0f - pImpl->explorationBias;
    
    // Emotional stability improves with 5-HT
    pImpl->emotionalStability = 0.3f + 0.7f * pImpl->level;
    pImpl->rewardSensitivity = 0.4f + 0.6f * pImpl->level;
}

void Serotonin::signalReward(float reward) {
    // Positive reward increases 5-HT level for mood regulation
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + reward * 0.3f);
    
    // Reduces impulsivity after reward (better behavioral control)
    pImpl->impulsivityLevel = std::max(0.1f, pImpl->impulsivityLevel - reward * 0.2f);
}

void Serotonin::signalPunishment(float punishment) {
    // Negative events decrease 5-HT level (reflecting low mood)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - punishment * pImpl->releaseRate * 0.8f);
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - punishment * 0.5f);
    
    // Increases impulsivity after punishment (desperation behavior)
    pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel + punishment * 0.3f);
}

void Serotonin::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply 5-HT effects to behavioral control
    float serotoninMod = 1.0f - pImpl->level;  // Low 5-HT = high exploration behavior
    
    // Modify behavioral strategies based on mood
    // Low 5-HT (low mood) encourages exploration for novel solutions
    // High 5-HT (high mood) favors exploitation of known strategies
    
    // This would be used by action selection systems to choose between
    // exploration vs exploitation strategies
    
    // Social behavior modulation
    if (pImpl->level > 0.7f) {
        // High 5-HT promotes social cooperation and conformity
        pImpl->socialBehaviorModifier = 1.2f;
    } else if (pImpl->level < 0.3f) {
        // Low 5-HT promotes individualistic and competitive behavior
        pImpl->socialBehaviorModifier = 0.8f;
    }
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getExplorationBias() const {
    return pImpl->explorationBias;
}

} // namespace nlm