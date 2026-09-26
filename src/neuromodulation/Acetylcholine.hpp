#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signals
    void signalAttention(float attention);
    void signalMemory(float memory);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setAttentionWeight(float weight);
    void setMemoryWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getAttentionWeight() const;
    float getMemoryWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm