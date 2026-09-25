#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Uses comprehensive biologically accurate Hebbian learning rules including:
// - BCM (Bienenstock-Cooper-Munro) theory for stability
// - Oja's rule to prevent runaway excitation
// - Synaptic scaling for weight normalization
// - Anti-Hebbian learning for inhibitory synapses
// - Spike-based Hebbian rules with learning rate modulation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Learning rate and weight parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // BCM (Bienenstock-Cooper-Munro) parameters
    void setBCMThreshold(float theta);
    float getBCMThreshold() const;
    void setBCMThresholdRate(float rate);
    float getBCMThresholdRate() const;
    
    // Rule configuration flags
    void enableBCM(bool enable);
    void enableOja(bool enable);
    void enableSpikeBased(bool enable);
    void enableAntiHebbian(bool enable);
    void enableScaling(bool enable);
    
    // Activity normalization
    void setPostActivityNorm(float norm);
    float getPostActivityNorm() const;
    void setPreActivityNorm(float norm);
    float getPreActivityNorm() const;
    
    // Learning rate modulation
    void setLearningRateMod(float mod);
    float getLearningRateMod() const;
    void setGlobalLearningMod(float mod);
    float getGlobalLearningMod() const;
    void setScalingThreshold(float threshold);
    float getScalingThreshold() const;
    void setTimeDependentModulation(float mod);
    float getTimeDependentModulation() const;
    
    // STDP parameters for spike-based learning
    void setSTDPTimeWindow(float window);
    float getSTDPTimeWindow() const;
    void setSTDPMaxWeightChange(float change);
    float getSTDPMaxWeightChange() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
