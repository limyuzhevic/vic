#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Implemented: Modulates mood, impulse control, social motivation, and learning
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social modulation
    void signalMoodBoost(float boost);
    void signalPositiveSocialInteraction(float strength);
    void signalNegativeSocialFeedback(float penalty);
    
    // Accessors
    float getMoodLevel() const;
    float getImpulseControl() const;
    float getSocialMotivation() const;
    float getAnxietyLevel() const;
    float getSocialLearningStrength() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm