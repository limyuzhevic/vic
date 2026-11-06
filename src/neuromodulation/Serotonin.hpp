#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Phase 2: Implemented - mood modulation and social behavior effects

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social behavior modulation mechanisms
    void regulateMood(float anxiety, float socialContext);
    void updateDerivedStates();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Getters for derived states
    float getMood() const;
    float getImpulsivity() const;
    float getSocialBehavior() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm