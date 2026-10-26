#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

// Neuromodulator: Acetylcholine - attention and memory consolidation
// Real implementation - affects sensory processing and memory consolidation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signal new attention to novel stimuli
    void signalAttention(float attentionStrength);
    
    // Boost memory encoding and consolidation
    void signalMemoryBoost(float boostStrength);
    
    // Get current attention modulation level
    float getAttentionModulation() const;
    
    // Get current memory boost level
    float getMemoryBoost() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Apply attention modulation to neural processing
    void applyAttentionModulation();
    
    // Apply memory boost effects
    void applyMemoryBoost(float boostStrength);
};

} // namespace nlm