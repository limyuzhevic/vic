#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance (Phase 6)
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal detection functions
    void enhanceSignalDetection(float snrBoost);
    void enhanceNoveltyEncoding(float noveltyFactor);
    void increaseArousal(float arousalLevel);
    
    // Working memory and memory encoding
    void enhanceWorkingMemoryEncoding(float encodingStrength);
    void enhanceMemoryEncodingDuringNovelty(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm