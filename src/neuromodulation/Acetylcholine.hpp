#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Modulates attention to relevant stimuli, strengthens working memory,
// promotes memory consolidation, and modulates plasticity rules

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    void setLevel(float level);
    
    // Update acetylcholine based on attention and memory demands
    void update(TimestepDuration dt);
    
    // Get attention modulation factor
    float getAttentionModulation() const;
    
    // Get working memory modulation factor
    float getWorkingMemoryModulation() const;
    
    // Get plasticity modulation factor
    float getPlasticityModulation() const;
    
    // Set attention strength
    void setAttentionStrength(float strength);
    
    // Set memory consolidation rate
    void setConsolidationRate(float rate);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm