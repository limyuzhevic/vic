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

/**
 * @brief Represents a neural processing region in a computational brain model
 * 
 * A NeuralRegion is a computational abstraction of a brain region that contains
 * multiple neural populations, manages local connectivity patterns, and provides
 * high-level interfaces for network simulation and analysis. Regions are the
 * primary organizational units in hierarchical neural network models, allowing
 * for modular design and efficient parallel processing.
 * 
 * The region concept supports:
 * - Population management (add/remove neurons with specific properties)
 * - Local connectivity patterns (intra-region synaptic connections)
 * - Network statistics and analysis
 * - Coordinated simulation across multiple populations
 * 
 * @note This is a placeholder implementation that will be expanded in Phase 2
 *       to include region-level dynamics and more sophisticated connectivity patterns.
 */
class NeuralRegion {
public:
    /**
     * @brief Constructs a neural region with a unique identifier
     * 
     * @param id Unique identifier for this region within the network
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Constructs a neural region with identifier and name
     * 
     * @param id Unique identifier for this region
     * @param name Human-readable name for this region
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destroys the region and cleans up resources
     */
    ~NeuralRegion();
    
    /**
     * @brief Copy constructor is deleted (regions cannot be copied)
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted (regions cannot be assigned)
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor (efficient transfer of ownership)
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient transfer of ownership)
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Retrieves the region's unique identifier
     * 
     * @return The RegionId assigned during construction
     */
    RegionId getId() const;
    
    /**
     * @brief Retrieves the region's human-readable name
     * 
     * @return Constant reference to the region's name string
     */
    const std::string& getName() const;
    
    /**
     * @brief Sets the region's name
     * 
     * @param name New human-readable name for the region
     */
    void setName(const std::string& name);
    
    /**
     * @brief Adds a new neural population to this region
     * 
     * @param size Number of neurons to create in the population
     * @param type Type of neurons (Internal, External, Special)
     * 
     * @return The PopulationId of the newly created population
     * 
     * @note The population is owned by this region and will be cleaned up
     *       when the region is destroyed. Each population is assigned a unique
     *       sequential identifier.
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Retrieves a specific population from this region
     * 
     * @param id The PopulationId of the population to retrieve
     * 
     * @return Pointer to the NeuralPopulation, or nullptr if not found
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Retrieves a specific population from this region (read-only)
     * 
     * @param id The PopulationId of the population to retrieve
     * 
     * @return Constant pointer to the NeuralPopulation, or nullptr if not found
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Retrieves the number of populations in this region
     * 
     * @return Number of populations currently in the region
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Retrieves all populations in this region (read-only)
     * 
     * @return Vector of unique pointers to all populations in the region
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Retrieves all populations in this region (mutable)
     * 
     * @return Vector of raw pointers to all populations in the region
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Adds a local synapse within this region
     * 
     * @param source NeuronId of the presynaptic neuron
     * @param destination NeuronId of the postsynaptic neuron
     * @param weight Synaptic weight (default: 0.0f)
     * @param delay Synaptic transmission delay in simulation steps (default: 1)
     * 
     * @return The SynapseId of the newly created synapse
     * 
     * @note This creates local connectivity within the region only.
     *       For cross-region connections, see Phase 2 implementation.
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Retrieves a specific synapse from this region
     * 
     * @param id The SynapseId of the synapse to retrieve
     * 
     * @return Pointer to the Synapse, or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Retrieves a specific synapse from this region (read-only)
     * 
     * @param id The SynapseId of the synapse to retrieve
     * 
     * @return Constant pointer to the Synapse, or nullptr if not found
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Retrieves the number of synapses in this region
     * 
     * @return Number of synapses currently in the region
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Retrieves all synapses in this region (read-only)
     * 
     * @return Vector of unique pointers to all synapses in the region
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Retrieves all synapses originating from a specific neuron
     * 
     * @param neuron NeuronId of the source neuron
     * 
     * @return Vector of pointers to all outgoing synapses from this neuron
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Retrieves all synapses terminating at a specific neuron
     * 
     * @param neuron NeuronId of the target neuron
     * 
     * @return Vector of pointers to all incoming synapses to this neuron
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Retrieves the total number of neurons across all populations
     * 
     * @return Total neuron count in the region
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Retrieves the number of neurons that are currently active
     * 
     * @return Number of neurons with firing rate > 0
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Retrieves the number of neurons that are currently firing
     * 
     * @return Number of neurons in firing state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Retrieves the average firing rate across all neurons
     * 
     * @return Average firing rate in Hertz (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Retrieves the average synaptic weight
     * 
     * @return Average synaptic weight across all synapses
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Retrieves the synaptic density
     * 
     * @return Fraction of possible neuron-to-neuron connections that are present
     *         (0.0 to 1.0, where 1.0 means all-to-all connectivity)
     */
    float getSynapticDensity() const;
    
    /**
     * @brief Advances all neurons and synapses through one simulation step
     * 
     * This is the main simulation method for the region, coordinating updates
     * across all populations and synaptic connections. Each neuron will update
     * its membrane potential, check for spikes, and update synaptic states.
     * 
     * @param currentTime Current simulation time in steps
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets all neurons and synapses to their initial states
     */
    void reset();
    
    /**
     * @brief Initializes random connectivity patterns
     * 
     * This method creates random synaptic connections between neurons within
     * the region, following a probabilistic connectivity model.
     * 
     * @param rng Reference to random number generator
     * @param connectionProbability Probability that any given pair of neurons
     *                               will be connected (0.0 to 1.0)
     * @param meanWeight Mean synaptic weight for generated connections
     * @param weightVariance Variance of synaptic weights for generated connections
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    /**
     * @brief Retrieves all neurons from all populations in the region
     * 
     * @return Vector of pointers to all neurons in the region
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Retrieves all neurons from all populations in the region (read-only)
     * 
     * @return Vector of constant pointers to all neurons in the region
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    /**
     * @brief Opaque implementation structure for the pImpl idiom
     */
    struct Impl;
    
    /**
     * @brief Pointer to the implementation structure
     */
    Impl* pImpl;
};

} // namespace nlm
