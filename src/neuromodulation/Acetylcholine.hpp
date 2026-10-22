#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation neuromodulator
// Biological role: Enhances attention, facilitates working memory, promotes memory consolidation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const { return "ACh"; }
    
    // Get current concentration/level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Attention modulation
    void enhanceAttention(float targetRegionActivation);
    float getAttentionLevel() const;
    
    // Memory consolidation
    void markForConsolidation(NeuronId neuronId);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm