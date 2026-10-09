#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// PLACEHOLDER - Phase 2 will implement region-level dynamics

class NeuralRegion {
public:
    // Create region with ID
    explicit NeuralRegion(RegionId id);
    
    // Create region with ID and name
    NeuralRegion(RegionId id, const std::string& name);
    
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
    
    // Enhanced neuron type management
    void createNeuronTypes(NeuronType type, size_t populationSize);
    void balanceNeuronTypes();  // Adjust population sizes to match brain composition
    NeuronType getDominantNeuronType() const;
    float getNeuronTypeRatio(NeuronType type) const;
    void setNeuronTypeRatio(NeuronType type, float ratio);
    
    // Statistical queries for advanced dynamics
    float computeTypeSpecificFiringRates(NeuronType type) const;
    float computeTypeSpecificRefractoryPeriods(NeuronType type) const;
    float computeTypeSpecificAdaptationStrengths(NeuronType type) const;
    
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
    
// Step all synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    // === BATCH OPERATIONS ===
    
    // Batch weight operations
    void batchSetSynapseWeights(const std::vector<SynapseId>& synapseIds,
                               const std::vector<SynapticWeight>& weights);
    void batchAddToSynapseWeights(const std::vector<SynapseId>& synapseIds,
                                  const std::vector<SynapticWeight>& deltas);
    
    // Batch spike recording
    void batchRecordPreSpikes(const std::vector<SynapseId>& synapseIds,
                             const std::vector<Timestamp>& timestamps);
    void batchRecordPostSpikes(const std::vector<SynapseId>& synapseIds,
                              const std::vector<Timestamp>& timestamps);
    
    // Batch synaptic current computation
    void batchComputeSynapticCurrents(const std::vector<SynapseId>& synapseIds,
                                     const std::vector<NeuronId>& preNeuronIds,
                                     const std::vector<float>& preFiringRates,
                                     std::vector<MembranePotential>& currents);
    
    // Batch plasticity operations
    void batchEnablePlasticity(const std::vector<SynapseId>& synapseIds,
                              bool hebbian, bool stdp, bool rewardModulated);
    void batchSetEligibilityTraces(const std::vector<SynapseId>& synapseIds,
                                   const std::vector<float>& traces);
    void batchDecayEligibilityTraces(const std::vector<SynapseId>& synapseIds,
                                     const std::vector<float>& decayRates);
    
    // Batch spike history management
    void batchOptimizeSpikeHistory(const std::vector<SynapseId>& synapseIds);
    void batchClearSpikeHistory(const std::vector<SynapseId>& synapseIds);
    
    // === LIFECYCLE MANAGEMENT ===
    
    // Synapse factory methods for batch creation
    std::vector<SynapseId> createBatchSynapses(const std::vector<std::pair<NeuronId, NeuronId>>& connections,
                                              SynapseType type = SynapseType::Excitatory,
                                              SynapticWeight baseWeight = 0.1f);
    void batchInitializeSynapses(const std::vector<SynapseId>& synapseIds,
                                class RandomGenerator& rng);
    void batchDeleteSynapses(const std::vector<SynapseId>& synapseIds);
    
    // === STATE QUERY METHODS ===
    
    // Batch weight queries
    void batchGetSynapseWeights(const std::vector<SynapseId>& synapseIds,
                               std::vector<SynapticWeight>& weights) const;
    float batchComputeAverageSynapseWeight(const std::vector<SynapseId>& synapseIds) const;
    
    // Batch statistics
    float batchComputeSynapseExcitationInhibitionRatio(const std::vector<SynapseId>& synapseIds) const;
    float batchComputeSynapseWeightVariance(const std::vector<SynapseId>& synapseIds) const;
    float batchComputeSynapseWeightSkewness(const std::vector<SynapseId>& synapseIds) const;
    
    // Synapse neighborhood operations
    std::vector<SynapseId> batchGetSynapseNeighborhood(SynapseId centerSynapseId,
                                                     float maxDistance) const;
    std::vector<SynapseId> batchGetSynapsesByType(SynapseType type) const;
    std::vector<SynapseId> batchGetSynapsesInWeightRange(SynapticWeight minWeight,
                                                        SynapticWeight maxWeight) const;
    
    // === DEPENDENCY TRACKING ===
    
    // Neuron dependency tracking
    std::vector<SynapseId> batchGetSynapsesFromNeuron(NeuronId neuronId) const;
    std::vector<SynapseId> batchGetSynapsesToNeuron(NeuronId neuronId) const;
    std::vector<NeuronId> batchGetUniqueSourceNeurons(const std::vector<SynapseId>& synapseIds) const;
    std::vector<NeuronId> batchGetUniqueDestinationNeurons(const std::vector<SynapseId>& synapseIds) const;
    
    // === STATISTICS ===
    
    // Region-wide statistics
    float computeRegionalConnectivityDensity() const;
    float computeRegionalAverageFiringRate() const;
    float computeRegionalAverageSynapticWeight() const;
    
    // Get all synapses efficiently
    std::vector<Synapse*> getAllSynapses() const;
    std::vector<const Synapse*> getAllSynapsesPtr() const;
    
private:
    // Helper methods for efficient lookups
    Synapse* getSynapseById(SynapseId id) const;
    
    // Optimized synapse operations
    void addSynapseInternal(SynapseId id, NeuronId source, NeuronId destination,
                           SynapticWeight weight, Delay delay);
    
    // Helper for efficient synapse iteration
    std::vector<SynapseId> getAllSynapseIds() const;
    
    // Thread safety for batch operations
    mutable std::mutex batchOperationMutex_;
    
private:
    struct Impl;
    Impl* pImpl;
};
