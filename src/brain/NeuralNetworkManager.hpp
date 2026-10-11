#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Neuron;
class Synapse;
class SpikeSystem;
class STDP;
class Hebbian;

class NeuralNetworkManager {
public:
    NeuralNetworkManager();
    ~NeuralNetworkManager();
    
    // Neural computation core
    void updateNeurons(Timestamp currentTime, TimestepDuration dt);
    void processSpikes(SimulationStep currentStep);
    void applyPlasticity(float plasticityMod);
    void registerSpikes(const std::vector<Neuron*>& neurons, Timestamp time);
    
    // Spike event management
    void queueSpike(const SpikeEvent& event);
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void processSpikes(SimulationStep currentStep);
    
    // Synapse management
    void addSynapse(NeuronId source, NeuronId destination, float weight, 
                   SynapseType type, const PlasticityFlags& flags,
                   Delay delay);
    void removeSynapse(SynapseId id);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    
    // Neuron accessor utilities
    void addNeuron(Neuron* neuron);
    void removeNeuron(NeuronId id);
    Neuron* getNeuron(NeuronId id);
    const Neuron* getNeuron(NeuronId id) const;
    
    // Accessors
    Neuron* getNeuron(NeuronId id) { return neuronLookup.at(id); }
    const Neuron* getNeuron(NeuronId id) const { return neuronLookup.at(id); }
    
    // Statistics
    size_t getNeuronCount() const { return allNeurons.size(); }
    size_t getSynapseCount() const { return synapseLookup.size(); }
    size_t getTotalSpikesThisStep() const { return totalSpikesThisStep; }
    
    // Integration with plasticity systems
    void setSTDP(STDP* stdp) { this->stdp = stdp; }
    void setHebbian(Hebbian* hebbian) { this->hebbian = hebbian; }
    void setSpikeSystem(SpikeSystem* spikeSystem) { this->spikeSystem = spikeSystem; }
    
    // Reset state
    void reset();
    
private:
    std::vector<Neuron*> allNeurons;
    std::unordered_map<NeuronId, Neuron*> neuronLookup;
    std::unordered_map<SynapseId, Synapse*> synapseLookup;
    
    // Neural computation state
    std::vector<SpikeEvent> pendingSpikes;
    std::vector<DelayedSpikeEvent> pendingDelayedSpikes;
    
    // Plasticity system integration
    STDP* stdp;
    Hebbian* hebbian;
    SpikeSystem* spikeSystem;
    
    // Statistics
    size_t totalSpikesThisStep;
};

} // namespace nlm
