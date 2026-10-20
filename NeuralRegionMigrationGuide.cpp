// NeuralRegion.cpp Migration to MemoryPool Integration
// This file shows the key changes needed to migrate from std::make_unique to MemoryPool

#include "NeuralRegion.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct NeuralRegion::Impl {
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    std::vector<std::unique_ptr<Synapse>> synapses;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;  // source -> synapse ids
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;  // dest -> synapse ids
    SynapseId nextSynapseId;
    
    // NEW: MemoryPool integration
    std::unique_ptr<nlm::NeuronPool> neuronPool;
    std::unique_ptr<nlm::SynapsePool> synapsePool;
    bool useMemoryPool;
    
    explicit Impl(RegionId id) : id(id), nextSynapseId(1), useMemoryPool(true) {
        // Initialize MemoryPool instances for this region
        // Total neurons and synapses would be determined by parent Brain
        neuronPool = std::make_unique<nlm::NeuronPool>(1000);  // Region-specific allocation
        synapsePool = std::make_unique<nlm::SynapsePool>(10000);  // Region-specific allocation
    }
};

// Implementation of MemoryPool integration for NeuralRegion

NeuralRegion::NeuralRegion(RegionId id) : pImpl(new Impl(id)) {}

NeuralRegion::NeuralRegion(RegionId id, const std::string& name) : pImpl(new Impl(id)) {
    pImpl->name = name;
}

NeuralRegion::~NeuralRegion() = default;

NeuralRegion::NeuralRegion(NeuralRegion&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

NeuralRegion& NeuralRegion::operator=(NeuralRegion&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

RegionId NeuralRegion::getId() const {
    return pImpl->id;
}

const std::string& NeuralRegion::getName() const {
    return pImpl->name;
}

void NeuralRegion::setName(const std::string& name) {
    pImpl->name = name;
}

PopulationId NeuralRegion::addPopulation(size_t size, NeuronType type) {
    PopulationId popId(pImpl->populations.size() + 1);
    
    if (pImpl->useMemoryPool && pImpl->neuronPool) {
        // NEW: Use MemoryPool for neuron allocation in populations
        auto population = std::make_unique<NeuralPopulation>(popId, size);
        population->setNeuronType(type);
        
        // Inside NeuralPopulation::addNeuron, MemoryPool will be used
        for (size_t i = 0; i < size; ++i) {
            // This would use pImpl->neuronPool->allocate() to get NeuronId
            // Then convert to NeuronIndex for SoA access
            NeuronId neuronId = pImpl->neuronPool->allocate();
            NeuronIndex index = pImpl->neuronPool->getIndex(neuronId);
            
            // Create NeuronState and populate it
            NeuronState state;
            state.id = neuronId;
            state.type = type;
            state.regionId = pImpl->id;
            state.populationId = popId;
            
            // Initialize neuron state in MemoryPool
            // This is done inside neuronPool->allocate()
            
            // Add neuron to population using MemoryPool integration
            // (NeuralPopulation would need to be updated to support this)
        }
        
        pImpl->populations.push_back(std::move(population));
    } else {
        // Legacy implementation
        auto population = std::make_unique<NeuralPopulation>(popId, size);
        population->setNeuronType(type);
        pImpl->populations.push_back(std::move(population));
    }
    
    return popId;
}

NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

const NeuralPopulation* NeuralRegion::getPopulation(PopulationId id) const {
    if (id.index() == 0 || id.index() > pImpl->populations.size()) {
        return nullptr;
    }
    return pImpl->populations[id.index() - 1].get();
}

size_t NeuralRegion::getPopulationCount() const {
    return pImpl->populations.size();
}

const std::vector<std::unique_ptr<NeuralPopulation>>& NeuralRegion::getPopulations() const {
    return pImpl->populations;
}

std::vector<NeuralPopulation*> NeuralRegion::getAllPopulations() {
    std::vector<NeuralPopulation*> result;
    result.reserve(pImpl->populations.size());
    for (auto& pop : pImpl->populations) {
        result.push_back(pop.get());
    }
    return result;
}

SynapseId NeuralRegion::addSynapse(NeuronId source, NeuronId destination,
                                   SynapticWeight weight, Delay delay) {
    SynapseId synId(pImpl->nextSynapseId++);
    
    if (pImpl->useMemoryPool && pImpl->synapsePool) {
        // NEW: Use MemoryPool for synapse allocation
        SynapseId poolId = pImpl->synapsePool->allocate(source, destination);
        SynapseIndex index = pImpl->synapsePool->getIndex(poolId);
        
        // Create SynapseState and populate it
        SynapseState state;
        state.source = source;
        state.destination = destination;
        state.weight = weight;
        state.delay = delay;
        state.type = SynapseType::Excitatory;
        // Additional synapse properties would be set here
        
        // Create synapse wrapper for backward compatibility
        // (Synapse class would need to be updated to support MemoryPool)
    } else {
        // Legacy implementation
        auto synapse = std::make_unique<Synapse>(synId, source, destination);
        synapse->setWeight(weight);
        synapse->setDelay(delay);
        
        pImpl->outgoingSynapses[source].push_back(synId);
        pImpl->incomingSynapses[destination].push_back(synId);
        
        pImpl->synapses.push_back(std::move(synapse));
    }
    
    return synId;
}

Synapse* NeuralRegion::getSynapse(SynapseId id) {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

const Synapse* NeuralRegion::getSynapse(SynapseId id) const {
    for (auto& syn : pImpl->synapses) {
        if (syn->getId() == id) {
            return syn.get();
        }
    }
    return nullptr;
}

size_t NeuralRegion::getSynapseCount() const {
    return pImpl->synapses.size();
}

const std::vector<std::unique_ptr<Synapse>>& NeuralRegion::getSynapses() const {
    return pImpl->synapses;
}

std::vector<Synapse*> NeuralRegion::getSynapsesFrom(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->outgoingSynapses.find(neuron);
    if (it != pImpl->outgoingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

std::vector<Synapse*> NeuralRegion::getSynapsesTo(NeuronId neuron) {
    std::vector<Synapse*> result;
    auto it = pImpl->incomingSynapses.find(neuron);
    if (it != pImpl->incomingSynapses.end()) {
        for (SynapseId synId : it->second) {
            if (auto* syn = getSynapse(synId)) {
                result.push_back(syn);
            }
        }
    }
    return result;
}

// Remaining NeuralRegion methods continue...
// (The rest of NeuralRegion.cpp implementation remains unchanged)

} // namespace nlm
