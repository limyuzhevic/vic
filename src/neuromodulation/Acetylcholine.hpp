#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

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
    
    // Attention signaling
    void signalAttention(float salience, float novelty);
    void enhanceMemory(const std::vector<float>& encoding);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
