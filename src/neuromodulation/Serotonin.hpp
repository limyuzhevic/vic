#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced with comprehensive behavioral inhibition and mood regulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced serotonin effects
    
    // Modulation for behavioral inhibition
    float getBehavioralInhibitionFactor() const;
    
    // Modulation for mood regulation
    float getMoodModulationFactor() const;
    
    // Modulation for impulsivity control
    float getImpulsivityControlFactor() const;
    
    // Modulation for social behavior
    float getSocialBehaviorModulation() const;
    
    // Modulation for anxiety reduction
    float getAnxietyReductionFactor() const;
    
    // Modulation for reward processing (indirect)
    float getRewardProcessingModulation() const;
    
    // Set serotonin parameters
    void setBaseline(float baseline);
    void setMoodModulation(float mood);
    void setBehavioralInhibition(float inhibition);
    void setSocialBehaviorModulation(float social);
    
    // Get serotonin state information
    float getBaseline() const;
    float getMoodModulation() const;
    float getBehavioralInhibition() const;
    float getSocialBehaviorModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm