#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine (ACh): Attention and memory consolidation
// Enhances focused attention and stabilizes memory traces
// Reduces inhibition for attended stimuli and strengthens memory consolidation

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get plasticity factor for ACh-modulated learning
    float getPlasticityFactor() const override;
    
    // Update ACh dynamics
    void update(TimestepDuration dt) override;
    
    // Signal attention focus for burst release
    void signalAttention(float attentionStrength);
    
    // Enhance memory consolidation
    void enhanceMemoryConsolidation(float memoryStrength);
    
    // Get attention modulation factor
    float getAttentionModulation() const;
    
    // Get memory consolidation strength
    float getMemoryConsolidationStrength() const;
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm