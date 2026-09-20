#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <cmath>

namespace nlm {

/**
 * @class NeuralRegion
 * @brief A functional brain region containing multiple populations and local connectivity
 * 
 * NeuralRegion represents a modular unit of brain computation, analogous to a brain region
 * in biological systems. Each region contains multiple neural populations organized by
 * neuron type (sensory, motor, internal) and maintains local synaptic connectivity
 * through defined pathways. Regions can process information in parallel and communicate
 * through inter-regional connections managed by the central Brain class.
 * 
 * The region provides a complete computational unit with its own:
 * - Population management (creation, access, statistics)
 * - Local synaptic connectivity
 * - Regional statistics (neuron count, activity levels)
 * - Connectivity metrics (synaptic density, average weights)
 * 
 * @note Currently in Phase 2 implementation - region-level dynamics are being developed
 * as part of the integration of all brain systems in Phase 6.
 * 
 * @see NeuralPopulation Individual neuron groups within this region
 * @see Brain Central coordinator that manages multiple regions
 */
class NeuralRegion {
public:
    /**
     * @brief Construct a new NeuralRegion with specified ID only
     * 
     * Creates a minimal region with default settings. Use the name-based constructor
     * for regions with descriptive identifiers.
     * 
     * @param id Unique identifier for this region (typically 1-based)
     */
    explicit NeuralRegion(RegionId id);

    /**
     * @brief Construct a new NeuralRegion with ID and name
     * 
     * Creates a fully initialized region with both identifier and descriptive name.
     * The name is useful for debugging, visualization, and region identification.
     * 
     * @param id Unique identifier for this region
     * @param name Human-readable name for the region (default: empty string)
     */
    NeuralRegion(RegionId id, const std::string& name);

    /**
     * @brief Destructor
     * 
     * Properly cleans up all contained populations and synapses, ensuring no memory leaks.
     * The pImpl pattern ensures complete cleanup of all internal resources.
     */
    ~NeuralRegion();

    // Rule of five: delete copy operations, enable move operations
    /**
     * @brief Deleted copy constructor
     * 
     * Regions cannot be copied due to complex internal structures and pImpl pattern.
     * Use move operations instead for transferring region ownership.
     */
    NeuralRegion(const NeuralRegion&) = delete;

    /**
     * @brief Deleted copy assignment operator
     * 
     * See copy constructor for details.
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;

    /**
     * @brief Move constructor
     * 
     * Transfers ownership of all contained populations and synapses from another region.
     * The source region is left in a valid but empty state.
     * 
     * @param other The region to move from
     */
    NeuralRegion(NeuralRegion&&) noexcept;

    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership from another region, properly cleaning up any existing
     * resources and leaving the source region in a valid empty state.
     * 
     * @param other The region to move from
     * @return Reference to this region after the move
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;

    /**
     * @brief Get the unique identifier of this region
     * 
     * @return RegionId The unique identifier used throughout the system
     */
    RegionId getId() const;

    /**
     * @brief Get the human-readable name of this region
     * 
     * @return const std::string& The region name
     */
    const std::string& getName() const;

    /**
     * @brief Set the name of this region
     * 
     * @param name New human-readable name for the region
     */
    void setName(const std::string& name);

    /**
     * @brief Add a new neural population to this region
     * 
     * Creates a population with the specified number of neurons and type.
     * The population receives a unique ID automatically assigned by the region.
     * 
     * @param size Number of neurons to create in this population
     * @param type Type of neurons to create (default: Internal)
     * @return PopulationId Unique identifier of the created population
     * @throws std::invalid_argument if size is zero or negative
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);

    /**
     * @brief Get a reference to a specific population by ID
     * 
     * @param id ID of the population to retrieve
     * @return NeuralPopulation* Pointer to the population, or nullptr if not found
     */
    NeuralPopulation* getPopulation(PopulationId id);

    /**
     * @brief Get a const reference to a specific population by ID
     * 
     * @param id ID of the population to retrieve
     * @return const NeuralPopulation* Const pointer to the population, or nullptr if not found
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;

    /**
     * @brief Get the number of populations in this region
     * 
     * @return size_t Count of populations
     */
    size_t getPopulationCount() const;

    /**
     * @brief Get all populations in this region
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of unique pointers to populations
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;

    /**
     * @brief Get all populations as non-owning pointers (for performance)
     * 
     * @return std::vector<NeuralPopulation*> Vector of raw pointers to populations
     */
    std::vector<NeuralPopulation*> getAllPopulations();

    /**
     * @brief Add a synapse between two neurons within this region
     * 
     * Creates a directed synaptic connection from source to destination neuron.
     * The synapse receives automatic ID assignment and is added to connectivity maps.
     * 
     * @param source Neuron ID of the presynaptic neuron
     * @param destination Neuron ID of the postsynaptic neuron
     * @param weight Synaptic weight (positive = excitatory, negative = inhibitory, default: 0.0f)
     * @param delay Transmission delay in time steps (default: 1)
     * @return SynapseId Unique identifier of the created synapse
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                         SynapticWeight weight = 0.0f, Delay delay = 1);

    /**
     * @brief Get a reference to a specific synapse by ID
     * 
     * @param id ID of the synapse to retrieve
     * @return Synapse* Pointer to the synapse, or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);

    /**
     * @brief Get a const reference to a specific synapse by ID
     * 
     * @param id ID of the synapse to retrieve
     * @return const Synapse* Const pointer to the synapse, or nullptr if not found
     */
    const Synapse* getSynapse(SynapseId id) const;

    /**
     * @brief Get the total number of synapses in this region
     * 
     * @return size_t Count of local synapses within this region
     */
    size_t getSynapseCount() const;

    /**
     * @brief Get all synapses in this region
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Vector of unique pointers to synapses
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;

    /**
     * @brief Get all outgoing synapses from a specific neuron
     * 
     * @param neuron Neuron ID to find outgoing synapses for
     * @return std::vector<Synapse*> Vector of synapse pointers (non-owning)
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);

    /**
     * @brief Get all incoming synapses to a specific neuron
     * 
     * @param neuron Neuron ID to find incoming synapses for
     * @return std::vector<Synapse*> Vector of synapse pointers (non-owning)
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);

    /**
     * @brief Get the total number of neurons in this region
     * 
     * @return size_t Sum of neurons across all populations in the region
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get the number of active neurons in this region
     * 
     * @return size_t Count of neurons that are currently firing or have recent activity
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get the number of firing neurons in this region
     * 
     * @return size_t Count of neurons that are currently in the firing state
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Calculate the average firing rate across all neurons in the region
     * 
     * @return float Average firing rate (spikes per unit time)
     */
    float getAverageFiringRate() const;

    /**
     * @brief Calculate the average synaptic weight in this region
     * 
     * @return float Mean synaptic weight (positive/negative indicates excitation/inhibition)
     */
    float getAverageSynapticWeight() const;

    /**
     * @brief Calculate the synaptic density of this region
     * 
     * @return float Fraction of possible connections that are actually established (0.0 to 1.0)
     */
    float getSynapticDensity() const;

    /**
     * @brief Perform a simulation step for all components in this region
     * 
     * Advances the state of all populations and synapses within this region by one
     * time step, coordinating local neural dynamics and synaptic transmission.
     * 
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset all neurons and synapses to their initial state
     * 
     * Clears all accumulated state, activity, and statistics, preparing the region
     * for fresh initialization or restart.
     */
    void reset();

    /**
     * @brief Initialize random connectivity patterns within this region
     * 
     * Creates a biologically plausible random synaptic connectivity pattern
     * based on neuron types, connection probabilities, and weight distributions.
     * This simulates the spontaneous formation of neural connections during
     * brain development and early learning stages.
     * 
     * @param rng Random number generator for stochastic connection creation
     * @param connectionProbability Probability that any two neurons form a connection
     * @param meanWeight Average synaptic weight magnitude for new connections
     * @param weightVariance Standard deviation of synaptic weights
     */
    void initializeRandomConnectivity(RandomGenerator& rng,
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);

    /**
     * @brief Get all neurons across all populations in this region
     * 
     * @return std::vector<Neuron*> Vector of raw neuron pointers (non-owning)
     */
    std::vector<Neuron*> getAllNeurons();

    /**
     * @brief Get all neurons across all populations in this region (const version)
     * 
     * @return std::vector<const Neuron*> Vector of const raw neuron pointers (non-owning)
     */
    std::vector<const Neuron*> getAllNeurons() const;

private:
    /**
     * @brief Internal implementation structure using pImpl pattern
     * 
     * This struct contains all the actual implementation details of NeuralRegion,
     * including population storage, synapse management, connectivity maps,
     * and other member variables. This maintains the pImpl idiom for better
     * encapsulation and reduced compilation dependencies.
     */
    struct Impl;

    /**
     * @brief Pointer to the internal implementation
     * 
     * Manages the lifetime of the implementation struct and all contained data,
     * ensuring proper cleanup and ownership semantics.
     */
    Impl* pImpl;
};

} // namespace nlm