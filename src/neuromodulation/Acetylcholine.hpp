#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhances attention, boosts memory formation, and stabilizes memories

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral effects
    void enhanceAttention(float signalStrength);
    void consolidateMemory(float memoryStrength);
    void enhanceFocus(float focusSignal);
    
    // Accessors
    float getAttentionGain() const;
    float getFocusStrength() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm