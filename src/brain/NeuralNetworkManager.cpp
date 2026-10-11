#include "NeuralNetworkManager.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <iostream>

namespace nlm {

NeuralNetworkManager::NeuralNetworkManager()
    : stdp(nullptr), hebbian(nullptr), spikeSystem(nullptr), totalSpikesThisStep(0) {}

NeuralNetworkManager::~NeuralNetworkManager() {
    // Clean up neurons and synapses
    for (auto* neuron : allNeurons) {
        delete neuron;
    }
    for (auto& pair : synapseLookup) {
        delete pair.second;
    }
}

void NeuralNetworkManager::reset() {
    pendingSpikes.clear();
    pendingDelayedSpikes.clear();
    totalSpikesThisStep = 0;
    
    // Reset all neurons
    for (auto* neuron : allNeurons) {
        neuron->reset();
    }
}

void NeuralNetworkManager::updateNeurons(Timestamp currentTime, TimestepDuration dt) {
    // Update LIF dynamics for all neurons
    for (auto* neuron : allNeurons) {
        neuron->stepLIF(currentTime, dt);
    }
}

void NeuralNetworkManager::processSpikes(SimulationStep currentStep) {
    // Process queued spikes
    for (const auto& spike : pendingSpikes) {
        auto* neuron = getNeuron(spike.source_neuron);
        if (neuron) {
            // Record pre-synaptic spike for plasticity
            // Process spike effect on destination neurons
        }
    }
    
    pendingSpikes.clear();
    totalSpikesThisStep = pendingSpikes.size();
}

void NeuralNetworkManager::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spikes that have reached their delivery time
    auto it = pendingDelayedSpikes.begin();
    while (it != pendingDelayedSpikes.end()) {
        if (it->delivery_step <= currentStep) {
            auto* destinationNeuron = getNeuron(it->destination_neuron);
            if (destinationNeuron) {
                // Deliver synaptic input
                MembranePotential synapticCurrent = it->weight * 10.0f;  // Scale factor
                if (it->is_excitatory) {
                    destinationNeuron->receiveExcitatoryInput(synapticCurrent);
                } else {
                    destinationNeuron->receiveInhibitoryInput(-synapticCurrent);
                }
                
                // Record post-synaptic spike for plasticity if applicable
                if (stdp && destinationNeuron->getId() == it->destination_neuron) {
                    // TODO: Record spike for STDP
                }
            }
            it = pendingDelayedSpikes.erase(it);
        } else {
            ++it;
        }
    }
}

void NeuralNetworkManager::applyPlasticity(float plasticityMod) {
    // Apply STDP and Hebbian plasticity
    for (auto& pair : synapseLookup) {
        auto* syn = pair.second;
        
        // Apply STDP with neuromodulation
        if (syn->getPlasticityFlags().stdp) {
            // TODO: Implement STDP using stdp system
            float weight = syn->getWeight();
            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
            syn->setWeight(weight);
        }
        
        // Apply Hebbian learning
        if (syn->getPlasticityFlags().hebbian) {
            // TODO: Implement Hebbian learning
        }
        
        // Update synapse state
        syn->step(0.0);  // Use current time when called from Brain
    }
}

void NeuralNetworkManager::registerSpikes(const std::vector<Neuron*>& neurons, Timestamp time) {
    for (auto* neuron : neurons) {
        SpikeEvent event(neuron->getId(), time, 0);  // Step will be set by caller
        queueSpike(event);
        
        // Record pre-synaptic spike for incoming synapses (plasticity)
        auto incomingSynapses = getIncomingSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(time);
        }
        
        // Get outgoing synapses and schedule delayed spike events
        auto outgoingSynapses = getOutgoingSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
            // Create delayed spike event
            Delay delay = syn->getDelay();
            DelayedSpikeEvent delayedEvent(
                neuron->getId(),
                syn->getDestinationNeuron(),
                syn->getId(),
                syn->getWeight(),
                syn->getType(),
                time,
                time + delay * 0.001,  // Convert steps to time
                0,  // Current step (set by caller)
                0,  // Delivery step (set by caller)
                syn->getType() == SynapseType::Excitatory
            );
            queueDelayedSpike(delayedEvent);
            
            // Record pre-synaptic spike for plasticity
            syn->recordPreSpike(time);
        }
    }
}

void NeuralNetworkManager::queueSpike(const SpikeEvent& event) {
    pendingSpikes.push_back(event);
}

void NeuralNetworkManager::queueDelayedSpike(const DelayedSpikeEvent& event) {
    pendingDelayedSpikes.push_back(event);
}

void NeuralNetworkManager::addSynapse(NeuronId source, NeuronId destination, float weight,
                                     SynapseType type, const PlasticityFlags& flags,
                                     Delay delay) {
    auto* sourceNeuron = getNeuron(source);
    auto* destNeuron = getNeuron(destination);
    
    if (sourceNeuron && destNeuron) {
        auto synapseId = SynapseId(synapseLookup.size() + 1);
        auto* synapse = new Synapse(synapseId, source, destination, weight, type, flags, delay);
        synapseLookup[synapseId] = synapse;
        
        sourceNeuron->addOutgoingSynapse(synapseId);
        destNeuron->addIncomingSynapse(synapseId);
    }
}

void NeuralNetworkManager::removeSynapse(SynapseId id) {
    auto it = synapseLookup.find(id);
    if (it != synapseLookup.end()) {
        auto* synapse = it->second;
        
        // Update neuron synapse lists
        Neuron* sourceNeuron = getNeuron(synapse->getSourceNeuron());
        Neuron* destNeuron = getNeuron(synapse->getDestinationNeuron());
        
        if (sourceNeuron) {
            auto& outgoing = sourceNeuron->getOutgoingSynapses();
            outgoing.erase(std::remove(outgoing.begin(), outgoing.end(), synapse->getHandle()),
                          outgoing.end());
        }
        
        if (destNeuron) {
            auto& incoming = destNeuron->getIncomingSynapses();
            incoming.erase(std::remove(incoming.begin(), incoming.end(), synapse->getHandle()),
                          incoming.end());
        }
        
        delete synapse;
        synapseLookup.erase(it);
    }
}

Synapse* NeuralNetworkManager::getSynapse(SynapseId id) {
    auto it = synapseLookup.find(id);
    return it != synapseLookup.end() ? it->second : nullptr;
}

const Synapse* NeuralNetworkManager::getSynapse(SynapseId id) const {
    auto it = synapseLookup.find(id);
    return it != synapseLookup.end() ? it->second : nullptr;
}

void NeuralNetworkManager::addNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    NeuronId id = neuron->getId();
    if (neuronLookup.find(id) == neuronLookup.end()) {
        allNeurons.push_back(neuron);
        neuronLookup[id] = neuron;
    }
}

void NeuralNetworkManager::removeNeuron(NeuronId id) {
    auto it = neuronLookup.find(id);
    if (it != neuronLookup.end()) {
        Neuron* neuron = it->second;
        
        // Remove from synapse lists
        auto& incoming = neuron->getIncomingSynapses();
        for (SynapseHandle handle : incoming) {
            // Find and remove from synapseLookup
            for (auto& pair : synapseLookup) {
                if (pair.second->getHandle() == handle) {
                    // TODO: Remove synapse from synapseLookup
                    break;
                }
            }
        }
        
        auto& outgoing = neuron->getOutgoingSynapses();
        for (SynapseHandle handle : outgoing) {
            // Similar cleanup
        }
        
        allNeurons.erase(std::remove(allNeurons.begin(), allNeurons.end(), neuron), allNeurons.end());
        neuronLookup.erase(it);
        delete neuron;
    }
}

// Helper methods (will need to be added to Neuron and Synapse headers)
std::vector<Synapse*> NeuralNetworkManager::getIncomingSynapsesTo(NeuronId neuronId) {
    std::vector<Synapse*> result;
    for (auto& pair : synapseLookup) {
        if (pair.second->getDestinationNeuron() == neuronId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Synapse*> NeuralNetworkManager::getOutgoingSynapsesFrom(NeuronId neuronId) {
    std::vector<Synapse*> result;
    for (auto& pair : synapseLookup) {
        if (pair.second->getSourceNeuron() == neuronId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

} // namespace nlm
