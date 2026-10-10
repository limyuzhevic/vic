#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// TODO PHASE 2: Implement real neuromodulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood/social signaling
    void signalMood(float mood);
    void signalSocialInteraction(float socialStrength);
    void reduceImpulsivity(float calmFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm