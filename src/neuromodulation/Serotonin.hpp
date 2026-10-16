#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Implemented with biological realism for Phase 2

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // 5-HT-specific effects
    float getMoodEffect() const;
    float getImpulsivityEffect() const;
    float getSocialEffect() const;
    float getPlasticityEffect() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
