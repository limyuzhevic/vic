#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real implementation with Pimpl pattern

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Attention and memory enhancement
    void signalAttention(float attentionLevel);
    void signalLearning(float learningStrength);
    void enhanceWorkingMemory(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm