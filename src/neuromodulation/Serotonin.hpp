#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood states and behavioral inhibition

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void improveMood(float amount);
    void reduceImpulsivity(float amount);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulsivity level
    float getImpulsivity() const;
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
