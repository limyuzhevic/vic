#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "SpikeSystem.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include "StructuralPlasticity.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class NeuralRegion;
class Neuron;
class Synapse;
class Config;
class RandomGenerator;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

class BrainNeuralOperations {
public:
    // Forward declaration for friendship
    friend class Brain;
    
    BrainNeuralOperations() = default;
    ~BrainNeuralOperations() = default;
    
    // Core neural computation operations
    void processSpikes(SimulationStep currentStep);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(SimulationStep currentStep, Timestamp currentTime);
    void detectSpikes(SimulationStep currentStep, Timestamp currentTime);
    void queueSpike(const SpikeEvent& event);
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Input/output operations
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    void receiveSensoryInput(const class SensoryInput& input);
    std::unique_ptr<class Action> produceAction();
    
    // Statistics
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
private:
    // Helper functions for neural operations
    void deliverSynapticInput(const DelayedSpikeEvent& event);
    void updateWorkingMemory(const Neuron* neuron, float activation);
    void scheduleSpikes(const std::vector<SpikeEvent>& events);
    void updatePlasticity(float plasticityMod);
    
    // Internal state tracking
    size_t totalSpikesThisStep = 0;
    size_t totalSpikesTotal = 0;
    SimulationStep currentStep = 0;
    Timestamp currentTime = 0.0;
};

} // namespace nlm