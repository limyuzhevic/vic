#pragma once
 
#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include "OptimizedConnectivity.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>
 
namespace nlm {
 
// NeuralRegion: A brain region containing multiple populations and local connectivity
// Optimized to use CSR format for efficient connectivity management
 
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
    
    // Optimized synapse management using CSR format
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    
    // Optimized connectivity queries using CSR
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Get connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;
    
    // Get internal connectivity structure
    CSRConnectivity& getConnectivity() { return connectivity_; }
    const CSRConnectivity& getConnectivity() const { return connectivity_; }
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Step all populations and synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity with optimization
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
    // Get connectivity statistics
    CSRConnectivity::Stats getConnectivityStats() const;
    
    // Check if connectivity is optimized
    bool isConnectivityOptimized() const { return useOptimizedConnectivity_; }
    
    // Enable/disable optimized connectivity
    void enableOptimizedConnectivity(bool enable) { useOptimizedConnectivity_ = enable; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Optimized connectivity structure
    CSRConnectivity connectivity_;
    NeuronIndexer neuronIndexer_;
    
    // Flag to enable optimized connectivity
    bool useOptimizedConnectivity_;
    
    // Cache for frequently accessed connectivity
    ConnectivityCache connectivityCache_;
};
 
} // namespace nlm
