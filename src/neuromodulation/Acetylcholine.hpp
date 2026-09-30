#pragma once

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
    
    // Attention modulation
    float getAttentionEnhancement() const;
    void signalAttentionShift();
    
    // Memory consolidation
    float getMemoryConsolidationStrength() const;
    void signalMemoryConsolidation();
    
    // Novelty detection
    void signalNovelty();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm