#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Hodgkin-Huxley dynamics (detailed model)
// TODO PHASE 2: Implement Hodgkin-Huxley
class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics();
    ~HodgkinHuxleyDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // HH parameters
    void setCapacitance(float capacitance);
    float getCapacitance() const;
    void setMembraneResistance(float resistance);
    float getMembraneResistance() const;
    void setLeakConductance(float conductance);
    float getLeakConductance() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
