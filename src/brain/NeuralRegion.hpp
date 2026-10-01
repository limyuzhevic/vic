#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * @brief A brain region containing multiple populations and local connectivity
 * 
 * NeuralRegion represents a functional or anatomical subdivision of the brain
 * that contains multiple neural populations. Each region has its own unique
 * characteristics, connectivity patterns, and computational properties.
 * Regions are the primary organizational unit for implementing complex
 * brain architectures and functional specialization.
 * 
 * Key features:
 * - Contains multiple neural populations with diverse functions
 * - Manages local synaptic connectivity between neurons
 * - Provides region-level statistics and metrics
 * - Supports region-specific dynamics and behavior
 * - Enables hierarchical brain organization
 * 
 * @note This is a placeholder implementation. Phase 2 will implement
 *       region-level dynamics, local connectivity rules, and specialized
 *       region behaviors such as oscillatory activity or specific computational
 *       functions.
 * 
 * @see NeuralPopulation Building blocks within regions
 * @see Synapse Local connections within regions
 * @see Brain Top-level coordinator of all regions
 * 
 * Example usage:
 * @code
 * NeuralRegion cortex(0, "Cerebral Cortex");
 * PopulationId excitatoryPop = cortex.addPopulation(1000, NeuronType::Excitatory);
 * PopulationId inhibitoryPop = cortex.addPopulation(200, NeuronType::Inhibitory);
 * SynapseId syn = cortex.addSynapse(1, 2, 0.5f);
 * cortex.initializeRandomConnectivity(rng, 0.1f, 0.5f, 0.1f);
 * @endcode
 */
class NeuralRegion {
public:
    /**
     * @brief Constructor creating a region with only an identifier
     * 
     * Creates a neural region with a unique identifier. The region will be
     * nameless initially but can be given a name using setName(). This is
     * useful for creating regions programmatically.
     * 
     * @param id Unique identifier for this region (typically 0-based)
     * @throws std::invalid_argument if id is invalid or already in use
     * 
     * @note Common region ID conventions: 0 for input layer, 1-5 for cortical
     *       layers, 10+ for subcortical regions, etc.
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Constructor creating a region with identifier and name
     * 
     * Creates a neural region with a unique identifier and a descriptive name.
     * This is useful for creating regions with meaningful labels.
     * 
     * @param id Unique identifier for this region
     * @param name Descriptive name for the region (e.g., "Cerebral Cortex")
     * @throws std::invalid_argument if id is invalid or already in use
     * 
     * @note The name is used for logging, debugging, and visualization purposes.
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~NeuralRegion();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * NeuralRegion objects cannot be copied to prevent resource management
     * issues and maintain region integrity. Use move semantics instead.
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of internal resources from another NeuralRegion
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The NeuralRegion object to move from
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of internal resources from another NeuralRegion
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The NeuralRegion object to move from
     * @return Reference to this NeuralRegion object
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this region
     * 
     * Returns the region ID that was specified during construction.
     * This identifier is used for referencing the region and maintaining
     * connectivity and statistics.
     * 
     * @return RegionId The unique identifier for this region
     * 
     * @see Brain::addRegion() for creating regions
     */
    RegionId getId() const;
    
    /**
     * @brief Get the name of this region
     * 
     * Returns the descriptive name of the region. If no name was provided
     * during construction, returns an empty string.
     * 
     * @return const std::string& The name of this region
     * 
     * @note The name is used for logging, debugging, and visualization.
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the name of this region
     * 
     * Changes the descriptive name of the region. This is useful for
     * renaming regions during initialization or for providing meaningful
     * labels for visualization and analysis.
     * 
     * @param name The new name for this region
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add a new neural population to this region
     * 
     * Creates a new neural population with the specified size and neuron type,
     * and adds it to this region. Populations are the basic functional units
     * within a region and contain groups of neurons with similar properties.
     * 
     * @param size Number of neurons to create in the population
     * @param type Neuron type for the population (defaults to Internal)
     * @return PopulationId The identifier of the newly created population
     * @throws std::runtime_error if population creation fails
     * 
     * @note Common neuron types:
     *       - Excitatory: Fast, strong synaptic connections
     *       - Inhibitory: Fast, suppressive synaptic connections
     *       - Internal: Modulatory or intrinsic properties
     * 
     * @see getPopulation() Retrieve a population by ID
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a neural population by identifier
     * 
     * Returns a pointer to the neural population with the specified ID. The returned
     * pointer remains valid for the lifetime of the region.
     * 
     * @param id The identifier of the population to retrieve
     * @return NeuralPopulation* Pointer to the population
     * @return const NeuralPopulation* Const pointer to the population
     * @return nullptr if the population does not exist
     */
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get the number of populations in this region
     * 
     * @return size_t Number of populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all neural populations in this region
     * 
     * Returns a reference to the container containing all neural populations
     * in the region. This provides read-only access to all populations
     * for iteration or statistics.
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of population pointers
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all neural populations as non-const pointers
     * 
     * Returns pointers to all neural populations in the region. This provides
     * mutable access to all populations, allowing for direct modification
     * of population properties.
     * 
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Add a local synapse connection
     * 
     * Creates a synapse connection between two neurons within this region.
     * Local synapses typically have shorter delays and different plasticity
     * rules than inter-regional connections. This is the primary method for
     * building local connectivity within a region.
     * 
     * @param source Neuron identifier for the presynaptic neuron
     * @param destination Neuron identifier for the postsynaptic neuron
     * @param weight Synaptic weight (defaults to 0.0)
     * @param delay Signal delay in timesteps (defaults to 1)
     * @return SynapseId The identifier of the newly created synapse
     * @throws std::runtime_error if synapse creation fails
     * 
     * @note Synapse weights are typically normalized between 0.0 and 1.0.
     *       Positive weights indicate excitatory connections, negative weights
     *       inhibitory connections.
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a synapse by identifier
     * 
     * Returns a pointer to the synapse with the specified ID. The returned
     * pointer remains valid for the lifetime of the region.
     * 
     * @param id The identifier of the synapse to retrieve
     * @return Synapse* Pointer to the synapse
     * @return const Synapse* Const pointer to the synapse
     * @return nullptr if the synapse does not exist
     */
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get the number of synapses in this region
     * 
     * @return size_t Number of synapses
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in this region
     * 
     * Returns a reference to the container containing all synapses in the region.
     * This provides read-only access to all synapses for iteration or statistics.
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Vector of synapse pointers
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get all synapses originating from a specific neuron
     * 
     * Returns all synapses where the specified neuron is the source (presynaptic)
     * neuron. These are the outgoing connections from this neuron.
     * 
     * @param neuron The neuron identifier to query
     * @return std::vector<Synapse*> Vector of outgoing synapses from the neuron
     * 
     * @note Useful for analyzing the connectivity patterns and influence of
     *       individual neurons within the region.
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses targeting a specific neuron
     * 
     * Returns all synapses where the specified neuron is the destination
     * (postsynaptic) neuron. These are the incoming connections to this neuron.
     * 
     * @param neuron The neuron identifier to query
     * @return std::vector<Synapse*> Vector of incoming synapses to the neuron
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get the total number of neurons in this region
     * 
     * Returns the sum of neurons across all populations in the region.
     * This is a key metric for understanding region scale and computational
     * capacity.
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the number of active neurons in this region
     * 
     * Returns the count of neurons across all populations that are currently
     * active (firing or exhibiting activity). This is useful for measuring
     * region-level activity.
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of firing neurons in this region
     * 
     * Returns the count of neurons across all populations that are currently
     * in the firing state, generating action potentials or spikes.
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in the region
     * 
     * Returns the average firing rate of neurons across all populations in the
     * region, providing a measure of overall region activity and excitability.
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the average synaptic weight in the region
     * 
     * Returns the average synaptic weight across all synapses in the region.
     * This provides a measure of the overall strength and balance of
     * connectivity within the region.
     * 
     * @return float Average synaptic weight (typically 0.0 to 1.0)
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the synaptic density of the region
     * 
     * Returns the fraction of possible synaptic connections that are actually
     * present in the region. This is a measure of connectivity density and
     * network integration.
     * 
     * @return float Synaptic density (0.0 to 1.0)
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * @brief Step all neurons and synapses in the region
     * 
     * Advances all neurons and synapses in the region by one timestep according
     * to their specific dynamics and the current simulation time. This is the
     * primary operation for driving regional activity and dynamics.
     * 
     * @param currentTime The current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses to initial state
     * 
     * Resets all neurons and synapses in the region to their initial conditions
     * as defined during construction or initialization. This includes resetting
     * membrane potentials, firing states, synaptic weights, and other internal
     * state variables.
     * 
     * @note Useful for running multiple simulation trials with the same initial
     *       conditions or for testing regional reset behavior.
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity patterns
     * 
     * Creates random synaptic connections between neurons in the region based
     * on specified parameters. This is typically called once at the beginning
     * of the simulation to establish initial connectivity patterns.
     * 
     * @param rng Reference to the random number generator to use for connectivity
     * @param connectionProbability Probability that a specific connection exists (0.0 to 1.0)
     * @param meanWeight Mean synaptic weight for new connections
     * @param weightVariance Variance of synaptic weights for new connections
     * 
     * @note This creates both excitatory and inhibitory connections based on
     *       the probability and weight parameters. Connection probability should
     *       be chosen carefully to balance network integration and sparsity.
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    /**
     * @brief Get all neurons across all populations in the region
     * 
     * Returns pointers to all neurons in the region, traversing all populations
     * and collecting every neuron. This provides a global view of all neurons
     * in the region for analysis or manipulation.
     * 
     * @return std::vector<Neuron*> Vector of neuron pointers
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations in the region (const version)
     * 
     * Returns const pointers to all neurons in the region. This provides read-only
     * access to all neurons for analysis or inspection.
     * 
     * @return std::vector<const Neuron*> Vector of const neuron pointers
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the NeuralRegion class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
