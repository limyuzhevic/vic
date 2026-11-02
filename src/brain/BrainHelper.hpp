#pragma once

#include "Brain.hpp"
#include <functional>
#include <vector>
#include <unordered_map>

namespace nlm {

// Helper class for managing complex brain operations
class BrainHelper {
public:
    explicit BrainHelper(Brain::Impl* brainImpl);
    
    // Process pending delayed spikes
    void processDelayedSpikeDelivery(SimulationStep currentStep, Timestamp currentTime);
    
    // Update all neurons with LIF dynamics
    void updateAllNeurons(SimulationStep currentStep, Timestamp currentTime);
    
    // Process immediate spikes
    void processSpikes(SimulationStep currentStep);
    void updateWorkingMemory();
    
    // Apply neuromodulation effects
    void applyNeuromodulationEffects();
    
    // Apply plasticity rules
    void applyPlasticityRules(float plasticityMod);
    
    // Homeostatic synaptic scaling
    void applyHomeostaticSynapticScaling(SimulationStep currentStep);
    
    // Update episodic memory
    void updateEpisodicMemory(SimulationStep currentStep);
    
    // Update prediction system
    void updatePredictionSystem();
    
    // Update attention system
    void updateAttentionSystem();
    
    // Update concept formation
    void updateConceptFormation();
    
    // Apply structural plasticity
    void applyStructuralPlasticity(SimulationStep currentStep);
    
    // Replay important memories
    void replayImportantMemories(SimulationStep currentStep);
    
    // Apply development effects
    void applyDevelopmentEffects(SimulationStep currentStep);
    
    // Periodic memory consolidation
    void consolidateMemories(SimulationStep currentStep);
    
    // Checkpoint management
    void manageCheckpoints(SimulationStep currentStep, Timestamp currentTime);
    
    // Get spike statistics
    struct SpikeStats {
        size_t totalSpikesThisStep = 0;
        size_t firingNeuronCount = 0;
        size_t activeMemoryNeurons = 0;
    };
    
    SpikeStats getStepStatistics() const;
    
    // Get neuron for ID (O(1) lookup)
    Neuron* getNeuronById(NeuronId id);
    
    // Inject current into sensory neurons
    void injectSensoryInput(const SensoryInput& input);
    
    // Inject current to specific neuron
    void injectCurrentToNeuron(NeuronId neuron, MembranePotential current);
    
    // Inject current to all neurons of specific type
    void injectCurrentToNeuronType(NeuronType type, MembranePotential current);
    
    // Store neuron in working memory
    void storeNeuronInWorkingMemory(Neuron* neuron);
    
    // Calculate excitation/inhibition ratio
    float calculateEIRatio() const;
    
    // Get all neurons across all regions
    std::vector<Neuron*> getAllNeurons() const;
    
    // Get all synapses across all regions
    std::vector<Synapse*> getAllSynapses() const;
    
    // Apply dopamine to all neurons
    void applyDopamineToNeurons(float dopamineLevel);
    
    // Generate action from motor neurons
    std::unique_ptr<Action> generateAction() const;
    
    // Update firing rates
    void updateFiringRates(Timestamp currentTime, TimestepDuration dt);
    
private:
    Brain::Impl* brainImpl_ = nullptr;
    
    // Helper methods for specific operations
    void detectSpikesInRegion(const std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                             SimulationStep currentStep, Timestamp currentTime);
    void scheduleSpikeEvents(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime);
    void applyDopamineModulation(float dopamineLevel);
    void applyPlasticityToSynapse(Synapse* syn, const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes, float plasticityMod);
    void scaleIncomingExcitatoryWeights(SimulationStep currentStep);
    void storeCurrentBrainStateInEpisodicMemory(SimulationStep currentStep);
    void updateWorkingMemoryActivity();
    void updateAttentionBasedOnWorkingMemory();
    void processDevelopmentChanges(SimulationStep currentStep);
    
    // Constants for homeostatic scaling
    static constexpr float kIncomingExcitatoryCap = 3.0f;
};

} // namespace nlm
