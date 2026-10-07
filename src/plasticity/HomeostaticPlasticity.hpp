#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Homeostatic plasticity rule for firing rate stability
// Implements target firing rate regulation to prevent runaway excitation or silencing
class HomeostaticPlasticity : public PlasticityRule {
public:
    HomeostaticPlasticity();
    ~HomeostaticPlasticity() override;
    
    // Update synaptic weights based on firing rate deviation from target
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic parameters
    void setTargetFiringRate(float rate);    // Target average firing rate (Hz)
    float getTargetFiringRate() const;
    void setLearningRate(float rate);        // Strength of homeostatic adjustment (0-1)
    float getLearningRate() const;
    void setTimeConstant(float tau);         // Time constant for homeostatic dynamics (ms)
    float getTimeConstant() const;
    
    // Neuron state tracking for homeostatic regulation
    void recordNeuronActivity(Neuron* neuron, Timestamp currentTime, float currentTimeMs);
    void resetNeuronHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
