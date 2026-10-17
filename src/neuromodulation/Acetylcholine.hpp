#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced implementation with attention mechanisms
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Attention mechanisms
    void signalNovelty(float novelty);
    void signalAttention(bool focus);
    void setAttentionThreshold(float threshold);
    
    // State access
    float getAttentionLevel() const { return pImpl->attentionLevel; }
    float getNoveltySignal() const { return pImpl->noveltySignal; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
