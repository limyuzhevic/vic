#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    void initialize();
    
    // Get current level
    float getLevel() const;
    
    // Set level
    void setLevel(float level);
    
    // Update state
    void update(double dt);
    
    // Enhance memory encoding
    float enhanceMemoryEncoding() const;
    
    // Increase attention focus
    float increaseAttentionFocus() const;
    
    // Get attention modulation factor
    float getAttentionModulation() const { return attentionModulation_; }
    
    // Get memory consolidation effect
    float getMemoryConsolidationEffect() const { return memoryConsolidationEffect_; }
    
    // Clear state
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    float attentionModulation_;
    float memoryConsolidationEffect_;
    float baseline_;
};

} // namespace nlm
