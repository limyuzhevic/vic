#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhances attention, modulates memory encoding/consolidation

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
    
    // Signal novelty detection for attention modulation
    void signalNovelty(float novelty);
    
    // Enhance memory consolidation for specific neurons
    void enhanceMemory(const std::vector<NeuronId>& neurons, float strength);
    
    // Get attention gain modulation
    float getAttentionGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
