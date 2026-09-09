#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, motivation, and social behavior
// PHASE 1: Full implementation with biological plausibility
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // 5-HT-specific functions for mood and motivation
    float getMoodLevel() const;
    void signalMoodChange(float moodDelta, bool positiveEvent);
    float getMotivationModulator() const;
    void modulateMotivation(bool isRewardingContext);
    float getSocialBehaviorModulator() const;
    void influenceSocialBehavior(float socialContext);
    
    // 5-HT effects on behavioral state
    void improveMood() const;
    void modulateRewardProcessing() const;
    void regulateImpulsivity() const;
    void enhanceSocialLearning() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm