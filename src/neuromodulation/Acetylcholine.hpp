#pragma once

#include "../core/Types/Types.hpp"

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
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Attention modulation
    void signalAttention(float focusLevel);
    void signalTaskSwitching(float switchCost);
    
    // Memory consolidation
    void triggerMemoryConsolidation();
    
    // Get attention-related statistics
    float getAttentionLevel() const;
    float getWMEncodingEfficiency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
