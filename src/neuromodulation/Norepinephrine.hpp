#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance signaling
    void signalArousal(float arousal);
    void increaseVigilance(float vigilance);
    
    // Stress management
    void setStressLevel(float stress);
    float getStressLevel() const;
    
    // Getters and setters for parameters
    float getVigilanceFactor() const;
    void setVigilanceFactor(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm