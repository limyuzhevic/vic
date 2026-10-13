#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// ACh enhances attention and memory encoding, modulates plasticity

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const { return "ACh"; }
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Memory consolidation effect
    void consolidateMemory(float strength);
    
    // Attention enhancement
    void enhanceAttention(float attentionGain);
    
    // Initialize with brain reference (like other neuromodulators)
    void initialize(class Brain* brain);
    
    // Reset function
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm