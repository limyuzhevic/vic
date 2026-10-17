#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <string>

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
    
    // Attention and memory effects
    void enhanceAttention();
    void enhanceMemoryConsolidation();
    
    // Get current effects
    float getAttentionBoost() const;
    float getMemoryConsolidationBoost() const;
    
    // Signal processing
    float getTotalSignal() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm