#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Implements ACh signaling for attention allocation and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal attention and memory effects
    void signalAttention(float attention);
    void signalNovelty(float novelty);
    void signalSensoryInput(float inputStrength);
    
    // Get neuromodulation effects
    float getAttentionStrength() const;
    float getMemoryConsolidation() const;
    float getWorkingMemoryBoost() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
