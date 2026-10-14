#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

/**
 * @class Serotonin
 * @brief Mood, impulsivity, and social behavior neuromodulator
 * 
 * Serotonin (5-HT) regulates mood, impulsivity, aggression, and social behavior.
 * It influences emotional processing, social cognition, and decision-making.
 * Serotonin also modulates plasticity in response to social experiences.
 * 
 * @details Uses PImpl idiom for forward declaration to avoid circular
 * dependencies. Provides mood and social behavior modulation capabilities.
 */
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

} // namespace nlm
