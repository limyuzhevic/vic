#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with real biological mechanisms
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void modulateAttention(float attentionLevel);
    
    // Memory consolidation
    void triggerMemoryConsolidation();
    
    // cholinergic tone management
    void setAChTone(float tone);
    float getAChTone() const;
    
    // reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm