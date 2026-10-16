#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

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
    void signalNovelty(float novelty);
    void enhanceWorkingMemory(const std::vector<float>& pattern);
    void modulateAttention(float globalInhibition);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm