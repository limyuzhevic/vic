#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Cholinergic system implementation
// Integrates attention, memory consolidation, and cortical plasticity

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const { return "ACh"; }
    float getLevel() const { return level_; }
    void setLevel(float level);
    
    float getPlasticityFactor() const;
    void update(float dt);
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateNorepinephrine(float neLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // Memory modulation
    void enableMemoryConsolidation(bool enable);
    void setConsolidationStrength(float strength);
    
    // Attention modulation
    float getAttentionBias() const;
    void setAttentionBias(float bias);
    
    // State management
    void reset();
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
    
    // Memory consolidation
    bool consolidationEnabled_;
    float consolidationStrength_;
    
    // Attention
    float attentionBias_;
    float globalAttention_;
    
    // Integration weights
    float daWeight_;
    float neWeight_;
    float serotoninWeight_;
};

} // namespace nlm
