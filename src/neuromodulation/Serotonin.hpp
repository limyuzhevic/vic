#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced with raphe nucleus dynamics, mood regulation, and social behavior modulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Mood regulation
    float getMoodLevel() const;
    void setMoodLevel(float mood);
    
    // Impulsivity modulation
    float getImpulsivityLevel() const;
    void setImpulsivityLevel(float level);
    
    // Social behavior modulation
    float getSocialBehaviorModulation() const;
    void setSocialBehaviorModulation(float modulation);
    
    // Raphe nucleus dynamics
    void setRapheNucleusActivity(float activity);
    float getRapheNucleusActivity() const;
    
    // Anxiety regulation
    float getAnxietyLevel() const;
    void setAnxietyLevel(float level);
    
    // Reward processing
    void processReward(float rewardValue);
    float getRewardProcessingModulation() const;
    
    // Sleep regulation
    void setSeroSleepModulation(float modulation);
    float getSeroSleepModulation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
