#include "Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Helper function to find or create synapse from checkpoint data
SynapseId Brain::Impl::findOrCreateSynapse(const SynapseCheckpointData& data, size_t index) {
    NeuronId sourceId(data.sourceNeuron[index]);
    NeuronId destId(data.destinationNeuron[index]);
    
    // Try to find existing synapse between these neurons
    for (auto& region : regions) {
        // Get all neurons in this region
        auto neurons = region->getAllNeurons();
        
        // Check if source and destination neurons exist in this region
        Neuron* sourceNeuron = nullptr;
        Neuron* destNeuron = nullptr;
        
        for (auto* neuron : neurons) {
            if (neuron->getId() == sourceId) {
                sourceNeuron = neuron;
            }
            if (neuron->getId() == destId) {
                destNeuron = neuron;
            }
        }
        
        // If both neurons found, check if synapse exists or create new one
        if (sourceNeuron && destNeuron) {
            // Check if synapse already exists
            auto synapsesFrom = region->getSynapsesFrom(sourceId);
            for (auto* syn : synapsesFrom) {
                if (syn->getDestinationNeuron() == destId) {
                    // Synapse exists, update its state
                    syn->setWeight(data.weight[index]);
                    syn->setDelay(data.delay[index]);
                    syn->setType(static_cast<SynapseType>(data.synapseType[index]));
                    syn->setEligibilityTrace(data.eligibilityTrace[index]);
                    syn->setEfficacy(data.efficacy[index]);
                    
                    // Set plasticity flags
                    PlasticityFlags& flags = syn->getPlasticityFlags();
                    flags.hebbian = (data.plasticityFlags[index] & 1) != 0;
                    flags.stdp = (data.plasticityFlags[index] & 2) != 0;
                    flags.reward_modulated = (data.plasticityFlags[index] & 4) != 0;
                    flags.structural = (data.plasticityFlags[index] & 8) != 0;
                    flags.eligible = (data.plasticityFlags[index] & 16) != 0;
                    
                    return syn->getId();
                }
            }
            
            // Create new synapse
            return region->addSynapse(sourceId, destId, data.weight[index], data.delay[index]);
        }
    }
    
    // If neurons not found, this is an error - return invalid ID
    return SynapseId(UINT64_MAX);
}