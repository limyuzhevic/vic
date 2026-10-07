#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/NeuralWorkingMemory.hpp"

namespace nlm {

// Forward declarations
class SpikeSystem;
class Brain;
class Neuromodulator;

// NeuralDynamics manager class
// Handles neuron stepping, spike detection, spike scheduling,
// pre/post-synaptic spike recording for plasticity, and firing
// neuron storage to working memory.

class NeuralDynamics {
public:
    // Constructor
    NeuralDynamics(Brain* brain, SpikeSystem* spikeSystem);
    
    // Destructor
    ~NeuralDynamics();
    
    // Disable copying and moving
    NeuralDynamics(const NeuralDynamics&) = delete;
    NeuralDynamics& operator=(const NeuralDynamics&) = delete;
    NeuralDynamics(NeuralDynamics&&) = delete;
    NeuralDynamics& operator=(NeuralDynamics&&) = delete;
    
    // Main update method - processes one simulation step
    // 1. Updates all neurons (LIF dynamics)
    // 2. Detects spikes and schedules spike events
    // 3. Handles pre/post-synaptic spike recording for plasticity
    // 4. Stores firing neurons to working memory
    void updateStep(SimulationStep currentStep, Timestamp currentTime);
    
    // Set working memory reference
    void setWorkingMemory(NeuralWorkingMemory* workingMemory);
    
    // Set neuromodulator reference
    void setNeuromodulator(const Neuromodulator* neuromodulator);
    
    // Get statistics
    size_t getTotalSpikesThisStep() const;
    size_t getTotalSpikesTotal() const;
    size_t getFiringNeuronCount() const;
    
private:
    // Internal implementation structure
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
