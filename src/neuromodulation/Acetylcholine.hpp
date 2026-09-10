#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation neuromodulator
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty = 0.0f, 
                float predictionError = 0.0f, float reward = 0.0f) override;
    float getExcitabilityModulator() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    
    // Attention signaling
    void enhanceAttention(float strength);
    
    // Memory consolidation signaling
    void promoteMemoryConsolidation(float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm