#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal, vigilance, and cognitive flexibility
// Phase 2: Biological model with locus coeruleus dynamics and adaptive gain

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
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
    
    // Cognitive function signaling
    void signalNoveltyDetection(float noveltyStrength);
    void signalCognitiveFlexibility(float flexibilityLevel);
    void signalVigilance(float alertnessLevel);
    
    // Helper method to get novelty history
    float getNoveltyHistory() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
