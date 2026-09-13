#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// ACh modulates attention, perception, working memory, and memory consolidation
// It enhances signal-to-noise ratio in neural circuits for focused processing

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
    
    // Biological mechanisms
    void enhanceAttention(float stimulusStrength);
    void initiateMemoryConsolidation();
    void modulateWorkingMemory();
    void signalAttentionShift();
    void decayMemoryTrace();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
