#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Phase 2 implementation

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and focus functions
    void signalArousal(float arousalLevel);
    void enhanceFocus(const std::vector<NeuronId>& neurons, float intensity);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm