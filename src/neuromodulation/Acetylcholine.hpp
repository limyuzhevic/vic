#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Attention modulation
    void modulateAttention(float attentionStrength);
    
    // Memory encoding
    void enhanceMemoryEncoding(float noveltyLevel);
    
    // Working memory maintenance
    void maintainWorkingMemory(float activation);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
