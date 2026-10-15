// Neuron iteration helpers implementation
// Provides common iteration patterns for neuron operations

#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Template helper functions for common iteration patterns

// Inject current to neurons of specific type
template<typename BrainPtr>
void injectCurrentToNeuronsOfType(BrainPtr brain, NeuronType type, MembranePotential current) {
    if (!brain) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Collect all neurons of specific type
template<typename BrainPtr>
std::vector<Neuron*> collectNeuronsOfType(BrainPtr brain, NeuronType type) {
    std::vector<Neuron*> neurons;
    if (!brain) return neurons;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neurons.push_back(neuron);
                }
            }
        }
    }
    
    return neurons;
}

// Calculate statistics for neurons
template<typename BrainPtr>
struct NeuronStats {
    size_t total;
    size_t active;
    size_t firing;
    float averageFiringRate;
    float averagePotential;
    float maxPotential;
    float minPotential;
};

template<typename BrainPtr>
NeuronStats calculateNeuronStats(BrainPtr brain) {
    NeuronStats stats = {0, 0, 0, 0.0f, 0.0f, -1000.0f, 1000.0f};
    if (!brain) return stats;
    
    float totalPotential = 0.0f;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                stats.total++;
                
                const auto& state = neuron->getState();
                if (state.firingState != FiringState::Resting) {
                    stats.active++;
                }
                
                if (neuron->isFiring()) {
                    stats.firing++;
                }
                
                if (state.membranePotential > stats.maxPotential) {
                    stats.maxPotential = state.membranePotential;
                }
                if (state.membranePotential < stats.minPotential) {
                    stats.minPotential = state.membranePotential;
                }
                
                totalPotential += state.membranePotential;
            }
        }
    }
    
    if (stats.total > 0) {
        stats.averageFiringRate = static_cast<float>(stats.firing) / stats.total;
        stats.averagePotential = totalPotential / stats.total;
    }
    
    return stats;
}

// Update neurons with specific type
template<typename BrainPtr>
void updateNeuronsOfType(BrainPtr brain, TimestepDuration timestep, NeuronType type) {
    if (!brain) return;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(brain->pImpl->currentTime, timestep);
                }
            }
        }
    }
}

// Collect synapses from specific source neuron
template<typename BrainPtr>
std::vector<Synapse*> collectOutgoingSynapses(BrainPtr brain, NeuronId sourceNeuronId) {
    std::vector<Synapse*> synapses;
    if (!brain) return synapses;
    
    for (auto& region : brain->pImpl->regions) {
        auto outgoingSynapses = region->getSynapsesFrom(sourceNeuronId);
        for (Synapse* syn : outgoingSynapses) {
            synapses.push_back(syn);
        }
    }
    
    return synapses;
}

// Collect synapses to specific destination neuron
template<typename BrainPtr>
std::vector<Synapse*> collectIncomingSynapses(BrainPtr brain, NeuronId destinationNeuronId) {
    std::vector<Synapse*> synapses;
    if (!brain) return synapses;
    
    for (auto& region : brain->pImpl->regions) {
        auto incomingSynapses = region->getSynapsesTo(destinationNeuronId);
        for (Synapse* syn : incomingSynapses) {
            synapses.push_back(syn);
        }
    }
    
    return synapses;
}

// Find neuron by ID
template<typename BrainPtr>
Neuron* findNeuronById(BrainPtr brain, NeuronId neuronId) {
    if (!brain) return nullptr;
    
    for (auto& region : brain->pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return neuron;
            }
        }
    }
    
    return nullptr;
}

// Find population by ID
template<typename BrainPtr>
NeuralPopulation* findPopulationById(BrainPtr brain, PopulationId populationId) {
    if (!brain) return nullptr;
    
    for (auto& region : brain->pImpl->regions) {
        if (region->getPopulation(populationId)) {
            return region->getPopulation(populationId);
        }
    }
    
    return nullptr;
}

// Get all neurons from all regions
template<typename BrainPtr>
std::vector<Neuron*> getAllNeurons(BrainPtr brain) {
    std::vector<Neuron*> neurons;
    if (!brain) return neurons;
    
    for (auto& region : brain->pImpl->regions) {
        auto regionNeurons = region->getAllNeurons();
        neurons.insert(neurons.end(), regionNeurons.begin(), regionNeurons.end());
    }
    
    return neurons;
}

// Get all synapses from all regions
template<typename BrainPtr>
std::vector<Synapse*> getAllSynapses(BrainPtr brain) {
    std::vector<Synapse*> synapses;
    if (!brain) return synapses;
    
    for (auto& region : brain->pImpl->regions) {
        auto regionSynapses = region->getSynapses();
        synapses.insert(synapses.end(), regionSynapses.begin(), regionSynapses.end());
    }
    
    return synapses;
}

// Update neuromodulation effects on all neurons
template<typename BrainPtr>
void applyNeuromodulationToAllNeurons(BrainPtr brain) {
    if (!brain) return;
    
    float dopamineLevel = 0.0f;
    if (brain->pImpl->dopamine) {
        dopamineLevel = brain->pImpl->dopamine->getLevel();
    }
    
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

// Calculate connectivity statistics
template<typename BrainPtr>
struct ConnectivityStats {
    size_t totalConnections;
    size_t excitatoryConnections;
    size_t inhibitoryConnections;
    float averageConnectionWeight;
    float maxConnectionWeight;
    float minConnectionWeight;
};

template<typename BrainPtr>
ConnectivityStats calculateConnectivityStats(BrainPtr brain) {
    ConnectivityStats stats = {0, 0, 0, 0.0f, -1000.0f, 1000.0f};
    if (!brain) return stats;
    
    float totalWeight = 0.0f;
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            stats.totalConnections++;
            float weight = syn->getWeight();
            totalWeight += weight;
            
            if (weight > 0.0f) {
                stats.excitatoryConnections++;
            } else if (weight < 0.0f) {
                stats.inhibitoryConnections++;
            }
            
            if (weight > stats.maxConnectionWeight) {
                stats.maxConnectionWeight = weight;
            }
            if (weight < stats.minConnectionWeight) {
                stats.minConnectionWeight = weight;
            }
        }
    }
    
    if (stats.totalConnections > 0) {
        stats.averageConnectionWeight = totalWeight / stats.totalConnections;
    }
    
    return stats;
}

} // namespace nlm
