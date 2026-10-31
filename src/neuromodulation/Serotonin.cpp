#include "Neuromodulator.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Current 5-HT concentration
    float baselineLevel;             // Baseline 5-HT level
    float moodLevel;                 // Current mood level (0-1)
    float impulsivityLevel;          // Current impulsivity level (0-1)
    float timeConstant;              // Time constant for dynamics (ms)
    
    // Social behavior modulation
    float socialBehavior;            // Social behavior modulation
    float rewardProcessing;          // Reward processing modulation
    float punishmentSensitivity;      // Sensitivity to punishment (0-1)
    
    // Emotional regulation
    float emotionalRegulation;       // Emotional regulation strength
    float anxietyLevel;              // Anxiety level (0-1)
    float threatSensitivity;         // Sensitivity to threat (0-1)
    
    // Cognitive effects
    float cognitiveFlexibility;       // Cognitive flexibility (0-1)
    float behavioralActivation;       // Behavioral activation level
    float behavioralInhibition;      // Behavioral inhibition level
    
    // Learning and memory
    float associativeLearning;       // Associative learning modulation
    float habitFormation;             // Habit formation modulation
    
    // Sleep and circadian rhythms
    float sleepDrive;                // Sleep drive (0-1)
    float circadianPhase;            // Circadian phase (0-1)
    
    // Stress response
    float stressResilience;          // Stress resilience (0-1)
    float stressResponse;            // Stress response activation (0-1)
    
    Impl() : level(0.0f)
        , baselineLevel(0.1f)
        , moodLevel(0.5f)
        , impulsivityLevel(0.5f)
        , timeConstant(120.0f)
        , socialBehavior(0.5f)
        , rewardProcessing(1.0f)
        , punishmentSensitivity(0.5f)
        , emotionalRegulation(0.5f)
        , anxietyLevel(0.5f)
        , threatSensitivity(0.5f)
        , cognitiveFlexibility(0.5f)
        , behavioralActivation(0.5f)
        , behavioralInhibition(0.5f)
        , associativeLearning(1.0f)
        , habitFormation(1.0f)
        , sleepDrive(0.5f)
        , circadianPhase(0.5f)
        , stressResilience(0.5f)
        , stressResponse(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates multiple aspects of mood, behavior, and learning
    float plasticityFactor = 1.0f;
    
    // Mood modulation: 5-HT generally promotes positive mood and reduces negative mood
    plasticityFactor *= (1.0f + pImpl->moodLevel * 0.6f);
    
    // Impulsivity regulation: higher 5-HT reduces impulsivity
    plasticityFactor *= (1.0f - pImpl->impulsivityLevel * 0.4f);
    
    // Reward processing: 5-HT modulates reward learning
    plasticityFactor *= pImpl->rewardProcessing;
    
    // Punishment sensitivity: higher 5-HT increases punishment sensitivity
    plasticityFactor *= (1.0f + pImpl->punishmentSensitivity * 0.3f);
    
    // Emotional regulation: 5-HT improves emotional regulation
    plasticityFactor *= (1.0f + pImpl->emotionalRegulation * 0.5f);
    
    // Anxiety and threat: 5-HT reduces anxiety and threat sensitivity
    plasticityFactor *= (1.0f - pImpl->anxietyLevel * 0.4f) * (1.0f - pImpl->threatSensitivity * 0.3f);
    
    // Cognitive flexibility: 5-HT promotes cognitive flexibility
    plasticityFactor *= (1.0f + pImpl->cognitiveFlexibility * 0.5f);
    
    // Behavioral activation and inhibition: 5-HT balances approach and avoidance behavior
    plasticityFactor *= (1.0f + pImpl->behavioralActivation * 0.3f - pImpl->behavioralInhibition * 0.2f);
    
    // Associative learning: 5-HT modulates associative learning
    plasticityFactor *= pImpl->associativeLearning;
    
    // Habit formation: 5-HT modulates habit formation vs goal-directed learning
    plasticityFactor *= (1.0f - pImpl->habitFormation * 0.4f);  // Less habit formation = more goal-directed learning
    
    // Sleep drive: 5-HT increases sleep drive
    plasticityFactor *= (1.0f + pImpl->sleepDrive * 0.2f);
    
    // Stress response: 5-HT modulates stress response
    plasticityFactor *= (1.0f + pImpl->stressResilience * 0.5f - pImpl->stressResponse * 0.3f);
    
    // Apply neuromodulator-specific factors
    plasticityFactor *= (1.0f + pImpl->level * 0.6f);  // Higher 5-HT = more modulation
    plasticityFactor = std::clamp(plasticityFactor, 0.0f, 5.0f);
    
    return plasticityFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Real 5-HT dynamics with multiple components
    
    // Convert dt to milliseconds
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update mood level based on current 5-HT level
    pImpl->moodLevel = 0.4f + pImpl->level * 0.35f;
    pImpl->moodLevel = std::clamp(pImpl->moodLevel, 0.1f, 1.0f);
    
    // Update impulsivity level based on mood and 5-HT
    pImpl->impulsivityLevel = 0.6f - pImpl->moodLevel * 0.4f + (1.0f - pImpl->level * 0.3f);
    pImpl->impulsivityLevel = std::clamp(pImpl->impulsivityLevel, 0.2f, 0.8f);
    
    // Update reward processing based on mood
    pImpl->rewardProcessing = 1.0f + pImpl->moodLevel * 0.3f;
    
    // Update punishment sensitivity based on mood and 5-HT
    pImpl->punishmentSensitivity = 0.3f + pImpl->moodLevel * 0.4f + pImpl->level * 0.2f;
    pImpl->punishmentSensitivity = std::clamp(pImpl->punishmentSensitivity, 0.3f, 0.8f);
    
    // Update emotional regulation based on 5-HT level
    pImpl->emotionalRegulation = 0.5f + pImpl->level * 0.4f;
    pImpl->emotionalRegulation = std::clamp(pImpl->emotionalRegulation, 0.5f, 1.0f);
    
    // Update anxiety level based on mood and stress
    pImpl->anxietyLevel = std::max(0.2f, 0.8f - pImpl->moodLevel * 1.5f);
    
    // Update threat sensitivity based on anxiety
    pImpl->threatSensitivity = 0.4f + pImpl->anxietyLevel * 0.4f;
    pImpl->threatSensitivity = std::clamp(pImpl->threatSensitivity, 0.4f, 0.9f);
    
    // Update cognitive flexibility based on mood
    pImpl->cognitiveFlexibility = 0.5f + pImpl->moodLevel * 0.4f;
    
    // Update behavioral activation and inhibition
    pImpl->behavioralActivation = 0.6f + pImpl->moodLevel * 0.3f;
    pImpl->behavioralInhibition = 0.4f + pImpl->moodLevel * 0.3f;
    
    // Update associative learning based on mood
    pImpl->associativeLearning = 1.0f + pImpl->moodLevel * 0.3f;
    
    // Update habit formation based on mood
    pImpl->habitFormation = 0.7f - pImpl->moodLevel * 0.4f;
    
    // Update sleep drive based on 5-HT level
    pImpl->sleepDrive = 0.3f + pImpl->level * 0.5f;
    pImpl->sleepDrive = std::clamp(pImpl->sleepDrive, 0.3f, 0.9f);
    
    // Update circadian phase
    pImpl->circadianPhase = 0.5f + pImpl->sleepDrive * 0.3f - pImpl->moodLevel * 0.2f;
    pImpl->circadianPhase = std::fmod(pImpl->circadianPhase, 1.0f);
    
    // Update stress resilience based on 5-HT level
    pImpl->stressResilience = 0.3f + pImpl->level * 0.6f;
    pImpl->stressResilience = std::clamp(pImpl->stressResilience, 0.3f, 0.9f);
    
    // Update stress response
    if (pImpl->moodLevel < 0.3f) {
        // Low mood increases stress response
        pImpl->stressResponse = std::min(pImpl->stressResponse + 0.05f * dtMs / 100.0f, 1.0f);
    } else {
        // High mood reduces stress response
        pImpl->stressResponse *= std::exp(-dtMs / 300.0f);
    }
    
    // Smooth 5-HT level update with gradual changes
    float targetLevel = pImpl->baselineLevel + (pImpl->level * 0.4f);  // Bias toward baseline
    float alpha = 0.1f;  // Smoothing factor
    float smoothedLevel = pImpl->level * (1.0f - alpha) + targetLevel * alpha;
    
    // Apply mood modulation
    float moodModulation = 1.0f + (pImpl->moodLevel - 0.5f) * 0.2f;
    pImpl->level = smoothedLevel * moodModulation;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

} // namespace nlm
