#pragma once

#include "Neuromodulator.hpp"
#include <boost/optional.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior regulation
// Biologically plausible model based on raphe nucleus serotonin system
// Implementing mood stabilization, impulsivity control, and social behavior modulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void increaseMood(float amount);
    void decreaseMood(float amount);
    void stabilizeMood(float targetMood);
    
    // Impulsivity control
    void modulateImpulsivity(float amount);
    void enhanceImpulseControl(float duration);
    void reduceImpulseControl(float reduction);
    
    // Social behavior regulation
    void triggerSocialBehavior(bool socialInteraction);
    void enhanceSocialBonding(float bondingStrength);
    void reduceSocialMotivation(float reduction);
    
    // Homeostatic mood regulation
    void homeostaticMoodAdjustment();
    void rewardPredictionErrorModulation(float error);
    
    // State access
    float getMoodLevel() const;
    float getImpulseControl() const;
    float getSocialMotivation() const;
    float getAnhedoniaLevel() const;
    float getAnxietyLevel() const;
    float getBehavioralInhibition() const;
    
    // Enhanced neuromodulation effects
    void modulateEmotionalProcessing(std::vector<float>& emotionalInput);
    void stabilizeLearningRate(float currentLearningRate);
    void regulateBehavioralResponse(float responseStrength);
    void resetMoodFluctuations();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm