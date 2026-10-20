#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT signals reward and social interactions, affects mood and impulsivity
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Disable copying
    Serotonin(const Serotonin&) = delete;
    Serotonin& operator=(const Serotonin&) = delete;
    
    // Disable moving
    Serotonin(Serotonin&&) = delete;
    Serotonin& operator=(Serotonin&&) = delete;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get 5-HT-specific plasticity factor
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal reward event
    void signalReward(float intensity);
    
    // Signal social interaction
    void signalSocialEvent(float value);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm