#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Implemented: Modulates mood-dependent plasticity and social motivation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Improve mood and reduce impulsivity
    void improveMood(float amount);
    void reduceImpulsivity(float amount);
    
    // Get mood-impulsivity ratio (higher = more stable, less impulsive)
    float getMoodStability() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
