#pragma once

#include "Brain.hpp"
#include <vector>
#include <functional>

namespace nlm {

// Helper class for neuron stepping operations
class NeuronUpdater {
public:
    // Update all neurons in the brain regions
    static void updateAllNeurons(Brain::Impl* pImpl, Timestamp currentTime, TimestepDuration timestep);
    
    // Update a specific neuron
    static void updateNeuron(Neuron* neuron, Timestamp currentTime, TimestepDuration timestep);
    
    // Detect spikes and collect firing neurons
    static void detectSpikes(Brain::Impl* pImpl, SimulationStep currentStep, 
                            Timestamp currentTime, TimestepDuration timestep);
    
    // Process dopamine effects on neurons
    static void applyDopamineEffects(Brain::Impl* pImpl);
    
    // Store firing neurons to working memory
    static void storeToWorkingMemory(Brain::Impl* pImpl, Neuron* neuron, 
                                     const NeuronState& state);

private:
    NeuronUpdater() = delete;
    ~NeuronUpdater() = delete;
};

// Helper class for spike handling operations
class SpikeHandler {
public:
    // Process delayed spike events (deliver synaptic input)
    static void processDelayedSpikes(Brain::Impl* pImpl, SimulationStep currentStep, Timestamp currentTime);
    
    // Process immediate spike events
    static void processSpikes(Brain::Impl* pImpl, SimulationStep currentStep);
    
    // Queue a new spike event
    static void queueSpike(Brain::Impl* pImpl, const SpikeEvent& event);
    
    // Queue a new delayed spike event
    static void queueDelayedSpike(Brain::Impl* pImpl, const DelayedSpikeEvent& event);
    
    // Register spike handlers
    static void registerHandlers(Brain::Impl* pImpl);

private:
    SpikeHandler() = delete;
    ~SpikeHandler() = delete;
};

// Helper class for memory system operations
class MemorySystemUpdater {
public:
    // Update working memory
    static void updateWorkingMemory(Brain::Impl* pImpl, TimestepDuration timestep);
    
    // Update episodic memory
    static void updateEpisodicMemory(Brain::Impl* pImpl, SimulationStep currentStep);
    
    // Consolidate memories periodically
    static void consolidateMemories(Brain::Impl* pImpl, SimulationStep currentStep);
    
    // Replay important memories
    static void replayMemories(Brain::Impl* pImpl, SimulationStep currentStep);

private:
    MemorySystemUpdater() = delete;
    ~MemorySystemUpdater() = delete;
};

// Helper class for neuromodulation operations
class NeuromodulatorApplier {
public:
    // Update all neuromodulation systems
    static void updateAllNeuromodulators(Brain::Impl* pImpl, TimestepDuration timestep);
    
    // Apply neuromodulation effects to neurons
    static void applyNeuromodulationToNeurons(Brain::Impl* pImpl, float dopamineLevel);

private:
    NeuromodulatorApplier() = delete;
    ~NeuromodulatorApplier() = delete;
};

// Helper class for plasticity operations
class PlasticityUpdater {
public:
    // Apply all plasticity rules to synapses
    static void updateAllPlasticity(Brain::Impl* pImpl, SimulationStep currentStep,
                                   Timestamp currentTime, TimestepDuration timestep);
    
    // Apply STDP rule to a synapse
    static void applySTDP(Brain::Impl* pImpl, Synapse* syn, const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes, float plasticityMod);
    
    // Apply Hebbian learning to a synapse
    static void applyHebbian(Brain::Impl* pImpl, Synapse* syn, const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes);

private:
    PlasticityUpdater() = delete;
    ~PlasticityUpdater() = delete;
};

// Helper class for development operations
class DevelopmentUpdater {
public:
    // Apply developmental effects periodically
    static void updateDevelopment(Brain::Impl* pImpl, SimulationStep currentStep,
                                 TimestepDuration timestep);
    
    // Update plasticity rates based on developmental stage
    static void updatePlasticityBasedOnDevelopment(Brain::Impl* pImpl, DevelopmentalStage stage);

private:
    DevelopmentUpdater() = delete;
    ~DevelopmentUpdater() = delete;
};

} // namespace nlm