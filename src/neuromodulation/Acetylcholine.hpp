#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory consolidation signaling
    void signalAttention(float attention);
    void enhanceMemoryConsolidation(float memoryStrength);
    
    // Getters and setters for parameters
    float getAttentionFactor() const;
    void setAttentionFactor(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm