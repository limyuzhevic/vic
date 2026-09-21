#pragma once

#include "NeuralDynamics.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// FitzHugh-Nagumo dynamics (simplified excitatory model)
// Implements a simplified excitable system based on Morris-Lecar dynamics
// Produces spike trains and oscillatory behavior suitable for biological modeling

class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Parameters
    void setThreshold(float threshold);
    float getThreshold() const;
    void setResetVoltage(float voltage);
    float getResetVoltage() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm