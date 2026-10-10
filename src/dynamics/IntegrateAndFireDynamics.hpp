// IntegrateAndFireDynamics.hpp
#pragma once

#include "NeuralDynamics.hpp"

namespace nlm {

// Integrate-and-fire dynamics implementation
class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics();
    ~IntegrateAndFireDynamics() override;
    
    // From NeuralDynamics base class
    void updateNeuron(Neuron* neuron, TimestepDuration dt, Timestamp currentTime) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setMembraneTimeConstant(float tau);
    float getMembraneTimeConstant() const;
    
    // Additional parameters for enhanced LIF
    void setMembraneResistance(float resistance);
    float getMembraneResistance() const;
    
    // State management
    void setRefractoryPeriod(uint32_t steps);
    uint32_t getRefractoryPeriod() const;
    
    // Input handling
    void addExternalInput(float current);
    float getTotalExternalInput() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
