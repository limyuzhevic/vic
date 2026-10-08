#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
//
// This class represents a population of neurons that work together as a unit
// for specific computational tasks. All neurons in a population share the
// same neuron type and are typically organized by functional role (e.g.,
// sensory processing, motor control, memory functions).
//
// Key features:
// - Shared properties across all member neurons
// - Efficient memory layout for population operations
// - Specialized for population-level computations
// - Supports competitive and cooperative dynamics
//
// Population organization is fundamental to:
// - Efficient coding schemes
// - Modular brain architecture
// - Parallel processing capabilities
// - Functional segregation/integration
//
// @note This class enables scalable neural network design by grouping
//       neurons with similar properties and computational roles
class NeuralPopulation {
public:
// Create neural population with unique identifier and size
//
// @param id Unique identifier for this population
// @param size Number of neurons in this population
//
// @note Population is created with empty neuron list - neurons are added
//       individually via addNeuron() or during initialization
//
// @warning Population size should be chosen based on computational requirements
//         and available resources. Too small = inefficient, too large = excessive memory
NeuralPopulation(PopulationId id, size_t size);
    
// Destructor
//
// @note Automatic memory management via Pimpl idiom ensures proper cleanup
//       of neuron storage and implementation details
~NeuralPopulation();
    
    // Disable copying, enable moving
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
// Get unique identifier of this neural population
//
// @return PopulationId Unique identifier for this population
//
// @note Population identifiers are strongly-typed for safety and debugging
//       and can be used to look up populations in the brain
PopulationId getId() const;
    
// Get number of neurons in this population
//
// @return size_t Number of neurons in the population
//
// @note Population size determines computational capacity and memory usage
//       and influences dynamics (smaller populations = faster, larger = more complex)
    size_t getSize() const;

    // Check if population is empty
    //
    // @return bool true if population has no neurons, false otherwise
    //
    // @note Useful for:
    //       - Validation before operations
    //       - Checking population health
    //       - Early termination conditions
    bool isEmpty() const;
    
// Get neuron by index in population
//
// @param index Index of neuron to retrieve (0-based)
//
// @return Neuron* Pointer to neuron at specified index
//
// @warning Index out of bounds will cause undefined behavior
//         Use getSize() to check bounds or iterate safely
//
// @note Neurons in a population are typically ordered by:
//       - Creation time
//       - Position in brain region
//       - Functional specialization
    Neuron* getNeuron(NeuronIndex index);

    // Get constant neuron by index for read-only access
//
// @param index Index of neuron to retrieve (0-based)
//
// @return const Neuron* Constant pointer to neuron at specified index
//
// @note Same as getNeuron() but prevents modification of neuron state
//       Useful when only read access is needed
    const Neuron* getNeuron(NeuronIndex index) const;

    // Add neuron to population
//
// @param neuron Pointer to neuron to add
//
// @warning Neuron must not already belong to another population
//         Ensures single population membership for consistent dynamics
//
// @note Neurons are stored in population-specific container for efficient access
//       and population-level operations
    void addNeuron(Neuron* neuron);
    
// Get all neurons in population
//
// @return const std::vector<Neuron*>& Reference to neuron collection vector
//
// @note Returns reference to internal vector - modifications affect population
//       Use carefully and consider thread safety
//
// @warning Modifying the returned vector directly bypasses population safety checks
//         Use addNeuron() and removeNeuron() for safe modifications
    const std::vector<Neuron*>& getNeurons() const;
    
// Get neuron type for this population
//
// @return NeuronType Type of all neurons in this population
//
// @note All neurons in a population share the same type:
//       - Excitatory, Inhibitory, or Modulatory
//       - Influences synaptic connections and computational role
//       - Affects population interaction patterns
//
// @note Population type determines:
//       - Synaptic targets
//       - Plasticity rules
//       - Network dynamics
    NeuronType getNeuronType() const;

    // Set neuron type for this population
//
// @param type New neuron type to assign
//
// @warning Changing population type affects all member neurons
//         May require updates to plasticity rules and connectivity
//
// @note Use with caution as type changes neuron properties globally
    void setNeuronType(NeuronType type);
    
// Get number of active neurons in population
//
// @return size_t Number of neurons with membrane potential above threshold
//
// @note Active neurons contribute to:
//       - Population output
//       - Network computation
//       - Information processing
//       - Memory encoding
    size_t getActiveCount() const;

    // Get number of firing neurons in population
//
// @return size_t Number of neurons in Active firing state
//
// @note Firing neurons:
//       - Generate action potentials
//       - Transmit signals to postsynaptic neurons
//       - Drive network dynamics
//       - Produce observable output
    size_t getFiringCount() const;

    // Get average firing rate across all neurons in population
//
// @return float Average firing rate in Hertz (Hz)
//
// @note Population firing rate measures:
//       - Overall activity level
//       - Network engagement
//       - Computational load
//       - Synchrony patterns
    float getAverageFiringRate() const;

    // Get population activity level
//
// @return float Fraction of neurons that are active (0.0 to 1.0)
//
// @note Activity level is a normalized measure useful for:
//       - Comparing populations of different sizes
//       - Measuring engagement
//       - Detecting states (high vs. low activity)
//       - Normalization in network models
    float getActivityLevel() const;  // fraction of active neurons;
    
// Initialize all neurons in population with random biological parameters
//
// @param rng Random number generator for stochastic initialization
//
// @note This method:
//       1. Sets each neuron to valid initial state
//       2. Assigns biologically plausible parameters
//       3. Ensures diversity across population members
//       4. Sets up for proper dynamics and learning
//
// @note Neuron initialization includes:
//       - Membrane potential near resting potential
//       - Firing threshold for spike generation
//       - Refractory period settings
//       - Synaptic parameters
//       - Plasticity state
//
// @warning Must be called after all neurons have been added to population
//         Otherwise neurons will not be properly initialized
    void initializeRandom(class RandomGenerator& rng);

    // Update all neurons in population for one simulation step
//
// @param currentTime Current simulation time in seconds
//
// @note This method:
//       1. Advances each neuron through its dynamics
//       2. Processes synaptic inputs
//       3. Generates spikes when thresholds are reached
//       4. Updates firing rates and states
//       5. Handles plasticity updates
//
// @note Population stepping aggregates individual neuron computation
//       allowing for efficient parallel or vectorized implementations
    void step(Timestamp currentTime);

    // Reset all neurons in population to initial state
//
// @note This method:
//       1. Clears spike history
//       2. Resets membrane potentials
//       3. Clears synaptic inputs
//       4. Resets firing states
//       5. Preserves population structure (IDs, type)
//
// @note Similar to initializeRandom() but preserves population identity
//       rather than recreating neurons
    void reset();
    
// Get neuron identifiers for all neurons in population
//
// @return std::vector<NeuronId> Vector of neuron identifiers
//
// @note Neuron IDs can be used to:
//       - Look up neurons in brain structures
//       - Create synapses to population members
//       - Track neuron identity across operations
//       - Analyze population composition
//
// @note Order corresponds to getNeurons() vector
    std::vector<NeuronId> getNeuronIds() const;

    // Record current population activity for statistics
//
// @note This method:
//       1. Captures current membrane potentials
//       2. Records firing states
//       3. Updates activity counters
//       4. Stores data for analysis
//
// @note Called periodically to collect:
//       - Statistical measures
//       - Performance metrics
//       - Learning progress
//       - Network health indicators
    void recordActivity();  // Record current state for statistics

    // Get mean membrane potential across population
//
// @return float Mean membrane potential in millivolts (mV)
//
// @note Useful for:
//       - Monitoring population health
//       - Detecting state changes
//       - Analyzing network dynamics
//       - Detecting synchrony
    float getMeanMembranePotential() const;

    // Get membrane potential variance across population
//
// @return float Variance of membrane potentials
//
// @note High variance may indicate:
//       - Diverse neural states
//       - Emergent dynamics
//       - Synchronization or desynchronization
//       - Network instability
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
