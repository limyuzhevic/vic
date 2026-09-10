// NeuronSystemUpdater.hpp - Handles neuron dynamics (LIF, spikes, spike propagation)
#pragma once

#include "BrainSystemUpdater.hpp"

namespace nlm {

class NeuronSystemUpdater : public BrainSystemUpdater {
public:
    virtual void update(Brain& brain, SimulationStep currentStep,
                       Timestamp currentTime, TimestepDuration timestep) override;
    virtual std::string getName() const override { return "NeuronSystemUpdater"; }
    
protected:
    // Process pending delayed spike events (deliver synaptic input)
    void processDelayedSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Update all neurons (LIF dynamics)
    void updateNeurons(Brain& brain, Timestamp currentTime, TimestepDuration timestep);
    
    // Detect spikes and schedule outgoing spike events
    void detectSpikes(Brain& brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Process immediate spikes
    void processSpikes(Brain& brain, SimulationStep currentStep);
};

} // namespace nlm
