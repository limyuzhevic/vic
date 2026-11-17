#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhances attention to sensory input and facilitates memory formation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get ACh level
    float getLevel() const;
    
    // Update ACh based on attention needs and memory consolidation requirements
    void update(TimestepDuration dt);
    
    // Get attention modulation factor
    float getAttentionFactor() const;
    
    // Get memory consolidation factor
    float getMemoryConsolidationFactor() const;
    
    // Set parameters
    void setAttentionWeight(float weight);
    void setMemoryWeight(float weight);
    void setBaselineLevel(float level);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm