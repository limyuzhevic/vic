#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include "../performance/MemoryPool.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// PLACEHOLDER - Phase 2 will implement region-level dynamics

class NeuralRegion {
public:
    // Create region with ID using memory pool
    static NeuralRegion* createRegion(RegionId id);
    
    // Create region with ID and name using memory pool
    static NeuralRegion* createRegion(RegionId id, const std::string& name);
    
    // Destroy region
    static void destroyRegion(NeuralRegion* region);
    
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Population management
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
    /**
     * Reserve memory for region
     */
    void reserveMemory(size_t neuronCount, size_t synapseCount) {
        neuronPoolManager_.reserveNeurons(neuronCount);
        synapsePoolManager_.reserveSynapses(synapseCount);
    }
    
    /**
     * Get memory statistics
     */
    size_t getMemoryUsage() const {
        return neuronPoolManager_.getAllocatedCount() * sizeof(Neuron) +
               synapsePoolManager_.getAllocatedCount() * sizeof(Synapse);
    }

private:
    struct Impl;
    Impl* pImpl;
    
    // Friend class for memory management
    friend class NeuralRegionMemoryPool;
};

/**
 * NeuralRegion memory pool manager
 */
class NeuralRegionMemoryPool {
public:
    static NeuralRegionMemoryPool& getInstance() {
        static NeuralRegionMemoryPool instance;
        return instance;
    }
    
    NeuralRegion* createRegion(RegionId id, const std::string& name = "") {
        NeuralRegion* region = pool_.allocate();
        if (region) {
            // Initialize region state
            new (&region->pImpl) NeuralRegion::Impl();
            region->pImpl->id = id;
            region->pImpl->name = name;
        }
        return region;
    }
    
    void destroyRegion(NeuralRegion* region) {
        if (region) {
            region->~NeuralRegion();
            pool_.deallocate(region);
        }
    }
    
    void reserveRegions(size_t count) {
        pool_.reserve(count);
    }
    
    size_t getAllocatedCount() const {
        return pool_.allocatedCount();
    }
    
    size_t getAvailableCount() const {
        return pool_.availableCount();
    }
    
    size_t getMemoryUsage() const {
        return pool_.memoryUsage();
    }
    
    // Get specialized pool managers
    NeuronPoolManager& getNeuronPoolManager() {
        return neuronPoolManager_;
    }
    
    SynapsePoolManager& getSynapsePoolManager() {
        return synapsePoolManager_;
    }

private:
    NeuralRegionMemoryPool() = default;
    ~NeuralRegionMemoryPool() = default;
    
    // Disable copying
    NeuralRegionMemoryPool(const NeuralRegionMemoryPool&) = delete;
    NeuralRegionMemoryPool& operator=(const NeuralRegionMemoryPool&) = delete;
    
    MemoryPool<NeuralRegion> pool_;
    NeuronPoolManager neuronPoolManager_;
    SynapsePoolManager synapsePoolManager_;
};

} // namespace nlm
