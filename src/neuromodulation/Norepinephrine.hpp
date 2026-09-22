#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

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
    
    // Modulate arousal and exploration
    void signalStimulation(float sensoryInput, float stressLevel);
    void increaseVigilance(float increment);
    
    // Get modulation effects
    float getArousalLevel() const;
    float getVigilanceFactor() const;
    float getExplorationDrive() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm