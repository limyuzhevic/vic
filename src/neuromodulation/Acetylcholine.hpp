#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

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
    void reset() override;
    
    // Attention modulation
    void modulateAttention(float attentionLevel);
    float getAttentionModulation() const;
    
    // Memory encoding enhancement
    void enhanceEncoding(float encodingStrength);
    void enhanceMemoryConsolidation(float consolidationFactor);
    
    // Apply cholinergic effects
    void applyToBrain(Brain* brain) const override;
    
    // Get memory enhancement factor
    float getMemoryEnhancement() const { return memoryEnhancement_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float attentionLevel_;
    float memoryEnhancement_;
    std::vector<float> encodingHistory_;
};

} // namespace nlm