#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real-world based: ACh enhances attention to novel stimuli, boosts memory encoding and consolidation
// Clinical relevance: ACh deficits in Alzheimer's, targeted in ADHD treatments

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Get current ACh level
    float getLevel() const;
    void setLevel(float level);
    
    // Update ACh dynamics based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get ACh-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Advanced functionality
    void enhanceAttention(NeuronId targetNeuron);
    void boostMemoryConsolidation(TimestepDuration dt);
    
    // Configuration parameters
    void setAttentionWeight(float weight);
    void setNoveltyThreshold(float threshold);
    void setConsolidationBoost(float boost);
    
    float getAttentionWeight() const;
    float getNoveltyThreshold() const;
    float getConsolidationBoost() const;
    
    // Reset for new episode
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm