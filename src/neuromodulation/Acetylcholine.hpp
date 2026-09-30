#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention modulation, memory consolidation, cortical plasticity
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void signalAttention(float attentionSignal);
    float getAttentionLevel() const;
    
    // Memory consolidation
    void initiateMemoryConsolidation();
    float getMemoryConsolidationFactor() const;
    
    // Sensory processing enhancement
    void enhanceSensoryProcessing(float enhancementFactor);
    float getSensoryEnhancement() const;
    
    // Sleep/wake regulation
    void setWakeLevel(float wake) { setLevel(wake); }
    float getWakeLevel() const { return getLevel(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
