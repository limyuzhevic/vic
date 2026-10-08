#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced with comprehensive attention and memory modulation

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced acetylcholine effects
    
    // Modulation for selective attention
    float getAttentionModulation() const;
    
    // Modulation for memory encoding and consolidation
    float getMemoryEncodingFactor() const;
    float getMemoryConsolidationFactor() const;
    
    // Modulation for cortical excitability
    float getCorticalExcitabilityFactor() const;
    
    // Modulation for sensory gating
    float getSensoryGatingFactor() const;
    
    // Modulation for working memory maintenance
    float getWorkingMemoryMaintenanceFactor() const;
    
    // Modulation for feature binding
    float getFeatureBindingFactor() const;
    
    // Set acetylcholine parameters
    void setBaseline(float baseline);
    void setCholinergicTone(float tone);
    void setAttentionGain(float gain);
    
    // Get acetylcholine state information
    float getBaseline() const;
    float getCholinergicTone() const;
    float getAttentionGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm