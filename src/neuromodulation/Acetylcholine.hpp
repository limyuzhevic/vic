#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention, memory consolidation, and learning modulation
// 
// Key functions:
// - Modulates attention and signal detection
// - Enhances memory encoding and consolidation
// - Regulates learning rate for plasticity
// - Increases cortical excitability
// - Supports working memory maintenance

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get ACh level
    float getLevel() const;
    void setLevel(float level);
    
    // Get neuromodulator name
    const char* getName() const;
    
    // Get neuromodulator effects on plasticity
    float getPlasticityFactor() const;
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Signal specific cholinergic effects
    // Enhance attentional filtering and selection
    void signalAttentionGain(float gain);
    
    // Enhance memory encoding and consolidation
    void signalMemoryEncoding(float strength);
    
    // Modulate learning rate for plasticity rules
    void signalLearningRate(float rate);
    
    // Increase cortical excitability
    void signalCorticalExcitability(float excitability);
    
    // Get current attention bias (0.0f to 1.0f+)
    float getAttentionBias() const;
    
    // Get memory encoding enhancement factor (1.0f+)
    float getMemoryEncodingGain() const;
    
    // Get learning rate modulation factor (0.5f to 2.0f)
    float getLearningRate() const;
    
    // Get cortical excitability factor (0.5f to 2.0f)
    float getCorticalExcitability() const;
    
    // Reset for new learning episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm