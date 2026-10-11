#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Increases arousal, enhances vigilance, and improves signal-to-noise ratio

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Increase arousal and vigilance for environmental monitoring
    void enhanceVigilance(float strength);
    
    // Improve signal-to-noise ratio for relevant stimuli
    void enhanceFocus(float strength);
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm