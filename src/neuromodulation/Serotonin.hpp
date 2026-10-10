#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood regulation, impulse control, social behavior, memory modulation
// Effects on attention and memory consolidation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    void setLevel(float level);
    
    // Get attention modulation factor (serotonin modulates attention focus)
    float getAttentionModulation() const;
    
    // Get memory consolidation enhancement
    float getMemoryConsolidationFactor() const;
    
    // Update serotonin dynamics
    void update(TimestepDuration dt);
    
    // Signal social reward or punishment
    void signalSocialReward(float reward);
    void signalSocialPunishment(float punishment);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm