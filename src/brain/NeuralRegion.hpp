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
// This class represents a functional brain region that integrates multiple
// neural populations through local connectivity. Brain regions are the
// fundamental organizational units of the NLM architecture, combining:
//
// - Multiple neural populations with specialized functions
// - Local synaptic connections within the region
// - Input and output interfaces with other regions
// - Computational capabilities specific to region function
//
// Biological analogs include:
// - Cortical columns
// - Brain areas
// - Functional units
//
// Key features:
// - Modular organization for efficient computation
// - Local connectivity patterns
// - Population-based processing
// - Integration with higher-level brain systems
//
// Neural regions enable:
// - Distributed processing
// - Specialization of function
// - Hierarchical information flow
// - Scalable architecture
//
// @note This class implements the core organizational principle of NLM:
//       dividing complex neural computation into manageable, specialized units
class NeuralRegion {
public:
// Create neural region with unique identifier
//
// @param id Unique identifier for this region
//
// @note Region ID allows:
//       - Unique identification within brain
//       - Look-up operations via getRegion()
//       - Inter-region connection establishment
//       - Statistical analysis
//
// @warning Region ID must be unique within the brain
//         Generated automatically when using brain.addRegion()
    explicit NeuralRegion(RegionId id);

    // Create neural region with identifier and descriptive name
//
// @param id Unique identifier for this region
// @param name Human-readable name for debugging and documentation
//
// @note Name is optional - if empty, auto-generated from ID
//       Used in logging, statistics, and debugging output
//
// @param name is optional (can be empty string)
    NeuralRegion(RegionId id, const std::string& name);
    
// Destructor
//
// @note Automatic cleanup of region components and implementation details
//       Pimpl idiom ensures efficient memory management
~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
// Get unique identifier of this neural region
//
// @return RegionId Unique identifier for this region
//
// @note Region IDs are strongly-typed for safety and debugging
//       and can be used for inter-region communication
    RegionId getId() const;

    // Get human-readable name of this region
//
// @return const std::string& Reference to region name
//
// @note Used for:
//       - Logging and debugging
//       - Statistics reporting
//       - Documentation
//       - User interface display
    const std::string& getName() const;

    // Set region name
//
// @param name New region name
//
// @note Name is for debugging/documentation only
//       Does not affect region functionality
    void setName(const std::string& name);
    
// Create and add neural population to this region
//
// @param size Number of neurons in new population
// @param type Neuron type for all members (default: Internal)
//
// @return PopulationId Unique identifier for new population
//
// @note Population creation:
//       1. Allocates memory for neurons
//       2. Initializes neuron parameters
//       3. Adds neurons to region via addNeuron()
//       4. Returns population identifier
//
// @note Neuron types determine:
//       - Synaptic targets
//       - Plasticity rules
//       - Population function
//       - Network dynamics
//
// @param type is optional (defaults to NeuronType::Internal)
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);

    // Get neural population by identifier
//
// @param id Population identifier
//
// @return NeuralPopulation* Pointer to population, or nullptr if not found
//
// @note Used for:
//       - Direct population manipulation
//       - Accessing population-specific properties
//       - Creating synapses to population neurons
//       - Population-level statistics
//
// @warning Population must exist (created via addPopulation())
    NeuralPopulation* getPopulation(PopulationId id);

    // Get constant neural population by identifier (read-only access)
//
// @param id Population identifier
//
// @return const NeuralPopulation* Constant pointer to population, or nullptr if not found
    const NeuralPopulation* getPopulation(PopulationId id) const;

    // Get number of populations in this region
//
// @return size_t Number of populations in region
//
// @note Used for:
//       - Region capacity analysis
//       - Statistical reporting
//       - Resource allocation
//       - Debugging
    size_t getPopulationCount() const;

    // Get all populations in this region
//
// @return const std::vector<std::unique_ptr<NeuralPopulation>>& Reference to population collection
//
// @note Returns reference to internal vector - modifications affect region
//       Use addPopulation() and getPopulation() for safe manipulation
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;

    // Get all population pointers (mutable access)
//
// @return std::vector<NeuralPopulation*> Vector of population pointers
//
// @note Useful for:
//       - Iteration over populations
//       - Bulk operations
//       - Population-level algorithms
//       - Distributed computing
    std::vector<NeuralPopulation*> getAllPopulations();
    
// Create and add synaptic connection within this region
//
// @param source Neuron identifier of source neuron
// @param destination Neuron identifier of target neuron
// @param weight Initial synaptic weight (default: 0.0f)
// @param delay Transmission delay in simulation steps (default: 1)
//
// @return SynapseId Unique identifier for new synapse
//
// @note Synapse creation:
//       1. Validates neuron IDs exist in region
//       2. Creates synapse object
//       3. Initializes synaptic parameters
//       4. Returns synapse identifier
//
// @note Synaptic connections:
//       - Enable local information flow
//       - Create computational circuits
//       - Support pattern formation
//       - Enable learning through plasticity
//
// @param weight is optional (defaults to 0.0)
// @param delay is optional (defaults to 1 step)
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                         SynapticWeight weight = 0.0f, Delay delay = 1);

    // Get synapse by identifier
//
// @param id Synapse identifier
//
// @return Synapse* Pointer to synapse, or nullptr if not found
//
// @note Used for:
//       - Direct synapse manipulation
//       - Accessing synaptic properties
//       - Setting/modifying weights
//       - Studying connectivity
//
// @warning Synapse must exist (created via addSynapse())
    Synapse* getSynapse(SynapseId id);

    // Get constant synapse by identifier (read-only access)
//
// @param id Synapse identifier
//
// @return const Synapse* Constant pointer to synapse, or nullptr if not found
    const Synapse* getSynapse(SynapseId id) const;

    // Get number of synapses in this region
//
// @return size_t Number of synapses in region
//
// @note Used for:
//       - Connectivity analysis
//       - Resource management
//       - Statistical reporting
//       - Network efficiency metrics
    size_t getSynapseCount() const;

    // Get all synapses in this region
//
// @return const std::vector<std::unique_ptr<Synapse>>& Reference to synapse collection
//
// @note Returns reference to internal vector - modifications affect region
//       Use addSynapse() and getSynapse() for safe manipulation
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
// Get all synapses originating from specific neuron
//
// @param neuron Neuron identifier
//
// @return std::vector<Synapse*> Vector of outgoing synapses
//
// @note Used for:
//       - Analyzing neuron output
//       - Understanding firing patterns
//       - Studying network dynamics
//       - Debugging connectivity
//
// @warning Neuron must exist in this region
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);

    // Get all synapses terminating at specific neuron
//
// @param neuron Neuron identifier
//
// @return std::vector<Synapse*> Vector of incoming synapses
//
// @note Used for:
//       - Analyzing neuron input
//       - Studying integration
//       - Understanding reception patterns
//       - Circuit analysis
//
// @warning Neuron must exist in this region
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
// Get total number of neurons in this region
//
// @return size_t Total neuron count across all populations
//
// @note Used for:
//       - Resource allocation
//       - Capacity analysis
//       - Network statistics
//       - Regional size comparison
    size_t getTotalNeuronCount() const;

    // Get number of active neurons in this region
//
// @return size_t Number of neurons with membrane potential above threshold
//
// @note Active neurons contribute to:
//       - Regional computation
//       - Information processing
//       - Network dynamics
//       - Learning signals
    size_t getActiveNeuronCount() const;

    // Get number of firing neurons in this region
//
// @return size_t Number of neurons in Active firing state
//
// @note Firing neurons:
//       - Generate action potentials
//       - Transmit signals
//       - Drive network activity
//       - Produce output
    size_t getFiringNeuronCount() const;

    // Get average firing rate across all neurons in region
//
// @return float Average firing rate in Hertz (Hz)
//
// @note Regional firing rate measures:
//       - Overall activity level
//       - Regional engagement
//       - Network performance
//       - Synchronization
    float getAverageFiringRate() const;
    
// Get average synaptic weight in this region
//
// @return float Average synaptic weight (-1.0 to 1.0)
//
// @note Used for:
//       - Network stability analysis
//       - Learning rate assessment
//       - Network health monitoring
//       - Comparison with theoretical models
    float getAverageSynapticWeight() const;

    // Get synaptic density in this region
//
// @return float Fraction of possible connections that are realized (0.0 to 1.0)
//
// @note Synaptic density measures:
//       - Network connectivity
//       - Information integration capacity
//       - Computational potential
//       - Wiring efficiency
//
// @note Higher density generally means:
//       - More information processing capacity
//       - Greater potential for complex computation
//       - Increased metabolic cost
//       - Higher stability (due to redundancy)
    float getSynapticDensity() const;  // fraction of possible connections

    // Update all populations and synapses in this region for one simulation step
//
// @param currentTime Current simulation time in seconds
//
// @note This method:
//       1. Advances all neurons in all populations
//       2. Processes all synaptic transmissions
//       3. Updates population and synapse statistics
//       4. Handles local plasticity updates
//
// @note Regional stepping enables:
//       - Parallel processing of populations
//       - Efficient local computation
//       - Modular network dynamics
//       - Scalable simulation architecture
    void step(Timestamp currentTime);
    
// Reset all neurons and synapses in this region
//
// @note This method:
//       1. Clears all spike events
//       2. Resets neuron states
//       3. Clears synaptic inputs
//       4. Resets firing states
//       5. Preserves region structure (ID, name, populations)
//
// @note Similar to initializeRandomConnectivity() but preserves region identity
//       rather than recreating the entire region structure
    void reset();

    // Initialize random connectivity between neurons
//
// @param rng Random number generator for stochastic connectivity
// @param connectionProbability Probability that each possible connection exists
// @param meanWeight Average synaptic weight for new connections
// @param weightVariance Variance of synaptic weights
//
// @note This method:
//       1. Creates random synaptic connections between neurons
//       2. Assigns biologically plausible weights
//       3. Establishes baseline connectivity
//       4. Enables population interaction
//
// @note Connectivity initialization:
//       - Determines network structure
//       - Sets initial learning potential
//       - Establishes information flow pathways
//       - Creates computational capabilities
//
// @warning Must be called after all neurons have been added to populations
//         otherwise neurons will not be connected
    void initializeRandomConnectivity(class RandomGenerator& rng,
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
// Get all neurons in this region (mutable access)
//
// @return std::vector<Neuron*> Vector of all neuron pointers
//
// @note Useful for:
//       - Region-wide operations
//       - Bulk algorithms
//       - Distributed processing
//       - Complete region analysis
//
// @note Order corresponds to getPopulations() structure
    std::vector<Neuron*> getAllNeurons();

    // Get all neurons in this region (constant access)
//
// @return std::vector<const Neuron*> Vector of all constant neuron pointers
//
// @note Used for:
//       - Read-only region analysis
//       - Statistical computation
//       - Network modeling
//       - Validation and testing
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
