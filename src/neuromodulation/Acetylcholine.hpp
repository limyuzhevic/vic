#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Memory encoding, attention gating, reinforcement learning
// Mediates between sensory input and cortical processing
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory encoding strength
    void enhanceMemoryEncoding(float encodingStrength);
    float getMemoryEncodingStrength() const;
    
    // Attention gating to sensory pathways
    void gateSensoryInput(NeuronId neuron, float gatingStrength);
    
    // Binding of distributed representations
    void facilitateFeatureBinding(const std::vector<NeuronId>& neuronIds);
    
    // Reward prediction error modulation
    void modulateRewardPredictionError(float predictionError);
    
    // Sleep-wake cycle modulation
    void enterSleepState();
    void enterWakeState();
    bool isAwake() const;
    
    // Update cholinergic tone based on behavioral state
    void updateCholinergicTone(bool isActive, float attentionLevel);
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
