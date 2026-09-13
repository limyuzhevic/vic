#pragma once

#include "../core/Types/Types.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    void modulateArousal(float arousalLevel);
    void enhanceVigilance(class AttentionalSelection* attention, float vigilanceBoost = 1.0f);
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithAcetylcholine(float achLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
