#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Serotonin::Impl {
    // Core serotonin dynamics
    float level;
    float baseline;              // Tonic serotonin baseline
    float peak;                  // Maximum achievable level
    
    // Mood regulation
    float moodLevel;              // Current mood level (-1 to +1)
    float targetMood;             // Target mood for stabilization
    float moodStability;          // Mood stability (0 to 1)
    float anhedonia;              // Anhedonia level (reduced reward sensitivity)
    
    // Impulsivity control
    float impulseControl;         // Current impulse control level (0 to 1)
    float behavioralInhibition;  // Behavioral inhibition level
    float reinforcementSensitivity; // Sensitivity to rewards
    
    // Social behavior regulation
    float socialMotivation;       // Motivation for social interaction
    float bondingStrength;        // Strength of social bonds
    float socialReward;           // Social reward processing
    
    // Temporal dynamics
    float moodDecay;              // Mood decay rate
    float impulseControlDecay;    // Impulse control decay rate
    float stabilizationRate;      // Mood stabilization rate
    float homeostaticRate;        // Homeostatic adjustment rate
    
    // State variables
    float moodInhibition;         // Mood inhibition (top-down control)
    float rewardPredictionError;   // Modulated by serotonin
    float learningRateModulation; // Effect on learning rate
    float anxietyLevel;           // Anxiety level
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    // History tracking
    std::vector<float> moodHistory;
    std::vector<float> impulseHistory;
    float maxMoodRecorded;         // Maximum mood recorded
    float minMoodRecorded;         // Minimum mood recorded
    
    // Social interaction tracking
    float lastSocialInteraction;   // Time since last social interaction
    bool socialBondActive;         // Whether social bond is active
    float socialBondStrength;      // Current social bond strength
    
    Impl() : level(0.1f), baseline(0.1f), peak(1.0f)
           , moodLevel(0.0f), targetMood(0.0f), moodStability(0.8f), anhedonia(0.0f)
           , impulseControl(0.5f), behavioralInhibition(0.5f), reinforcementSensitivity(1.0f)
           , socialMotivation(0.5f), bondingStrength(0.5f), socialReward(0.0f)
           , moodDecay(0.1f), impulseControlDecay(0.05f), stabilizationRate(0.02f), homeostaticRate(0.01f)
           , moodInhibition(0.5f), rewardPredictionError(0.0f), learningRateModulation(1.0f)
           , anxietyLevel(0.0f), lastSocialInteraction(0.0f), socialBondActive(false)
           , socialBondStrength(0.5f) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        normalDist = std::normal_distribution<float>(0.0f, 0.1f);
        moodHistory.reserve(20);
        impulseHistory.reserve(20);
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // Update related state variables
    if (pImpl->level < pImpl->baseline) {
        // Low serotonin associated with negative mood
        pImpl->moodLevel -= (pImpl->baseline - pImpl->level) * 0.5f;
    } else if (pImpl->level > pImpl->baseline) {
        // High serotonin associated with positive mood
        pImpl->moodLevel += (pImpl->level - pImpl->baseline) * 0.3f;
    }
}

float Serotonin::getPlasticityFactor() const {
    // Biologically plausible 5-HT-modulated plasticity factor
    // Serotonin modulates plasticity based on mood and impulsivity
    float moodComponent = (pImpl->moodLevel + 1.0f) * 0.5f;  // Convert -1..1 to 0..1
    float impulseComponent = (1.0f - pImpl->impulseControl) * 0.5f;
    
    // Combined effect with anhedonia modulation
    float anhedoniaModulation = 1.0f - pImpl->anhedonia * 0.5f;
    
    float plasticityFactor = (moodComponent * 0.6f + impulseComponent * 0.4f) * anhedoniaModulation;
    
    // Apply sigmoid function for bounded response
    plasticityFactor = 1.0f / (1.0f + std::exp(-10.0f * (plasticityFactor - 0.5f)));
    
    return plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay of mood fluctuations towards baseline
    pImpl->moodLevel *= (1.0f - pImpl->moodDecay * dtFloat);
    
    // Decay of impulse control fluctuations
    pImpl->impulseControl *= (1.0f - pImpl->impulseControlDecay * dtFloat);
    
    // Apply homeostatic mood adjustment
    homeostaticMoodAdjustment();
    
    // Update history
    moodHistory.push_back(pImpl->moodLevel);
    impulseHistory.push_back(pImpl->impulseControl);
    
    // Maintain history sizes
    if (moodHistory.size() > 20) {
        moodHistory.erase(moodHistory.begin());
    }
    if (impulseHistory.size() > 20) {
        impulseHistory.erase(impulseHistory.begin());
    }
    
    // Update max/min mood records
    if (pImpl->moodLevel > pImpl->maxMoodRecorded) {
        pImpl->maxMoodRecorded = pImpl->moodLevel;
    }
    if (pImpl->moodLevel < pImpl->minMoodRecorded) {
        pImpl->minMoodRecorded = pImpl->moodLevel;
    }
    
    // Update social bond strength
    if (!pImpl->socialBondActive) {
        pImpl->bondingStrength *= 0.95f;  // Decay when not active
    }
    
    // Update time since last social interaction
    pImpl->lastSocialInteraction += dtFloat;
    
    // Adjust anxiety based on mood and social state
    if (pImpl->moodLevel < -0.5f) {
        pImpl->anxietyLevel = std::min(1.0f, pImpl->anxietyLevel + 0.01f * dtFloat);
    } else if (pImpl->moodLevel > 0.5f) {
        pImpl->anxietyLevel = std::max(0.0f, pImpl->anxietyLevel - 0.02f * dtFloat);
    }
}

void Serotonin::increaseMood(float amount) {
    // Increase mood level
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + amount);
    
    // Increase serotonin level
    if (pImpl->moodLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + amount * 0.5f);
    }
    
    // Enhance impulse control
    pImpl->impulseControl = std::max(0.0f, pImpl->impulseControl - amount * 0.3f);
}

void Serotonin::decreaseMood(float amount) {
    // Decrease mood level
    pImpl->moodLevel = std::max(-1.0f, pImpl->moodLevel - amount);
    
    // Decrease serotonin level
    pImpl->level = std::max(pImpl->baseline, pImpl->level - amount * 0.3f);
    
    // Reduce impulse control
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + amount * 0.3f);
}

void Serotonin::stabilizeMood(float targetMood) {
    // Move mood towards target with sigmoid function for smooth transition
    float moodDifference = targetMood - pImpl->moodLevel;
    float stabilizationForce = moodDifference * pImpl->stabilizationRate;
    
    pImpl->moodLevel += stabilizationForce;
    
    // Adjust serotonin level based on mood difference
    float serotoninAdjustment = moodDifference * 0.2f;
    pImpl->level = std::clamp(pImpl->level + serotoninAdjustment, pImpl->baseline, pImpl->peak);
}

void Serotonin::modulateImpulsivity(float amount) {
    // Modulate impulsivity level
    pImpl->impulseControl = std::clamp(pImpl->impulseControl + amount, 0.0f, 1.0f);
    
    // High impulse control reduces anhedonia
    if (pImpl->impulseControl > 0.8f) {
        pImpl->anhedonia = std::max(0.0f, pImpl->anhedonia - 0.1f);
    }
}

void Serotonin::enhanceImpulseControl(float duration) {
    // Enhance impulse control for a duration
    pImpl->impulseControl = std::min(1.0f, pImpl->impulseControl + 0.3f);
    
    // Schedule impulse control to decay after duration
    // In a real implementation, this would use timers
    // For now, just log the enhancement
    NLM_LOG_INFO("5-HT: Enhanced impulse control (duration: " + std::to_string(duration) + "s)");
}

void Serotonin::reduceImpulseControl(float reduction) {
    // Reduce impulse control
    pImpl->impulseControl = std::max(0.0f, pImpl->impulseControl - reduction);
    
    // Low impulse control increases anhedonia
    if (pImpl->impulseControl < 0.3f) {
        pImpl->anhedonia = std::min(1.0f, pImpl->anhedonia + 0.1f);
    }
}

void Serotonin::triggerSocialBehavior(bool socialInteraction) {
    if (socialInteraction) {
        // Positive social interaction increases social motivation
        pImpl->socialMotivation = std::min(1.0f, pImpl->socialMotivation + 0.2f);
        pImpl->socialBondActive = true;
        pImpl->lastSocialInteraction = 0.0f;
        
        // Increase bonding strength
        pImpl->bondingStrength = std::min(1.0f, pImpl->bondingStrength + 0.1f);
        
        // Positive social interaction improves mood
        increaseMood(0.2f);
        
        NLM_LOG_INFO("5-HT: Positive social interaction detected");
    } else {
        // Negative social interaction or lack of interaction
        pImpl->lastSocialInteraction += 10.0f; // Assume 10 seconds passed
        
        if (pImpl->lastSocialInteraction > 60.0f) {  // No interaction for 60 seconds
            pImpl->socialMotivation *= 0.9f;  // Decrease motivation
        }
    }
}

void Serotonin::enhanceSocialBonding(float bondingStrength) {
    // Enhance social bonding strength
    pImpl->bondingStrength = std::min(1.0f, pImpl->bondingStrength + bondingStrength);
    pImpl->socialBondActive = true;
    
    // Social bonding improves mood
    stabilizeMood(0.2f);  // Move towards moderately positive mood
}

void Serotonin::reduceSocialMotivation(float reduction) {
    // Reduce social motivation
    pImpl->socialMotivation = std::max(0.0f, pImpl->socialMotivation - reduction);
}

void Serotonin::homeostaticMoodAdjustment() {
    // Homeostatic adjustment towards ideal mood range
    float idealMood = 0.0f;  // Neutral mood is ideal
    float moodError = idealMood - pImpl->moodLevel;
    
    // Apply homeostatic drive
    float homeostaticForce = moodError * pImpl->homeostaticRate;
    pImpl->moodLevel += homeostaticForce;
    
    // Serotonin level adjusts based on mood error
    float serotoninAdjustment = moodError * 0.1f;
    pImpl->level = std::clamp(pImpl->level + serotoninAdjustment, pImpl->baseline, pImpl->peak);
}

void Serotonin::rewardPredictionErrorModulation(float error) {
    // Serotonin encodes reward prediction errors (inverse of dopamine)
    // Negative prediction errors (worse than expected) increase serotonin
    // Positive prediction errors (better than expected) decrease serotonin
    
    pImpl->rewardPredictionError = error;
    
    // Inverse relationship with dopamine's RPE
    if (error < 0.0f) {
        // Negative error (worse than expected) -> increase serotonin
        decreaseMood(-error * 0.5f);  // Decrease mood (negative emotion)
    } else {
        // Positive error (better than expected) -> decrease serotonin
        // Small increase in mood, but not as strong as dopamine response
        increaseMood(error * 0.3f);
    }
    
    // Modulate learning rate based on error
    pImpl->learningRateModulation = 1.0f + std::abs(error) * 0.2f;
}

void Serotonin::modulateEmotionalProcessing(std::vector<float>& emotionalInput) {
    // Modulate emotional processing based on serotonin level
    // Low serotonin = enhanced negative emotional processing
    // High serotonin = enhanced positive emotional processing
    
    float current5HTLevel = getLevel();
    
    // Calculate emotional modulation factor
    float emotionalModulation = 1.0f;
    if (current5HTLevel < pImpl->baseline) {
        // Low serotonin: enhance negative emotions
        emotionalModulation = 1.0f + (pImpl->baseline - current5HTLevel) * 2.0f;
    } else if (current5HTLevel > pImpl->baseline) {
        // High serotonin: enhance positive emotions
        emotionalModulation = 1.0f + (current5HTLevel - pImpl->baseline) * 1.5f;
    }
    
    // Apply modulation to emotional input
    for (float& input : emotionalInput) {
        input *= emotionalModulation;
        
        // Clip to reasonable range
        input = std::max(-10.0f, std::min(10.0f, input));
    }
}

void Serotonin::stabilizeLearningRate(float currentLearningRate) {
    // Serotonin stabilizes learning rate
    // High impulsivity/anxiety requires slower, more stable learning
    
    float instabilityFactor = (1.0f - pImpl->impulseControl) * pImpl->anxietyLevel;
    float stabilizedRate = currentLearningRate * (1.0f - instabilityFactor * 0.3f);
    
    // Store stabilized learning rate for use by other systems
    pImpl->learningRateModulation = stabilizedRate;
}

void Serotonin::regulateBehavioralResponse(float responseStrength) {
    // Regulate behavioral response based on serotonin state
    // High serotonin = more controlled, goal-directed behavior
    // Low serotonin = more impulsive, reactive behavior
    
    float behavioralModulation = 1.0f + (pImpl->impulseControl - 0.5f) * 2.0f;
    
    // Apply behavioral regulation
    float regulatedResponse = responseStrength * behavioralModulation;
    
    // In a real implementation, this would be used to modify action selection
}

void Serotonin::resetMoodFluctuations() {
    // Reset mood fluctuations to baseline
    pImpl->moodLevel = 0.0f;
    pImpl->moodInhibition = 0.5f;
    
    // Reset serotonin to baseline
    pImpl->level = pImpl->baseline;
    
    NLM_LOG_INFO("5-HT: Mood fluctuations reset");
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulseControl() const {
    return pImpl->impulseControl;
}

float Serotonin::getSocialMotivation() const {
    return pImpl->socialMotivation;
}

float Serotonin::getAnhedoniaLevel() const {
    return pImpl->anhedonia;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

float Serotonin::getBehavioralInhibition() const {
    return pImpl->behavioralInhibition;
}

} // namespace nlm