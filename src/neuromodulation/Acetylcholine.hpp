#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// TODO PHASE 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttention(float strength);
    void enhanceMemory(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm