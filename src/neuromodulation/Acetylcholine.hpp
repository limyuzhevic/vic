// Acetylcholine.hpp - Neuromodulator for attention and learning

#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine modulates attention and learning
// Increases neural excitability and facilitates LTP
// Important for working memory and attention

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    
    // Get attention enhancement factor
    float getAttentionFactor() const;
    
    // Update acetylcholine based on attention demands
    void update(TimestepDuration dt, float attentionDemand);
    
    // Signal important event to strengthen attention
    void signalImportantEvent(float importance);
    
    // Modulate neural excitability
    void enhanceAttention(class NeuralPopulation* population);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm