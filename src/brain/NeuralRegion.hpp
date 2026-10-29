#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
//
// Neural regions are functional units of the brain that contain multiple neural
// populations and establish local connectivity through synapses. Regions serve as
// the intermediate level between individual neurons and the global brain, allowing
// for:
// - Modular organization of neural computation
// - Local connectivity and communication
// - Regional specialization (e.g., sensory, motor, association areas)
// - Efficient spike propagation through structured connectivity
// - Regional plasticity and development
//
// Each region contains:
// - Multiple neural populations (groups of neurons with similar functions)
// - Local synapses connecting neurons within the region
// - Statistical and connectivity management
// - Random initialization and development
//
// Regions are organized hierarchically in the global brain, with inter-region
// connections providing long-range communication and integration.

class NeuralRegion {
public:
    // Create region with ID
    // 
    // Args:
    //   id: Unique identifier for the region
    explicit NeuralRegion(RegionId id);
    
    // Create region with ID and name
    // 
    // Args:
    //   id: Unique identifier for the region
    //   name: Human-readable name for the region
    NeuralRegion(RegionId id, const std::string& name);
    
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    // 
    // Gets the region's unique identifier.
    // 
    // Returns:
    //   Region ID
    RegionId getId() const;
    
    // Gets the region's human-readable name.
    // 
    // Returns:
    //   Region name
    const std::string& getName() const;
    
    // Sets the region's name.
    // 
    // Args:
    //   name: New name for the region
    void setName(const std::string& name);
    
    // Population management
    // 
    // Manages neural populations within the region. Populations are groups of
    // neurons with similar functions (e.g., sensory neurons, motor neurons,
    // internal neurons).
    // 
    // Args:
    //   size: Number of neurons in the new population
    //   type: Type of neurons (Sensory, Motor, Internal)
    // 
    // Returns:
    //   Population ID of the newly created population
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    // Gets a neural population by ID.
    // 
    // Args:
    //   id: Population ID to retrieve
    // 
    // Returns:
    //   Pointer to the population (may be null if not found)
    NeuralPopulation* getPopulation(PopulationId id);
    
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    // Gets the number of populations in the region.
    // 
    // Returns:
    //   Population count
    size_t getPopulationCount() const;
    
    // Gets all populations in the region.
    // 
    // Returns:
    //   Constant reference to vector of populations
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    // Gets all populations in the region (non-constant version).
    // 
    // Returns:
    //   Vector of population pointers
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    // 
    // Manages synaptic connections between neurons within the region.
    // This implements local connectivity and short-range communication.
    // 
    // Args:
    //   source: Source neuron ID
    //   destination: Target neuron ID
    //   weight: Synaptic weight (default: 0.0f)
    //   delay: Transmission delay in steps (default: 1)
    // 
    // Returns:
    //   Synapse ID of the newly created synapse
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    // Gets a synapse by ID.
    // 
    // Args:
    //   id: Synapse ID to retrieve
    // 
    // Returns:
    //   Pointer to the synapse (may be null if not found)
    Synapse* getSynapse(SynapseId id);
    
    const Synapse* getSynapse(SynapseId id) const;
    
    // Gets the number of synapses in the region.
    // 
    // Returns:
    //   Synapse count
    size_t getSynapseCount() const;
    
    // Gets all synapses in the region.
    // 
    // Returns:
    //   Constant reference to vector of synapses
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    // 
    // Provides access to synaptic connectivity patterns.
    // 
    // Args:
    //   neuron: Neuron ID to query
    // 
    // Returns:
    //   Vector of synapses originating from the neuron
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    // 
    // Provides statistical information about the region's state.
    // 
    // Returns:
    //   Various statistics about the region's neural activity
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    // 
    // Provides statistics about synaptic connectivity patterns.
    // 
    // Returns:
    //   Average synaptic weight in the region
    float getAverageSynapticWeight() const;
    
    // Gets the density of synaptic connections.
    // 
    // Returns:
    //   Fraction of possible connections that are present (0.0 to 1.0)
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    // 
    // Advances the region's neural computation by one time step.
    // Updates all neurons and synapses in the region.
    // 
    // Args:
    //   currentTime: Current simulation time (seconds)
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    // 
    // Resets all neural state variables in the region to initial values.
    // Useful for starting a new episode or experiment.
    void reset();
    
    // Initialize connectivity
    // 
    // Initializes random synaptic connectivity within the region.
    // Establishes the initial network structure.
    // 
    // Args:
    //   rng: Random number generator
    //   connectionProbability: Probability of connection between neurons
    //   meanWeight: Mean synaptic weight for new connections
    //   weightVariance: Variance of synaptic weights
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    // Get all neurons across all populations
    // 
    // Gets all neurons in the region, regardless of population.
    // 
    // Returns:
    //   Vector of all neuron pointers
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
