# NeuralRegion.hpp - Comprehensive documentation for NeuralRegion class

#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * @class NeuralRegion
 * @brief A brain region containing multiple populations and local connectivity
 * 
 * NeuralRegion represents a functional brain region that contains multiple neural
 * populations and implements local connectivity through synapses. This abstraction
 * allows for the simulation of different brain regions with distinct computational
 * properties, such as cortical areas, subcortical structures, or specialized
 * processing modules.
 * 
 * Regions are the primary organizational unit in the NLM architecture, providing:
 * - compartmentalization of neural computation
 * - local connectivity patterns that mimic biological brain structure
 * - independent management of neural populations
 * - region-specific dynamics and plasticity rules
 * 
 * Each region can contain multiple populations of different neuron types (sensory,
 * motor, internal) and implements both feedforward and recurrent connectivity
 * through synaptic connections between neurons within the same region.
 * 
 * @note Currently implementing Phase 2: Real Neural Computation
 *       This class is under active development with additional features planned.
 * @note Regions provide the foundation for scaling neural simulations from small
 *       networks to large, structurally realistic brain models.
 */
class NeuralRegion {
public:
    /**
     * @brief Create a new neural region with the specified ID
     * 
     * @param id Unique identifier for this region
     * @throw std::invalid_argument if id is invalid
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Create a new neural region with the specified ID and name
     * 
     * @param id Unique identifier for this region
     * @param name Human-readable name for this region (useful for debugging and analysis)
     * @throw std::invalid_argument if id is invalid
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor
     * 
     * Properly cleans up internal resources including populations and synapses,
     * ensuring no memory leaks or dangling references.
     */
    ~NeuralRegion();
    
    // === Core Operations ===
    
    /**
     * @brief Disable copy constructor (neural regions cannot be copied)
     * 
     * Neural regions manage complex internal state including populations,
     * synapses, and connectivity graphs. Copying would create complex issues
     * with shared state and resource management. Use move semantics instead.
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Disable copy assignment operator
     * 
     * See copy constructor note above.
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of all internal components (populations and synapses)
     * from another region. The source region is left in a valid but empty state.
     * 
     * @param other The region to move from
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership of all internal components from another region.
     * The source region is left in a valid but empty state.
     * 
     * @param other The region to move from
     * @return Reference to this region (for chaining)
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // === Identity and Basic Properties ===
    
    /**
     * @brief Get the unique identifier of this region
     * 
     * @return RegionId Unique identifier for this region
     */
    RegionId getId() const;
    
    /**
     * @brief Get the human-readable name of this region
     * 
     * @return const std::string& Name of this region
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the human-readable name of this region
     * 
     * @param name New name for this region
     */
    void setName(const std::string& name);
    
    // === Population Management ===
    
    /**
     * @brief Add a new neural population to this region
     * 
     * Creates a new population with the specified number of neurons and type,
     * and adds it to this region's collection of populations. Each population
     * maintains its own characteristics and dynamics.
     * 
     * @param size Number of neurons to create in the new population
     * @param type Type of neurons in the population (Sensory, Motor, Internal, etc.)
     * @return PopulationId Unique identifier for the newly created population
     * @throw std::invalid_argument if size is zero or negative
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a pointer to a population by its ID
     * 
     * @param id Unique identifier of the population to retrieve
     * @return NeuralPopulation* Pointer to the population, or nullptr if not found
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get a const pointer to a population by its ID
     * 
     * @param id Unique identifier of the population to retrieve
     * @return const NeuralPopulation* Const pointer to the population, or nullptr if not found
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get the number of populations in this region
     * 
     * @return size_t Number of populations in this region
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in this region
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of all population pointers
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all populations as mutable pointers
     * 
     * This is useful for operations that need to modify population properties
     * or access neurons directly within populations.
     * 
     * @return std::vector<NeuralPopulation*> Vector of all population pointers as mutable
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // === Synapse Management ===
    
    /**
     * @brief Add a new synapse to this region
     * 
     * Creates a synaptic connection between two neurons within this region,
     * implementing local connectivity. Synapses represent the physical
     * connections through which neurons communicate.
     * 
     * @param source NeuronId of the pre-synaptic neuron (sender)
     * @param destination NeuronId of the post-synaptic neuron (receiver)
     * @param weight Initial synaptic weight (default: 0.0)
     * @param delay Synaptic transmission delay in simulation steps (default: 1)
     * @return SynapseId Unique identifier for the newly created synapse
     * @throw std::invalid_argument if source or destination neurons don't exist
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                        SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a pointer to a synapse by its ID
     * 
     * @param id Unique identifier of the synapse to retrieve
     * @return Synapse* Pointer to the synapse, or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get a const pointer to a synapse by its ID
     * 
     * @param id Unique identifier of the synapse to retrieve
     * @return const Synapse* Const pointer to the synapse, or nullptr if not found
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get the number of synapses in this region
     * 
     * @return size_t Number of synapses in this region
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in this region
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Vector of all synapse pointers
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // === Connectivity Queries ===
    
    /**
     * @brief Get all synapses originating from a specific neuron
     * 
     * This retrieves all outgoing connections from the specified neuron,
     * useful for analyzing its influence and integration within the region.
     * 
     * @param neuron NeuronId of the pre-synaptic neuron
     * @return std::vector<Synapse*> Vector of all outgoing synapses
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses terminating at a specific neuron
     * 
     * This retrieves all incoming connections to the specified neuron,
     * useful for analyzing its input integration and receptive field.
     * 
     * @param neuron NeuronId of the post-synaptic neuron
     * @return std::vector<Synapse*> Vector of all incoming synapses
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // === Statistics and Analysis ===
    
    /**
     * @brief Get the total number of neurons in this region
     * 
     * This includes all neurons across all populations in the region.
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the number of active neurons in this region
     * 
     * Active neurons are those that are currently firing or have non-zero firing rates.
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of currently firing neurons in this region
     * 
     * Firing neurons are those that have just crossed their firing threshold
     * during the current simulation step.
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this region
     * 
     * @return float Average firing rate in Hz, or 0.0 if no neurons present
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the average synaptic weight across all synapses in this region
     * 
     * This provides a measure of the overall strength of connectivity within
     * the region, which can influence network dynamics and stability.
     * 
     * @return float Average synaptic weight, or 0.0 if no synapses
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the synaptic density of this region
     * 
     * This represents the fraction of possible connections that are actually
     * present, providing a measure of the region's connectivity pattern.
     * A value of 1.0 indicates a fully connected network.
     * 
     * @return float Synaptic density (0.0 to 1.0), or 0.0 if no neurons
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    // === Simulation Operations ===
    
    /**
     * @brief Advance all neurons and synapses in this region by one simulation step
     * 
     * This method propagates all local dynamics within the region, including:
     * - LIF dynamics for all neurons
     * - Synaptic transmission and plasticity
     * - Local recurrent connectivity effects
     * - Population-level statistics updates
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses in this region to their initial state
     * 
     * This clears all spiking activity, resets membrane potentials, and prepares
     * the region for a new simulation cycle. All neurons and synapses are returned
     * to their default initialized states.
     */
    void reset();
    
    // === Connectivity Initialization ===
    
    /**
     * @brief Initialize local connectivity patterns within this region
     * 
     * This method creates random synaptic connections between neurons within
     * the region, implementing Hebbian and STDP principles. The resulting
     * connectivity pattern determines how neurons interact within the region.
     * 
     * @param rng Reference to random number generator for reproducible connectivity
     * @param connectionProbability Probability that any two neurons are connected
     * @param meanWeight Mean synaptic weight for new connections
     * @param weightVariance Variance of synaptic weights for new connections
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // === Neuron Access ===
    
    /**
     * @brief Get all neurons across all populations in this region
     * 
     * This provides a flattened view of all neurons in the region, useful for
     * operations that need to access neurons without considering their population structure.
     * 
     * @return std::vector<Neuron*> Vector of all neuron pointers in this region
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations in this region (const version)
     * 
     * @return std::vector<const Neuron*> Vector of all const neuron pointers in this region
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    /**
     * @brief Internal implementation structure (pImpl pattern)
     * 
     * This uses the pImpl idiom to hide implementation details from the header file,
     * reducing compilation dependencies and improving encapsulation. The internal
     * structure manages populations, synapses, and connectivity mappings.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Managed pointer to the internal implementation structure that contains
     * the actual data and method implementations for region-level operations.
     */
    Impl* pImpl;
};

} // namespace nlm
