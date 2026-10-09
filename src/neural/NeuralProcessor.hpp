#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain; // Forward declaration

class NeuralProcessor {
public:
    NeuralProcessor(Brain* brain, SpikeSystem* spikeSystem);
    
    // Execute all neuronal dynamics operations
    void updateNeuronalDynamics(TimestepDuration dt);
    
    // Process spike events and deliver synaptic input
    void processSpikes();
    
    // Process delayed spike events (synaptic transmission)
    void processDelayedSpikes(Timestamp currentTime);
    
    // Detect neuron firing and schedule spike events
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Deliver synaptic input to destination neurons
    void deliverSynapticInput(const DelayedSpikeEvent& event);
    
    // Update neuron states for plasticity eligibility
    void updatePlasticityEligibility(SimulationStep currentStep, Timestamp currentTime);
    
    // Get access to internal components
    std::vector<Neuron*> getAllNeurons() const;
    std::vector<Synapse*> getAllSynapses() const;
    
private:
    Brain* brain;
    SpikeSystem* spikeSystem;
    
    // Event-driven spike delivery
    void processSpikes(const std::vector<SpikeEvent>& spikeEvents);
    void queueDelayedSpike(const SpikeEvent& spikeEvent, 
                          const Synapse* synapse, 
                          Timestamp currentTime);
};
