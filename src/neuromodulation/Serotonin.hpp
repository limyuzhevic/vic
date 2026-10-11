#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Reduces impulsivity, promotes social behavior, and modulates mood

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Decrease impulsive actions and improve self-control
    void reduceImpulsivity(float strength);
    
    // Enhance social behavior and reduce aggression
    void promoteSocialBehavior(float strength);
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm