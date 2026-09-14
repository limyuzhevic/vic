#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// PHASE 6: Implement region-level dynamics with real neural computation

class NeuralRegion {
public:
    /**
     * \brief Constructor - creates a brain region with a unique identifier
     * \details Creates a new neural region with the specified ID. The region can contain
     *          multiple neuron populations and local synaptic connections within the brain.
     * \param id Unique identifier for the neural region
     * \note Each region is responsible for maintaining its own population of neurons,
     *       synapses, and local connectivity patterns. Regions can communicate through
     *       inter-regional connections.
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * \brief Constructor - creates a named brain region with a unique identifier
     * \details Creates a new neural region with the specified ID and name. The name is
     *          useful for identifying regions in multi-region brain architectures.
     * \param id Unique identifier for the neural region
     * \param name Human-readable name for the neural region
     * \note The name can be changed later using setName(). For multi-region brains,
     *       region names should be unique to avoid confusion.
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * \brief Destructor - cleans up region resources
     * \details Properly cleans up the unique_ptr holding the implementation details.
     *          Ensures cleanup of all neurons, populations, and synapses in the region.
     */
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * \brief Get the unique identifier of the neural region
     * \return RegionId The unique identifier for this region
     * \note This ID is immutable and used to reference the region across the brain system.
     */
    RegionId getId() const;
    
    /**
     * \brief Get the name of the neural region
     * \return const std::string& The human-readable name of this region
     * \note Region names are useful for debugging and system visualization.
     */
    const std::string& getName() const;
    
    /**
     * \brief Set the name of the neural region
     * \param name New name for the neural region
     * \note This operation is thread-safe. Region names are not used for uniqueness
     *       checks - regions can have the same name.
     */
    void setName(const std::string& name);
    
    /**
     * \brief Add a new neuron population to the region
     * \details Creates a new population containing neurons of the specified type and size.
     *          Each population is assigned a unique PopulationId within this region.
     * \param size Number of neurons in the new population
     * \param type Type of neurons in the population (default: Internal)
     * \return PopulationId Unique identifier for the newly created population
     * \note Populations maintain neurons with similar characteristics and types.
     *       Available neuron types: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal.
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * \brief Get a population by ID (non-const version)
     * \details Retrieves the neural population with the specified ID from this region.
     *          Returns nullptr if the population doesn't exist.
     * \param id Unique identifier of the population to retrieve
     * \return NeuralPopulation* Pointer to the population if found, nullptr otherwise
     * \note This method provides direct access to population internals. For thread-safe
     *       access, consider using const versions or locking mechanisms.
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * \brief Get a population by ID (const version)
     * \details Retrieves the neural population with the specified ID from this region.
     *          Returns nullptr if the population doesn't exist.
     * \param id Unique identifier of the population to retrieve
     * \return const NeuralPopulation* Pointer to the population if found, nullptr otherwise
     * \note This is the thread-safe version for reading population information.
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * \brief Get the number of populations in the region
     * \return size_t Number of populations currently in this region
     * \note This includes all populations regardless of neuron type or size.
     */
    size_t getPopulationCount() const;
    
    /**
     * \brief Get all populations in the region (const version)
     * \return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of all populations
     * \note This is the thread-safe version for accessing population information.
     *       The vector contains unique_ptrs, so ownership is retained by the region.
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * \brief Get all populations in the region (non-const version)
     * \return std::vector<NeuralPopulation*> Vector of population pointers
     * \note This method provides direct access to population pointers for modification.
     *       For thread-safe access, consider using the const version or locking.
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * \brief Add a synaptic connection between two neurons within this region
     * \details Creates a new synapse connecting a source neuron to a destination neuron.
     *          Synapses represent the functional connections that enable neural communication.
     * \param source NeuronId of the presynaptic neuron (sender)
     * \param destination NeuronId of the postsynaptic neuron (receiver)
     * \param weight Initial synaptic weight (default: 0.0f)
     * \param delay Transmission delay in milliseconds (default: 1)
     * \return SynapseId Unique identifier for the newly created synapse
     * \note This creates a local connection within the region. For long-range connections,
     *       use inter-regional connection management at the Brain level.
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * \brief Get a synapse by ID (non-const version)
     * \details Retrieves the synapse with the specified ID from this region.
     *          Returns nullptr if the synapse doesn't exist.
     * \param id Unique identifier of the synapse to retrieve
     * \return Synapse* Pointer to the synapse if found, nullptr otherwise
     * \note This method provides direct access to synapse internals.
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * \brief Get a synapse by ID (const version)
     * \details Retrieves the synapse with the specified ID from this region.
     *          Returns nullptr if the synapse doesn't exist.
     * \param id Unique identifier of the synapse to retrieve
     * \return const Synapse* Pointer to the synapse if found, nullptr otherwise
     * \note This is the thread-safe version for reading synapse information.
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * \brief Get the number of synapses in the region
     * \return size_t Number of synaptic connections in this region
     * \note This includes all synapses regardless of type or weight.
     */
    size_t getSynapseCount() const;
    
    /**
     * \brief Get all synapses in the region (const version)
     * \return const std::vector<std::unique_ptr<Synapse>>& Vector of all synapses
     * \note This is the thread-safe version for accessing synapse information.
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * \brief Get all synapses originating from a specific neuron
     * \details Retrieves all outgoing synapses from the specified neuron.
     * \param neuron NeuronId of the presynaptic neuron
     * \return std::vector<Synapse*> Vector of synapse pointers originating from the neuron
     * \note Returns empty vector if neuron doesn't exist or has no outgoing synapses.
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * \brief Get all synapses terminating at a specific neuron
     * \details Retrieves all incoming synapses to the specified neuron.
     * \param neuron NeuronId of the postsynaptic neuron
     * \return std::vector<Synapse*> Vector of synapse pointers terminating at the neuron
     * \note Returns empty vector if neuron doesn't exist or has no incoming synapses.
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * \brief Get the total number of neurons in the region
     * \return size_t Total neuron count across all populations
     * \note This is the sum of all neurons in all populations within this region.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * \brief Get the number of active neurons in the region
     * \return size_t Number of neurons currently in firing state
     * \note Active neurons are those that have recently crossed their firing threshold.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * \brief Get the number of firing neurons in the region
     * \return size_t Number of neurons currently emitting action potentials
     * \note Firing neurons are those in the process of spike generation.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * \brief Get the average firing rate across all neurons in the region
     * \return float Average firing rate in Hertz (spikes per second)
     * \note This is computed from the activity of all neurons in the region.
     */
    float getAverageFiringRate() const;
    
    /**
     * \brief Get the average synaptic weight in the region
     * \return float Average weight of all synapses in the region
     * \note This provides insight into the overall strength of connectivity.
     */
    float getAverageSynapticWeight() const;
    
    /**
     * \brief Get the synaptic density of the region
     * \return float Fraction of possible connections that are actually present
     * \note Density = (actual synapses) / (total possible neuron pairs).
     *       Values range from 0.0 (no connections) to 1.0 (fully connected).
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * \brief Advance the region state by one simulation timestep
     * \details Performs all neural computations for this region including:
     *         - LIF neuron dynamics and spike generation
     *         - Synaptic transmission and integration
     *         - Plasticity rule updates
     *         - Statistics collection
     * \param currentTime Current simulation timestamp
     * \note This is a core simulation function called by the Brain class.
     *       Time-stepping must be synchronized with other brain components.
     */
    void step(Timestamp currentTime);
    
    /**
     * \brief Reset all neurons and synapses to initial state
     * \details Resets all neurons to resting potential and clears synaptic states.
     *          This provides a clean state for starting new simulation runs.
     * \note This does not reset the region's structural configuration (populations,
     *       synapses, or neuron counts). Only the dynamic state is reset.
     */
    void reset();
    
    /**
     * \brief Initialize random connectivity between neurons
     * \details Creates random synaptic connections based on neuron types and probabilities.
     *          This is typically called during region initialization.
     * \param rng Random number generator for probabilistic connectivity
     * \param connectionProbability Probability that a potential connection exists
     * \param meanWeight Mean initial synaptic weight
     * \param weightVariance Standard deviation of initial synaptic weights
     * \note This creates local connections within the region. Inter-regional connections
     *       are managed separately at the Brain level.
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    /**
     * \brief Get all neurons across all populations (non-const version)
     * \return std::vector<Neuron*> Vector of all neuron pointers in the region
     * \note This provides direct access to all neurons for modification.
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * \brief Get all neurons across all populations (const version)
     * \return std::vector<const Neuron*> Vector of all neuron const pointers in the region
     * \note This is the thread-safe version for reading neuron information.
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
