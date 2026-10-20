#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// NOT consciousness - cholinergic signaling for attention and memory
//
// Key mechanisms:
// - Modulates attention through attentional selection
// - Enhances memory consolidation during sleep/rest
// - Regulates arousal and alertness
// - Influences neural plasticity for learning

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Enhance memory consolidation (called during sleep/rest)
    void enhanceMemoryConsolidation();
    
    // Boost attention based on stimulus strength
    void boostAttention(float stimulusStrength);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Calculate attention demand from brain systems
    float calculateAttentionDemand() const;
};

} // namespace nlm