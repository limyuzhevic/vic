#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Phase 2: Biological model with attention gating and memory stabilization

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getBaselineLevel() const override;
    float getReceptorActivation() const override;
    float getExcitabilityModulation() const override;
    float getWorkingMemoryModulation() const override;
    float getEligibilityTraceModulation() const override;
    float getAttentionModulation() const override;
    float getLearningRateModulation() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(float value, const std::string& eventType) override;
    std::string getNeurotransmitterType() const override;
    void applyCrossModulation(float level, const std::string& sourceNeurotransmitter) override;
    float getActivityDependentRelease(float neuralActivity) const override;
    
    // Attention-related signaling
    void signalAttentionShift(float newFocusStrength);
    void signalWorkingMemoryEncoding(float memoryContent);
    void signalWorkingMemoryConsolidation(float memoryStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
