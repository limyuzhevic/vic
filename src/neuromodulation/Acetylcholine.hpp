#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Integrates with attention and memory systems

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get attention boost factor
    float getAttentionBoost() const;
    
    // Signal attention increase
    void signalAttention(float attentionStrength);
    
    // Enhance memory consolidation
    void enhanceMemory(float memoryPriority);
    
    // Update ACh state
    void update(TimestepDuration dt);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm