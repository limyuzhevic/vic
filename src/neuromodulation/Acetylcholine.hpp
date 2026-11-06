#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 2: Implemented - attention modulation and memory consolidation effects

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation mechanisms
    void enhanceAttention(float predictionError, float novelty);
    void triggerMemoryConsolidation();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm