#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Brain;

// Acetylcholine: Attention and memory consolidation neuromodulator
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Get modulator name
    const char* getName() const;
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Signal attention-boosting events
    void signalAttentionBoost(float strength);
    
    // Signal memory consolidation events
    void signalMemoryConsolidation(float efficacy);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm