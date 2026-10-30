#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Mediates attention allocation, working memory encoding, and memory consolidation

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current ACh level
    float getLevel() const;
    void setLevel(float level);
    
    // Get attention modulation factor
    float getAttentionModulation() const;
    
    // Get memory consolidation strength
    float getConsolidationStrength() const;
    
    // Update ACh based on attention demands and memory operations
    void update(TimestepDuration dt, float attentionDemand, float memoryLoad);
    
    // Signal memory encoding (e.g., when storing to working memory)
    void signalEncoding(float strength);
    
    // Signal memory retrieval (e.g., when accessing working memory)
    void signalRetrieval(float strength);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm