#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Mood and behavioral signaling
    void signalMood(float mood);
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalAnxiety(float anxiety);
    void signalSatiety(float satiety);
    
    // Getters for behavioral parameters
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    float getSocialBehaviorLevel() const;
    float getAnxietyLevel() const;
    float getSatietyLevel() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm