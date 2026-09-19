#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float riseRate;
    float motivationLevel;
    float rewardLearningFactor;
    float moodLevel;
    float rewardSignals[2]; // reward, prediction error
    std::vector<float> decisionPatternHistory;
    std::vector<float> moodHistory;
    std::mt19937 rng;
    
    Impl() : brain(nullptr), level(0.01f), baseline(0.001f), peak(1.0f), decayRate(0.02f), 
              riseRate(2.5f), motivationLevel(0.0f), rewardLearningFactor(1.0f), moodLevel(0.5f) {
        std::random_device rd;
        rng.seed(rd());
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized for mood regulation and motivation");
}

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
    // 5-HT modulates motivation and decision making
    float basePlasticity = 0.5f;
    float modulation = pImpl->level * 0.5f; // Up to 1.5x baseline under high 5-HT
    return basePlasticity + modulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update motivation level (slower dynamics)
    pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + (pImpl->level - pImpl->motivationLevel) * 0.15f);
    
    // Update reward learning factor
    pImpl->rewardLearningFactor = 1.0f + pImpl->level * 0.6f;
    
    // Update mood level (affects motivation)
    pImpl->moodLevel = 0.5f + pImpl->level * 0.5f; // Mood goes from 0.5 to 1.0 with 5-HT
    
    // Decay history
    if (!pImpl->moodHistory.empty()) {
        pImpl->moodHistory.erase(pImpl->moodHistory.begin());
    }
    pImpl->moodHistory.push_back(pImpl->moodLevel);
}

void Serotonin::signalRewardEvent(float reward, float predictionError) {
    // 5-HT responds to reward and punishment
    float rewardMagnitude = std::max(reward, -predictionError); // Positive for reward, negative for punishment
    
    // Bursty release in response to reward/punishment
    float burstMagnitude = rewardMagnitude * pImpl->riseRate * static_cast<float>(0.12);
    if (rewardMagnitude > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    } else {
        // For punishment, might decrease 5-HT or cause different dynamics
        pImpl->level = std::max(pImpl->baseline, pImpl->level + burstMagnitude * 0.5f);
    }
    
    // Store reward signals
    pImpl->rewardSignals[0] = reward;
    pImpl->rewardSignals[1] = predictionError;
    
    NLM_LOG_DEBUG("Serotonin signaling reward event: reward=%.3f, PE=%.3f", reward, predictionError);
}

void Serotonin::modulateMotivation(class Action* action, float currentLevel) {
    if (action == nullptr) return;
    
    // 5-HT modulates motivation and decision making
    float motivationMod = pImpl->level * 0.4f * currentLevel;
    
    // Can affect action selection through motivation modulation
    if (motivationMod > 0.0f) {
        // Higher 5-HT increases tendency toward approach behaviors
        if (action->getType() == ActionType::MoveForward || 
            action->getType() == ActionType::Interact ||
            action->getType() == ActionType::Eat ||
            action->getType() == ActionType::Drink) {
            // Enhance approach motivation
            std::vector<float>& params = action->getParameters();
            if (params.empty()) {
                params.push_back(motivationMod);
            } else {
                params[0] = std::min(1.0f, params[0] + motivationMod * 0.5f);
            }
        }
    } else if (motivationMod < 0.0f) {
        // Low 5-HT can increase avoidance behaviors
        if (action->getType() == ActionType::MoveBackward || 
            action->getType() == ActionType::TurnLeft ||
            action->getType() == ActionType::TurnRight ||
            action->getType() == ActionType::Rest) {
            // Enhance avoidance motivation
            std::vector<float>& params = action->getParameters();
            if (params.empty()) {
                params.push_back(-motivationMod);
            } else {
                params[0] = std::max(0.0f, params[0] - motivationMod * 0.5f);
            }
        }
    }
    
    NLM_LOG_DEBUG("Serotonin modulated action motivation by: %.3f", motivationMod);
}

void Serotonin::enhanceLearningFromReward(const std::vector<float>& pattern, float reward) {
    if (pattern.empty()) return;
    
    // 5-HT enhances reward-based learning
    float learningBoost = pImpl->level * std::max(reward, 0.0f) * pImpl->rewardLearningFactor;
    
    // Store pattern with 5-HT-enhanced reward weighting
    for (size_t i = 0; i < std::min(pattern.size(), static_cast<size_t>(1000)); ++i) {
        float enhancedValue = pattern[i] * (1.0f + learningBoost);
        pImpl->decisionPatternHistory.push_back(enhancedValue);
    }
    
    NLM_LOG_DEBUG("Serotonin enhanced reward-based learning: boost=%.3f", learningBoost);
}

void Serotonin::affectMood(float moodLevel, float emotionalState) {
    // 5-HT directly affects mood and emotional responses
    pImpl->moodLevel = std::clamp(moodLevel + emotionalState * 0.3f * pImpl->level, 0.0f, 1.0f);
    
    // Can affect motivation level based on mood
    if (pImpl->moodLevel > 0.7f) {
        // Positive mood increases motivation
        pImpl->motivationLevel = std::min(1.0f, pImpl->motivationLevel + 0.2f);
    } else if (pImpl->moodLevel < 0.3f) {
        // Negative mood decreases motivation
        pImpl->motivationLevel = std::max(0.0f, pImpl->motivationLevel - 0.2f);
    }
    
    // Can boost 5-HT level based on mood/emotional state
    if (emotionalState > 0.6f && moodLevel > 0.6f) {
        // Positive emotional state can increase 5-HT
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f);
    }
    
    NLM_LOG_DEBUG("Serotonin affected mood: mood=%.3f, emotional_state=%.3f", moodLevel, emotionalState);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->motivationLevel = 0.0f;
    pImpl->rewardLearningFactor = 1.0f;
    pImpl->moodLevel = 0.5f;
    pImpl->rewardSignals[0] = 0.0f;
    pImpl->rewardSignals[1] = 0.0f;
    pImpl->decisionPatternHistory.clear();
    pImpl->moodHistory.clear();
}

float Serotonin::getMotivationLevel() const {
    return pImpl->motivationLevel;
}

float Serotonin::getRewardLearningFactor() const {
    return pImpl->rewardLearningFactor;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

} // namespace nlm