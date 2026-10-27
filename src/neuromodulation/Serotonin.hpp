#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood regulation, punishment processing, and behavioral flexibility
class Serotonin : public Neuromodulator {
public:
    Serotonin() : pImpl(new Impl) {}
    ~Serotonin() override = default;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl ? pImpl->level : 0.0f; }
    void setLevel(float level) override { if (pImpl) pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    
    float getPlasticityFactor() const override { return pImpl ? pImpl->plasticityFactor : 1.0f; }
    void update(TimestepDuration dt) override { if (pImpl) pImpl->update(dt); }
    
    // Mood regulation
    void setMoodLevel(float level) { if (pImpl) pImpl->moodLevel = level; }
    float getMoodLevel() const { return pImpl ? pImpl->moodLevel : 0.0f; }
    
    // Punishment processing
    void triggerPunishment(float magnitude) { if (pImpl) pImpl->triggerPunishment(magnitude); }
    bool isPunishmentActive() const { return pImpl ? pImpl->punishmentActive : false; }
    
    // Behavioral flexibility
    void setBehavioralFlexibility(float level) { if (pImpl) pImpl->behavioralFlexibility = level; }
    float getBehavioralFlexibility() const { return pImpl ? pImpl->behavioralFlexibility : 0.0f; }
    
    // Social behavior
    void setSocialBehavior(float level) { if (pImpl) pImpl->socialBehavior = level; }
    float getSocialBehavior() const { return pImpl ? pImpl->socialBehavior : 0.0f; }
    
    // Inhibitory control
    void enhanceInhibitoryControl(float level) { if (pImpl) pImpl->enhanceInhibitoryControl(level); }
    float getInhibitoryControl() const { return pImpl ? pImpl->inhibitoryControl : 0.0f; }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        
        // Mood parameters
        float moodLevel;
        float sadnessLevel;
        float happinessLevel;
        
        // Punishment parameters
        float punishmentActive;
        float punishmentMagnitude;
        float aversiveLearning;
        
        // Behavioral parameters
        float behavioralFlexibility;
        float explorationInhibition;
        float behavioralApproach;
        
        // Social parameters
        float socialBehavior;
        float socialApproach;
        float socialWithdrawal;
        
        // Control parameters
        float inhibitoryControl;
        float impulseControl;
        float decisionConfidence;
        
        Impl() : level(0.0f), baseline(0.03f), peak(1.0f), decayRate(0.04f), releaseRate(2.0f),
                 plasticityFactor(1.0f), moodLevel(0.5f), sadnessLevel(0.0f), happinessLevel(0.5f),
                 punishmentActive(0.0f), punishmentMagnitude(0.0f), aversiveLearning(0.0f),
                 behavioralFlexibility(0.5f), explorationInhibition(0.5f), behavioralApproach(0.5f),
                 socialBehavior(0.5f), socialApproach(0.5f), socialWithdrawal(0.0f),
                 inhibitoryControl(0.5f), impulseControl(0.5f), decisionConfidence(0.5f) {}
        
        void update(TimestepDuration dt) {
            // Decay serotonin level
            level = std::max(baseline, level - decayRate * static_cast<float>(dt));
            
            // Punishment response decay
            if (punishmentActive > 0.0f) {
                punishmentActive = std::max(0.0f, punishmentActive - 0.15f * static_cast<float>(dt));
                aversiveLearning = std::max(0.0f, aversiveLearning - 0.1f * static_cast<float>(dt));
            }
            
            // Mood dynamics
            moodLevel = std::clamp(moodLevel, 0.0f, 1.0f);
            happinessLevel = 1.0f - sadnessLevel;
        }
        
        void triggerPunishment(float magnitude) {
            punishmentActive = 1.0f;
            punishmentMagnitude = magnitude;
            aversiveLearning = magnitude * 1.5f;  // Strong learning from punishment
        }
        
        void enhanceInhibitoryControl(float level) {
            inhibitoryControl = level;
            impulseControl = level * 0.8f;
            decisionConfidence = level * 0.9f;
        }
    };
    
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
