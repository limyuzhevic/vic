#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declaration for Brain
class Brain;

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get modulator level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Attention modulation
    void enhanceAttention(float attentionStrength);
    float getAttentionLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm