#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Mood regulation
    void regulateMood(float moodLevel);
    float getMoodLevel() const { return mood_; }
    
    // Impulsivity control
    void controlImpulsivity(float impulseControl);
    float getImpulseControl() const { return impulseControl_; }
    
    // Social behavior influence
    void influenceSocialBehavior(float socialInfluence);
    void enhanceSocialLearning(float socialLearningStrength);
    
    // Apply serotonergic effects
    void applyToBrain(Brain* brain) const override;
    
    // Get decision making bias
    float getDecisionMakingBias() const { return decisionBias_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float mood_;
    float impulseControl_;
    float socialInfluence_;
    float socialLearningStrength_;
    float decisionBias_;
    std::vector<float> moodHistory_;
};

} // namespace nlm