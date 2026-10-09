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
    
    // Advanced connectivity patterns
    enum class ConnectivityPattern {
        Random,         // Random connectivity
        Local,          // Local (nearby) connectivity
        Clustered,      // Grouped into clusters
        Modular,        // Modules with inter-module connections
        Spiral,         // Spiral connectivity pattern
        DistanceBased,   // Connectivity decays with distance
        ActivityBased   // Hebbian-like connectivity based on activity
    };
    
    // Set connectivity pattern
    void setConnectivityPattern(ConnectivityPattern pattern);
    ConnectivityPattern getConnectivityPattern() const;
    
    // Configure connectivity parameters
    void configureConnectivity(float connectionProbability,
                               float meanWeight,
                               float weightVariance,
                               float localConnectionDistance = 1.0f);
    
    // Pattern-specific configuration
    void configureLocalConnectivity(float localConnectionDistance);
    void configureClusteredConnectivity(size_t clusterSize,
                                         float interClusterWeight = 0.1f,
                                         float intraClusterWeight = 1.0f);
    void configureSpiralConnectivity(float spiralRadius, float spiralPitch);
    
    // Activity-based connectivity (Hebbian)
    void enableActivityBasedConnectivity(float learningRate = 0.001f,
                                         float weightDecay = 0.999f);
    
    // Region-specific configuration
    void setRegionalBias(const std::string& biasType); // "cortical", "cerebellar", "hippocampal", etc.
    std::string getRegionalBias() const;
    
    // Regional evolution (structural plasticity)
    void enableRegionalEvolution(float synaptogenesisRate = 0.001f,
                                 float pruningRate = 0.0001f,
                                 float evolutionPressure = 1.0f);
    
    // Regional homeostasis
    void enableHomeostasis(float targetFiringRate = 10.0f,
                           float adaptationRate = 0.01f);
    
    // Regional statistics and analysis
    float getRegionalCohesion() const;        // Measure of how connected the region is internally
    float getRegionalIntegration() const;      // Measure of information flow within region
    float getRegionalEfficiency() const;       // Measure of network efficiency
    
    // Regional development and maturation
    void enableDevelopment(bool enable = true);
    bool isDevelopmentEnabled() const;
    void setDevelopmentStage(float stage);     // 0.0 = embryonic, 1.0 = mature
    float getDevelopmentStage() const;
    
    // Regional monitoring
    struct RegionMetrics {
        size_t neuronCount = 0;
        size_t activeNeuronCount = 0;
        float averageFiringRate = 0.0f;
        float synapticStrength = 0.0f;
        float regionalCohesion = 0.0f;
        float integrationIndex = 0.0f;
        float efficiencyScore = 0.0f;
        float connectivityDensity = 0.0f;
        Timestamp lastDevelopmentTime = 0;
        size_t developmentIterations = 0;
    };
    
    RegionMetrics getMetrics() const;
    void resetMetrics();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
