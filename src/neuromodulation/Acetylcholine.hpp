#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real biological implementation for cholinergic signaling

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention mechanisms
    void signalAttention(float stimulusSalience);
    void enhanceSensoryProcessing(float inputReliability);
    
    // Memory consolidation
    void enhanceMemoryConsolidation(float memoryStrength);
    void boostWorkingMemoryCapacity(float loadFactor);
    
    // Cholinergic dynamics
    void modulateArchingPattern(bool isArcing);
    void respondToNovelty(float noveltyScore);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm