#include "Norepinephrine.hpp"
#pragma once

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
    
    // Arousal-related signaling
    void signalArousal(float arousalLevel);
    void signalVigilance(float vigilance);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm