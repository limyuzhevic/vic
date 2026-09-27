/**
 * @file NeuralRegion.hpp
 * @brief Neural region implementation with population and connectivity management
 * 
 * This header defines the NeuralRegion class, representing a functional or anatomical
 * region of the brain that contains multiple neural populations and local connectivity.
 * Neural regions organize neurons into hierarchical structures, enabling efficient
 * management and simulation of large-scale neural networks.
 * 
 * @details NeuralRegion implementation features:
 * - **Hierarchical organization**: Groups neurons into functional units
 * - **Population management**: Manages multiple neuron populations with different types
 * - **Local connectivity**: Implements intra-regional synaptic connections
 * - **Efficient access**: Provides O(1) lookup for neurons, synapses, and populations
 * - **Statistics tracking**: Monitors regional neural activity and connectivity
 * - **Random initialization**: Supports reproducible connectivity setup
 * - **Step management**: Manages local neural computation and event processing
 * - **Reset capabilities**: Provides regional state reset functionality
 * 
 * @note Neural regions implement the hierarchical organization found in biological
 * brains, where neurons are grouped into functional units based on:
 * - **Anatomical location**: Physical proximity and connectivity
 * - **Functional specialization**: Specific computational roles
 * - **Developmental origin**: Embryonic and genetic factors
 * - **Computational efficiency**: Reduced complexity through modular organization
 * 
 * Regions enable:
 * - **Scalable network construction**: Building large networks from smaller units
 * - **Efficient resource management**: Optimizing memory and computation
 * - **Modular simulation**: Independent management of neural modules
 * - **Biological plausibility**: Implementing brain-like organization
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @warning Region parameters must be validated for simulation stability.
 * Invalid neuron counts, connection probabilities, or weights can cause
 * simulation errors or unrealistic behavior.
 * 
 * @see NeuralPopulation for neuron population implementation
 * @see Synapse for synaptic connection implementation
 * @see Brain for brain-level region management
 * @see RandomGenerator for random connectivity initialization
 * @see Neuron for individual neuron implementation
 * @see Synapse for synaptic connection implementation
 * 
 * @ingroup NeuralComponents
 */
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
 * @brief Functional/anatomical region containing neural populations and connectivity
 * 
 * NeuralRegion represents a computational unit in the neural architecture, organizing
 * neurons into functional populations with local connectivity. This class provides
 * efficient management of neural resources and computation within a hierarchical
 * brain structure.
 * 
 * @details NeuralRegion capabilities:
 * - **Population organization**: Groups neurons by type and function
 * - **Local connectivity**: Manages intra-regional synaptic connections
 * - **Efficient access**: O(1) lookup for neurons, synapses, and populations
 * - **Statistics monitoring**: Tracks regional neural activity and connectivity
 * - **Random initialization**: Supports reproducible network construction
 * - **Step management**: Manages local neural computation
 * - **State management**: Provides reset and initialization capabilities
 * 
 * **Regional Organization**:
 * NeuralRegion implements hierarchical organization:
 * 
 * **Population Structure**:
 * - Each region contains multiple neural populations
 * - Populations are specialized by neuron type (Excitatory, Inhibitory, Internal)
 * - Populations can be added dynamically during simulation
 * - Each population manages its own neuron set
 * 
 * **Connectivity Management**:
 * - **Intra-regional**: Local synapses within the region
 * - **Inter-regional**: Connections to other regions (managed by Brain)
 * - **Efficient indexing**: Fast lookup of synapses by source/destination
 * - **Connectivity statistics**: Monitors connection density and strength
 * 
 * **Usage Example**:
 * @code
 * nlm::NeuralRegion region(0, "CorticalRegion");
 * 
 * // Add excitatory neuron population
 * PopulationId excPop = region.addPopulation(100, nlm::NeuronType::Excitatory);
 * 
 * // Add inhibitory neuron population  
 * PopulationId inhPop = region.addPopulation(20, nlm::NeuronType::Inhibitory);
 * 
 * // Create local synapse between neurons
 * SynapseId synapse = region.addSynapse(0, 42, 0.5f, 2);
 * 
 * // Initialize random connectivity
 * nlm::RandomGenerator rng(12345);
 * region.initializeRandomConnectivity(rng, 0.1f, 0.5f, 0.1f);
 * 
 * // Simulation loop
 * for (int step = 0; step < 1000; ++step) {
 *     region.step(step * 0.1); // 0.1 ms time step
 * }
 * @endcode
 * 
 * **Regional Statistics**:
 * Regions provide detailed statistics for monitoring and analysis:
 * - **Neural statistics**: Neuron count, activity, firing rates
 * - **Connectivity statistics**: Synaptic weight, density, patterns
 * - **Computational metrics**: Processing load, efficiency
 * 
 * @see NeuralPopulation for neuron population implementation
 * @see Synapse for synaptic connection implementation
 * @see Brain for brain-level region management
 * @see RandomGenerator for random connectivity initialization
 * @see Neuron for individual neuron implementation
 * @see Synapse for synaptic connection implementation
 * @see NLMException for error conditions
 * 
 * @warning Region initialization must complete before simulation steps.
 * Invalid parameters can cause simulation instability.
 * 
 * @note NeuralRegion uses the pImpl idiom for efficient memory management
 * and forward declaration of the implementation class.
 * 
 * @ingroup NeuralComponents
 */
class NeuralRegion {
public:
    /**
     * @brief Construct a neural region with identifier
     * 
     * Creates a new neural region with a unique identifier. The region is
     * initialized in an empty state and ready for population and synapse
     * addition.
     * 
     * @param id Unique identifier for this region
     * 
     * @see NeuralRegion(RegionId id, const std::string& name) for named constructor
     * @see addPopulation() for adding neuron populations
     * @see addSynapse() for adding synaptic connections
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Construct a neural region with identifier and name
     * 
     * Creates a new neural region with a unique identifier and descriptive name.
     * This constructor provides both functional identification (ID) and semantic
     * labeling (name) for the region.
     * 
     * @param id Unique identifier for this region
     * @param name Descriptive name for the region
     * 
     * @see NeuralRegion(RegionId id) for unnamed constructor
     * @see getId() for retrieving region identifier
     * @see getName() for retrieving region name
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor
     * 
     * Cleans up region resources and performs any necessary cleanup operations.
     * Memory management is handled via the pImpl pattern.
     */
    ~NeuralRegion();
    
    /**
     * @brief Copy constructor is deleted (use move semantics instead)
     * 
     * Regions cannot be copied due to their complex internal state and
     * implementation details. Use move semantics for efficient transfer.
     * 
     * @param other Source region to copy (deleted)
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     * 
     * Regions cannot be assigned due to their complex internal state and
     * implementation details. Use move assignment for efficient transfer.
     * 
     * @param other Source region to assign (deleted)
     * @return NeuralRegion& Reference to assigned region (never returned)
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor (efficient)
     * 
     * Transfers ownership of region implementation from source to new region
     * without copying complex internal state. This provides efficient region
     * transfer and reassignment.
     * 
     * @param other Source region to move from
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient)
     * 
     * Transfers ownership of region implementation from source to existing region
     * without copying complex internal state. This provides efficient region
     * reassignment and transfer.
     * 
     * @param other Source region to move from
     * @return NeuralRegion& Reference to this region
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get the region's unique identifier
     * 
     * @return RegionId Unique identifier for this region
     * 
     * @see getName() for retrieving region name
     * @see addPopulation() for adding neuron populations
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42, "VisualCortex");
     * RegionId id = region.getId();
     * std::cout << "Region ID: " << id << std::endl;
     * @endcode
     */
    RegionId getId() const;
    
    /**
     * @brief Get the region's name
     * 
     * @return const std::string& Descriptive name for this region
     * 
     * @see setName() for setting region name
     * @see getId() for retrieving region identifier
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42, "VisualCortex");
     * std::string name = region.getName();
     * std::cout << "Region name: " << name << std::endl;
     * @endcode
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the region's name
     * 
     * @param name New descriptive name for this region
     * 
     * @see getName() for retrieving region name
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * region.setName("VisualCortex");
     * @endcode
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add a neuron population to the region
     * 
     * Creates and adds a new neuron population to the region. The population
     * contains neurons of the specified type with the given size. This method
     * initializes the population with default parameters and adds it to the
     * region's population list.
     * 
     * @param size Number of neurons in the population
     * @param type Neuron type for the population (default: NeuronType::Internal)
     * @return PopulationId Identifier for the newly created population
     * 
     * @see getPopulation() for retrieving populations
     * @see getPopulationCount() for population statistics
     * @see getAllPopulations() for all populations
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * PopulationId popId = region.addPopulation(100, nlm::NeuronType::Excitatory);
     * // Create population with 100 excitatory neurons
     * @endcode
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a neuron population
     * 
     * @param id Population identifier to retrieve
     * @return NeuralPopulation* Pointer to population if found, nullptr otherwise
     * 
     * @see getPopulation(id) const for const version
     * @see addPopulation() for population creation
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * PopulationId popId = region.addPopulation(100);
     * nlm::NeuralPopulation* pop = region.getPopulation(popId);
     * if (pop) {
     *     std::cout << "Found population with " << pop->getSize() << " neurons" << std::endl;
     * }
     * @endcode
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get a neuron population (const version)
     * 
     * @param id Population identifier to retrieve
     * @return const NeuralPopulation* Const pointer to population if found, nullptr otherwise
     * 
     * @see getPopulation(id) for non-const version
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get population count
     * 
     * @return size_t Number of populations in the region
     * 
     * @see addPopulation() for population creation
     * @see getPopulationCount() for population statistics
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * region.addPopulation(100); // Add one population
     * size_t count = region.getPopulationCount();
     * std::cout << "Region has " << count << " populations" << std::endl;
     * @endcode
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in the region
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of all populations
     * 
     * @note Populations are returned in implementation-defined order.
     * Population pointers should not be stored for long periods as populations
     * may be reallocated.
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all populations in the region (non-const version)
     * 
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * 
     * @note Population pointers should not be stored for long periods as
     * populations may be reallocated.
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Add a synapse within this region
     * 
     * Creates and adds a synaptic connection between two neurons within this
     * region. The synapse is initialized with the specified parameters and
     * added to the region's synapse list.
     * 
     * @param source NeuronId of the source neuron (presynaptic)
     * @param destination NeuronId of the destination neuron (postsynaptic)
     * @param weight Synaptic weight (default: 0.0f)
     * @param delay Transmission delay (default: 1)
     * @return SynapseId Identifier for the newly created synapse
     * 
     * @see getSynapse() for retrieving synapses
     * @see getSynapseCount() for synapse statistics
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * // Create synapse between neuron 10 and neuron 42
     * SynapseId synId = region.addSynapse(10, 42, 0.5f, 2);
     * @endcode
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a synapse
     * 
     * @param id Synapse identifier to retrieve
     * @return Synapse* Pointer to synapse if found, nullptr otherwise
     * 
     * @see getSynapse(id) const for const version
     * @see addSynapse() for synapse creation
     * 
     * @par Example
     * @code
     * nlm::NeuralRegion region(42);
     * SynapseId synId = region.addSynapse(10, 42);
     * nlm::Synapse* syn = region.getSynapse(synId);
     * if (syn) {
     *     std::cout << "Found synapse between neurons " 
     *               << syn->getSourceNeuron() << " and " 
     *               << syn->getDestinationNeuron() << std::endl;
     * }
     * @endcode
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get a synapse (const version)
     * 
     * @param id Synapse identifier to retrieve
     * @return const Synapse* Const pointer to synapse if found, nullptr otherwise
     * 
     * @see getSynapse(id) for non-const version
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get synapse count
     * 
     * @return size_t Number of synapses in the region
     * 
     * @see addSynapse() for synapse creation
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in the region
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Vector of all synapses
     * 
     * @note Synapses are returned in implementation-defined order.
     * Synapse pointers should not be stored for long periods as synapses
     * may be reallocated.
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get synapses originating from a neuron
     * 
     * @param neuron Neuron identifier to query
     * @return std::vector<Synapse*> Vector of synapses originating from the neuron
     * 
     * @note This method provides fast access to outgoing synapses for a neuron.
     * The returned vector is dynamically allocated and should not be stored
     * for long periods.
     * 
     * @see getSynapsesTo() for incoming synapses
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get synapses terminating at a neuron
     * 
     * @param neuron Neuron identifier to query
     * @return std::vector<Synapse*> Vector of synapses terminating at the neuron
     * 
     * @note This method provides fast access to incoming synapses for a neuron.
     * The returned vector is dynamically allocated and should not be stored
     * for long periods.
     * 
     * @see getSynapsesFrom() for outgoing synapses
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get total neuron count in the region
     * 
     * @return size_t Total number of neurons in the region
     * 
     * @note This includes neurons in all populations within the region.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get active neuron count in the region
     * 
     * @return size_t Number of neurons currently active (membrane potential != resting)
     * 
     * @note Active neurons are those that have been excited or are in process
     * of integration. This metric helps monitor regional activity levels.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count in the region
     * 
     * @return size_t Number of neurons currently firing (in refractory period)
     * 
     * @note Firing neurons are those that have just crossed threshold and
     * are producing action potentials.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate in the region
     * 
     * @return float Average firing rate across all neurons in the region (Hz)
     * 
     * @note This is a regional statistic computed from all neurons in the region.
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get average synaptic weight in the region
     * 
     * @return float Average synaptic weight across all synapses in the region
     * 
     * @note This is a connectivity metric computed from all synapses in the region.
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get synaptic density
     * 
     * @return float Synaptic density (fraction of possible connections)
     * 
     * @note Synaptic density is calculated as:
     * actual_connections / (num_neurons * num_neurons - num_neurons)
     * This metric helps assess connectivity patterns in the region.
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * @brief Step all populations and synapses in the region
     * 
     * Performs one complete simulation step for all neural components within
     * the region. This method executes:
     * - Population dynamics (neuron updates)
     * - Synapse transmission (synaptic inputs)
     * - Event processing (spike generation and propagation)
     * - State management (neuron and synapse state updates)
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @see NeuralPopulation::step() for population implementation
     * @see Synapse::step() for synapse implementation
     * @see step() for brain-level stepping
     * @see reset() for state reset
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses in the region
     * 
     * Resets all neural state variables to their initial values:
     * - Neuron membrane potentials to resting values
     * - Neuron firing states to resting
     * - Synaptic weights to initial values
     * - Synapse history to empty
     * - Spike system events to clear
     * 
     * This method is useful for:
     * - Resetting regional state between trials
     * - Running the same simulation multiple times
     * - Testing with consistent initial conditions
     * 
     * @see step() for simulation steps
     * @see reset() for brain-level reset
     * @see initializeRandomConnectivity() for connectivity initialization
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity in the region
     * 
     * Sets up synaptic connections between neurons using random parameters.
     * This method creates a biologically plausible connectivity pattern
     * based on the specified parameters.
     * 
     * @param rng Reference to random number generator
     * @param connectionProbability Probability of connection between any two neurons
     * @param meanWeight Mean synaptic weight for new connections
     * @param weightVariance Variance of synaptic weights for new connections
     * 
     * @note This method implements initial network construction. It should be
     * called after all populations have been added but before simulation steps.
     * 
     * @see reset() for state reset
     * @see initialize() for overall brain initialization
     * @param RandomGenerator for random number generation
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    /**
     * @brief Get all neurons across all populations
     * 
     * @return std::vector<Neuron*> Vector of all neuron pointers in the region
     * 
     * @note Neuron pointers should not be stored for long periods as neurons
     * may be reallocated during simulation.
     * 
     * @see getAllNeurons() const for const version
     * @see addPopulation() for neuron creation
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations (const version)
     * 
     * @return std::vector<const Neuron*> Vector of all const neuron pointers
     * 
     * @see getAllNeurons() for non-const version
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
