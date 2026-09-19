#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm
{

// Acetylcholine: Attention and memory consolidation
// Enhances working memory and attention during learning

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // ACh enhances synaptic plasticity, particularly LTP
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal learning event for memory encoding
    void signalLearningEvent(float novelty, float predictionError);
    
    // Enhance working memory encoding
    void enhanceMemoryEncoding(const std::vector<float>& pattern, float strength);
    
    // Boost attentional focus
    void boostAttention(float focusLevel);
    
    // Reset state
    void reset();
    
    // Get attention level (derived from ACh)
    float getAttentionLevel() const;
    
    // Get memory enhancement factor
    float getMemoryEnhancementFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm