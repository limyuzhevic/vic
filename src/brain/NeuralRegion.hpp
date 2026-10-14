#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// PLACEHOLDER - Phase 2 will implement region-level dynamics

class NeuralRegion {
public:
    // Create region with ID
    explicit NeuralRegion(RegionId id) : id(id), nextSynapseId(1) {}
    
    // Create region with ID and name
    NeuralRegion(RegionId id, const std::string& name) : id(id), name(name), nextSynapseId(1) {}
    
    ~NeuralRegion() = default;
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&& other) noexcept : id(other.id), name(std::move(other.name)),
        populations(std::move(other.populations)), synapses(std::move(other.synapses)),
        outgoingSynapses(std::move(other.outgoingSynapses)),
        incomingSynapses(std::move(other.incomingSynapses)),
        nextSynapseId(other.nextSynapseId) {
        other.id = INVALID_REGION_ID;
        other.nextSynapseId = 1;
    }
    NeuralRegion& operator=(NeuralRegion&& other) noexcept {
        if (this != &other) {
            id = other.id;
            name = std::move(other.name);
            populations = std::move(other.populations);
            synapses = std::move(other.synapses);
            outgoingSynapses = std::move(other.outgoingSynapses);
            incomingSynapses = std::move(other.incomingSynapses);
            nextSynapseId = other.nextSynapseId;
            other.id = INVALID_REGION_ID;
            other.nextSynapseId = 1;
        }
        return *this;
    }
    
    // Identity
    RegionId getId() const { return id; }
    const std::string& getName() const { return name; }
    void setName(const std::string& name) { this->name = name; }
    
    // Population management
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal) {
        PopulationId popId(static_cast<uint64_t>(populations.size()) + 1);
        auto population = std::make_unique<NeuralPopulation>(popId, size);
        population->setNeuronType(type);
        populations.push_back(std::move(population));
        return popId;
    }
    
    NeuralPopulation* getPopulation(PopulationId id) {
        if (id.index() == 0 || id.index() > populations.size()) {
            return nullptr;
        }
        return populations[id.index() - 1].get();
    }
    
    const NeuralPopulation* getPopulation(PopulationId id) const {
        if (id.index() == 0 || id.index() > populations.size()) {
            return nullptr;
        }
        return populations[id.index() - 1].get();
    }
    
    size_t getPopulationCount() const { return populations.size(); }
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const { return populations; }
    std::vector<NeuralPopulation*> getAllPopulations() {
        std::vector<NeuralPopulation*> result;
        result.reserve(populations.size());
        for (auto& pop : populations) {
            result.push_back(pop.get());
        }
        return result;
    }
    
    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1) {
        SynapseId synId(nextSynapseId++);
        auto synapse = std::make_unique<Synapse>(synId, source, destination);
        synapse->setWeight(weight);
        synapse->setDelay(delay);
        
        outgoingSynapses[source].push_back(synId);
        incomingSynapses[destination].push_back(synId);
        
        synapses.push_back(std::move(synapse));
        return synId;
    }
    
    Synapse* getSynapse(SynapseId id) {
        for (auto& syn : synapses) {
            if (syn->getId() == id) {
                return syn.get();
            }
        }
        return nullptr;
    }
    
    const Synapse* getSynapse(SynapseId id) const {
        for (auto& syn : synapses) {
            if (syn->getId() == id) {
                return syn.get();
            }
        }
        return nullptr;
    }
    
    size_t getSynapseCount() const { return synapses.size(); }
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const { return synapses; }
    
    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron) {
        std::vector<Synapse*> result;
        auto it = outgoingSynapses.find(neuron);
        if (it != outgoingSynapses.end()) {
            for (SynapseId synId : it->second) {
                if (auto* syn = getSynapse(synId)) {
                    result.push_back(syn);
                }
            }
        }
        return result;
    }
    
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron) {
        std::vector<Synapse*> result;
        auto it = incomingSynapses.find(neuron);
        if (it != incomingSynapses.end()) {
            for (SynapseId synId : it->second) {
                if (auto* syn = getSynapse(synId)) {
                    result.push_back(syn);
                }
            }
        }
        return result;
    }
    
    // Statistics
    size_t getTotalNeuronCount() const {
        size_t total = 0;
        for (const auto& pop : populations) {
            total += pop->getSize();
        }
        return total;
    }
    
    size_t getActiveNeuronCount() const {
        size_t total = 0;
        for (const auto& pop : populations) {
            total += pop->getActiveCount();
        }
        return total;
    }
    
    size_t getFiringNeuronCount() const {
        size_t total = 0;
        for (const auto& pop : populations) {
            total += pop->getFiringCount();
        }
        return total;
    }
    
    float getAverageFiringRate() const {
        size_t popCount = populations.size();
        if (popCount == 0) return 0.0f;
        
        float sum = 0.0f;
        for (const auto& pop : populations) {
            sum += pop->getAverageFiringRate();
        }
        return sum / static_cast<float>(popCount);
    }
    
    // Local connectivity statistics
    float getAverageSynapticWeight() const {
        if (synapses.empty()) {
            return 0.0f;
        }
        float sum = 0.0f;
        for (const auto& syn : synapses) {
            sum += syn->getWeight();
        }
        return sum / static_cast<float>(synapses.size());
    }
    
    float getSynapticDensity() const {
        size_t neuronCount = getTotalNeuronCount();
        if (neuronCount == 0) return 0.0f;
        
        // Maximum possible connections: N * (N-1) for directed graph
        size_t maxConnections = neuronCount * (neuronCount - 1);
        if (maxConnections == 0) return 0.0f;
        
        return static_cast<float>(synapses.size()) / static_cast<float>(maxConnections);
    }
    
    // Step all populations and synapses
    void step(Timestamp currentTime) {
        // Step all populations
        for (auto& pop : populations) {
            pop->step(currentTime);
        }
        
        // Step all synapses
        for (auto& syn : synapses) {
            syn->step(currentTime);
        }
    }
    
    // Reset all neurons and synapses
    void reset() {
        for (auto& pop : populations) {
            pop->reset();
        }
        for (auto& syn : synapses) {
            syn->reset();
        }
    }
    
    // Initialize connectivity
    void initializeRandomConnectivity(RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance) {
        // Efficient random connectivity initialization
        // Creates synapses based on neuron types:
        // - Excitatory neurons -> all neurons (excitatory synapses)
        // - Inhibitory neurons -> all neurons (inhibitory synapses)
        
        // Get all neurons
        auto neurons = getAllNeurons();
        size_t neuronCount = neurons.size();
        
        // Create random connections
        for (size_t i = 0; i < neuronCount; ++i) {
            Neuron* preNeuron = neurons[i];
            NeuronType preType = preNeuron->getType();
            
            // Skip if not a proper source neuron
            if (preType == NeuronType::Modulatory) continue;
            
            for (size_t j = 0; j < neuronCount; ++j) {
                if (i == j) continue;  // No self-connections
                
                Neuron* postNeuron = neurons[j];
                
                // Probabilistic connection
                if (!rng.bernoulli(connectionProbability)) continue;
                
                // Determine synapse type based on pre-synaptic neuron type
                SynapseType synType;
                float weight;
                
                if (preType == NeuronType::Excitatory || 
                    preType == NeuronType::Sensory ||
                    preType == NeuronType::Motor ||
                    preType == NeuronType::Internal) {
                    // excitatory synapse
                    synType = SynapseType::Excitatory;
                    weight = meanWeight + rng.normal(0.0f, weightVariance);
                    weight = std::max(0.01f, weight);  // Ensure positive
                } else if (preType == NeuronType::Inhibitory) {
                    // inhibitory synapse
                    synType = SynapseType::Inhibitory;
                    weight = -(meanWeight + rng.normal(0.0f, weightVariance));
                    weight = std::min(-0.01f, weight);  // Ensure negative
                } else {
                    synType = SynapseType::Excitatory;
                    weight = meanWeight + rng.normal(0.0f, weightVariance);
                }
                
                // Add synapse
                SynapseId synId = addSynapse(preNeuron->getId(), postNeuron->getId(), weight, 1);
                
                // Initialize random properties
                if (Synapse* syn = getSynapse(synId)) {
                    syn->setType(synType);
                    syn->initializeRandom(rng);
                }
            }
        }
    }
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons() {
        std::vector<Neuron*> result;
        for (auto& pop : populations) {
            auto popNeurons = pop->getNeurons();
            result.insert(result.end(), popNeurons.begin(), popNeurons.end());
        }
        return result;
    }
    
    std::vector<const Neuron*> getAllNeurons() const {
        std::vector<const Neuron*> result;
        for (const auto& pop : populations) {
            auto popNeurons = pop->getNeurons();
            result.insert(result.end(), popNeurons.begin(), popNeurons.end());
        }
        return result;
    }
    
private:
    // NeuralRegion implementation details (moved from pImpl)
    RegionId id;
    std::string name;
    std::vector<std::unique_ptr<NeuralPopulation>> populations;
    std::vector<std::unique_ptr<Synapse>> synapses;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses;  // source -> synapse ids
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses;  // dest -> synapse ids
    SynapseId nextSynapseId;
};

} // namespace nlm
