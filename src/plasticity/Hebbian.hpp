#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Enhanced Hebbian learning implementation
// Real-time spike-timing dependent Hebbian learning with NMDA receptor dynamics
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Plasticity rule interface
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Learning rate control
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Weight normalization and saturation control
    void setWeightSaturation(float saturation);
    float getWeightSaturation() const;
    
    // Synaptic depression control
    void setDepressionRate(float rate);
    float getDepressionRate() const;
    
    // NMDA receptor threshold control
    void setNMDAThreshold(float threshold);
    float getNMDAThreshold() const;
    
    // Homeostatic regulation
    void setHomeostaticWeight(float weight);
    float getHomeostaticWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

// Hebbian implementation notes:
// - Implements real spike-timing dependent Hebbian learning
// - NMDA receptor dynamics for temporal integration of coincident spikes
// - Homeostatic regulation to prevent runaway excitation
// - Synaptic depression for anti-Hebbian effects
// - Weight normalization to maintain stability
// - Configurable learning parameters for different scenarios
