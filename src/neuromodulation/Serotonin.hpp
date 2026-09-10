#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin (5-HT): Mood, impulsivity, and social behavior
// Modulates social behavior, reduces impulsivity, affects mood and decision making

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get plasticity factor for 5-HT-modulated learning
    float getPlasticityFactor() const override;
    
    // Update 5-HT dynamics
    void update(TimestepDuration dt) override;
    
    // Signal positive mood/social behavior
    void signalSocialBehavior(float socialStrength);
    
    // Signal mood improvement
    void enhanceMood(float moodImprovement);
    
    // Reduce impulsivity
    void reduceImpulsivity(float reductionStrength);
    
    // Get mood modulation factor
    float getMoodModulation() const;
    
    // Get impulsivity reduction factor
    float getImpulsivityReduction() const;
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm