#pragma once

#include "NeuralDynamics.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // Ion channel parameters
    float getSodiumReversal() const;
    void setSodiumReversal(float reversal);
    
    float getPotassiumReversal() const;
    void setPotassiumReversal(float reversal);
    
    float getLeakReversal() const;
    void setLeakReversal(float reversal);
    
    float getSodiumConductance() const;
    void setSodiumConductance(float conductance);
    
    float getPotassiumConductance() const;
    void setPotassiumConductance(float conductance);
    
    float getLeakConductance() const;
    void setLeakConductance(float conductance);
    
    float getCapacitance() const;
    void setCapacitance(float capacitance);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
