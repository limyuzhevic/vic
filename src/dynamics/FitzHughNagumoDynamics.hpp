#pragma once

#include "NeuralDynamics.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // FHN model parameters
    void setA(float a);
    float getA() const;
    
    void setB(float b);
    float getB() const;
    
    void setTau(float tau);
    float getTau() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
