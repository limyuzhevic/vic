#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Modulates alertness, attention, and stress response

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal effects
    float getAlertness() const;
    void setAlertness(float alertness);
    
    // Stress response
    float getStressLevel() const;
    void setStressLevel(float stress);
    
    // Vigilance modulation
    float getVigilanceLevel() const;
    void setVigilanceLevel(float vigilance);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
