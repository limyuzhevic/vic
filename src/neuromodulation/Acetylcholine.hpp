#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Implemented: Modulates attention, memory consolidation, and learning
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void signalAttentionBoost(float boost);
    void signalMemoryConsolidation(float strength);
    
    // Accessors
    float getAttentionThreshold() const;
    float getMemoryConsolidationStrength() const;
    float getMemoryPriority() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm