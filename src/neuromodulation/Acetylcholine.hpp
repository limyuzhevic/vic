#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention modulation and working memory
// Computational model based on Sarter & Bruno (2001) and Futropolis et al. (2014)
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh modulates attention and sensory processing
    void signalAttention(float attentionLevel);
    void signalWorkingMemoryLoad(float load);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm