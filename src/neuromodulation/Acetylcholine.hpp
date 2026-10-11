#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Modulates attention, arousal, and memory processes

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effects
    float getMemoryConsolidationBoost() const;
    void setMemoryConsolidationBoost(float boost);
    
    // Attention modulation
    float getAttentionModulation() const;
    void setAttentionModulation(float modulation);
    
    // Arousal effects
    float getArousalLevel() const;
    void setArousalLevel(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
