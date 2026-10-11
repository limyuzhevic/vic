// NeuralAccessor implementation with fast access structures

#include "NeuralAccessor.hpp"
#include "NeuralRegion.hpp"
#include "NeuralPopulation.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct NeuralAccessor::Impl {
    // Fast lookup tables - O(1) access
    std::unordered_map<NeuronId, Neuron*> neuronLookup;
    std::unordered_map<PopulationId, NeuralPopulation*> populationLookup;
    std::unordered_map<RegionId, NeuralRegion*> regionLookup;
    
    // Type-based indexing for fast population iteration
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    std::vector<Neuron*> internalNeurons;
    
    // Statistics
    size_t totalNeuronCount;
    size_t totalSynapseCount;
};

NeuralAccessor::NeuralAccessor() : pImpl(new Impl) {
    pImpl->totalNeuronCount = 0;
    pImpl->totalSynapseCount = 0;
}

NeuralAccessor::~NeuralAccessor() {
    // Clean up regions
    for (auto& pair : pImpl->regionLookup) {
        delete pair.second;
    }
    delete pImpl;
}

Neuron* NeuralAccessor::getNeuron(NeuronId id) {
    auto it = pImpl->neuronLookup.find(id);
    return it != pImpl->neuronLookup.end() ? it->second : nullptr;
}

const Neuron* NeuralAccessor::getNeuron(NeuronId id) const {
    auto it = pImpl->neuronLookup.find(id);
    return it != pImpl->neuronLookup.end() ? it->second : nullptr;
}

NeuralRegion* NeuralAccessor::getRegion(RegionId id) {
    auto it = pImpl->regionLookup.find(id);
    return it != pImpl->regionLookup.end() ? it->second : nullptr;
}

const NeuralRegion* NeuralAccessor::getRegion(RegionId id) const {
    auto it = pImpl->regionLookup.find(id);
    return it != pImpl->regionLookup.end() ? it->second : nullptr;
}

NeuralPopulation* NeuralAccessor::getPopulation(RegionId regionId, PopulationId populationId) {
    auto regionIt = pImpl->regionLookup.find(regionId);
    if (regionIt == pImpl->regionLookup.end()) return nullptr;
    
    NeuralRegion* region = regionIt->second;
    return region->getPopulation(populationId);
}

const NeuralPopulation* NeuralAccessor::getPopulation(RegionId regionId, PopulationId populationId) const {
    auto regionIt = pImpl->regionLookup.find(regionId);
    if (regionIt == pImpl->regionLookup.end()) return nullptr;
    
    const NeuralRegion* region = regionIt->second;
    return region->getPopulation(populationId);
}

std::vector<Neuron*> NeuralAccessor::getNeuronsOfType(RegionId regionId, NeuronType type) {
    std::vector<Neuron*> result;
    auto regionIt = pImpl->regionLookup.find(regionId);
    if (regionIt == pImpl->regionLookup.end()) return result;
    
    NeuralRegion* region = regionIt->second;
    for (auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == type) {
            auto neurons = pop->getNeurons();
            result.insert(result.end(), neurons.begin(), neurons.end());
        }
    }
    
    return result;
}

Neuron* NeuralAccessor::getSensoryNeuron(RegionId regionId, PopulationId populationId) {
    auto population = getPopulation(regionId, populationId);
    if (population && population->getNeuronType() == NeuronType::Sensory) {
        auto neurons = population->getNeurons();
        if (!neurons.empty()) {
            return neurons[0];
        }
    }
    return nullptr;
}

Neuron* NeuralAccessor::getMotorNeuron(RegionId regionId, PopulationId populationId) {
    auto population = getPopulation(regionId, populationId);
    if (population && population->getNeuronType() == NeuronType::Motor) {
        auto neurons = population->getNeurons();
        if (!neurons.empty()) {
            return neurons[0];
        }
    }
    return nullptr;
}

void NeuralAccessor::registerRegion(NeuralRegion* region) {
    if (region) {
        pImpl->regionLookup[region->getId()] = region;
    }
}

void NeuralAccessor::unregisterRegion(RegionId id) {
    auto it = pImpl->regionLookup.find(id);
    if (it != pImpl->regionLookup.end()) {
        pImpl->regionLookup.erase(it);
    }
}

void NeuralAccessor::registerNeuron(Neuron* neuron) {
    if (neuron) {
        NeuronId id = neuron->getId();
        pImpl->neuronLookup[id] = neuron;
        pImpl->totalNeuronCount++;
        
        // Add to type-specific cache
        NeuronType type = neuron->getType();
        if (type == NeuronType::Sensory) {
            pImpl->sensoryNeurons.push_back(neuron);
        } else if (type == NeuronType::Motor) {
            pImpl->motorNeurons.push_back(neuron);
        } else {
            pImpl->internalNeurons.push_back(neuron);
        }
    }
}

void NeuralAccessor::unregisterNeuron(NeuronId id) {
    auto it = pImpl->neuronLookup.find(id);
    if (it != pImpl->neuronLookup.end()) {
        Neuron* neuron = it->second;
        
        // Remove from type-specific cache
        NeuronType type = neuron->getType();
        std::vector<Neuron*>& cache = (type == NeuronType::Sensory) ? 
                                      pImpl->sensoryNeurons : 
                                      (type == NeuronType::Motor) ? 
                                      pImpl->motorNeurons : pImpl->internalNeurons;
        
        cache.erase(std::remove(cache.begin(), cache.end(), neuron), cache.end());
        
        pImpl->neuronLookup.erase(it);
        pImpl->totalNeuronCount--;
        delete neuron;
    }
}

} // namespace nlm