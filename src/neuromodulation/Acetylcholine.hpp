#pragma once

#include "../core/Types/Types.hpp"

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
    
    // Signal attentional events
    void signalAttentionEvent();
    void signalNoveltyEvent();
    void signalWorkingMemoryEvent();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm