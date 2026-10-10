#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Learning, memory, attention, cortical activation
// Effects on attention and memory consolidation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    void setLevel(float level);
    
    // Get learning enhancement factor
    float getLearningFactor() const;
    
    // Get memory encoding enhancement
    float getMemoryEncodingFactor() const;
    
    // Get cortical activation level
    float getCorticalActivation() const;
    
    // Update acetylcholine dynamics
    void update(TimestepDuration dt);
    
    // Signal learning event
    void signalLearningEvent(float predictionError);
    
    // Signal attention demand
    void signalAttentionDemand(float demand);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm