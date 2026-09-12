// Brain utility functions - common operations and statistics
// 
// This module provides utility functions for common brain operations and statistics
// that are needed across different brain systems. These utilities provide a clean
// interface for accessing brain properties and statistics without exposing internal
// implementation details.
#pragma once

#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * @class BrainUtils
 * @brief Provides utility functions for common brain operations
 * 
 * The BrainUtils class provides utility functions for common brain operations and
 * statistics that are needed across different brain systems. These utilities provide
 * a clean interface for accessing brain properties and statistics without exposing
 * internal implementation details.
 * 
 * @details This class implements utility functions for the integrated brain architecture:
 * - Statistical queries: spikes, neurons, synapses, firing rates
 * - Action production: Generate motor actions based on neural activity
 * - Neuromodulation: Apply neuromodulatory signals
 * - Random generation: Access to random number generator
 * 
 * @note Utility functions provide a convenient interface for common operations
 * without exposing the complexity of brain system internals.
 */
class BrainUtils {
public:
    /**
     * @brief Get excitation/inhibition ratio across all synapses
     * 
     * This method returns the excitation/inhibition ratio of the brain's neural network,
     * which indicates the balance between excitatory and inhibitory synaptic connections.
     * This ratio is important for network stability and dynamics.
     * 
     * @param brain Pointer to Brain instance
     * @return E/I ratio (higher = more excitation, lower = more inhibition)
     * 
     * @pre brain must not be null
     * @post Excitation/inhibition ratio returned
     */
    static float getExcitationInhibitionRatio(Brain* brain);
    
    /**
     * @brief Get total spike count across all neurons
     * 
     * This method returns the total number of spikes fired by all neurons in the brain
     * during the current simulation and throughout the brain's lifetime. Spike count
     * is a measure of overall neural activity.
     * 
     * @param brain Pointer to Brain instance
     * @return Total spike count
     * 
     * @pre brain must not be null
     * @post Total spike count returned
     */
    static size_t getTotalSpikeCount(Brain* brain);
    
    /**
     * @brief Get number of pending spike events
     * 
     * This method returns the number of spike events that are pending delivery,
     * including both immediate and delayed spike events. Pending spike count indicates
     * the amount of future neural activity scheduled.
     * 
     * @param brain Pointer to Brain instance
     * @return Number of pending spike events
     * 
     * @pre brain must not be null
     * @post Pending spike event count returned
     */
    static size_t getPendingSpikeEventCount(Brain* brain);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * This method returns the total number of neurons in the brain across all neural
     * regions. This is a fundamental statistic for brain size and computational capacity.
     * 
     * @param brain Pointer to Brain instance
     * @return Total neuron count
     * 
     * @pre brain must not be null
     * @post Total neuron count returned
     */
    static size_t getTotalNeuronCount(Brain* brain);
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * This method returns the total number of synapses in the brain across all neural
     * regions. Synapse count indicates connectivity and computational complexity.
     * 
     * @param brain Pointer to Brain instance
     * @return Total synapse count
     * 
     * @pre brain must not be null
     * @post Total synapse count returned
     */
    static size_t getTotalSynapseCount(Brain* brain);
    
    /**
     * @brief Get active neuron count
     * 
     * This method returns the number of currently active neurons in the brain.
     * Active neurons represent ongoing neural processing and computation.
     * 
     * @param brain Pointer to Brain instance
     * @return Active neuron count
     * 
     * @pre brain must not be null
     * @post Active neuron count returned
     */
    static size_t getActiveNeuronCount(Brain* brain);
    
    /**
     * @brief Get firing neuron count this step
     * 
     * This method returns the number of neurons that fired in the current simulation
     * step. Firing neuron count indicates recent neural activity and computation.
     * 
     * @param brain Pointer to Brain instance
     * @return Firing neuron count (number of neurons that fired this step)
     * 
     * @pre brain must not be null
     * @post Firing neuron count returned
     */
    static size_t getFiringNeuronCount(Brain* brain);
    
    /**
     * @brief Get average firing rate
     * 
     * This method returns the average firing rate of neurons in the brain,
     * calculated as total spikes divided by number of neurons and simulation time.
     * Average firing rate indicates overall network activity levels.
     * 
     * @param brain Pointer to Brain instance
     * @return Average firing rate (Hz, spikes per second per neuron)
     * 
     * @pre brain must not be null
     * @post Average firing rate returned
     */
    static float getAverageFiringRate(Brain* brain);
    
    /**
     * @brief Produce motor action based on neural activity
     * 
     * This method produces a motor action based on the current neural activity patterns
     * in the brain's motor neuron populations. Actions are generated based on the
     * level of motor neuron activation and neural decision-making processes.
     * 
     * @param brain Pointer to Brain instance
     * @return Action to produce
     * 
     * @pre brain must not be null
     * @post Action produced based on neural activity
     */
    static std::unique_ptr<Action> produceAction(Brain* brain);
    
    /**
     * @brief Apply neuromodulation signal
     * 
     * This method applies a neuromodulatory signal to the brain, influencing neural
     * excitability, plasticity, and behavior based on the neuromodulator's level
     * and type (dopamine, curiosity, novelty, prediction error).
     * 
     * @param brain Pointer to Brain instance
     * @param neuromodulator Neuromodulator signal to apply
     * 
     * @pre brain must not be null, neuromodulator must be valid
     * @post Neuromodulation applied to brain
     */
    static void applyNeuromodulation(Brain* brain, const Neuromodulator& neuromodulator);
    
    /**
     * @brief Get current random generator
     * 
     * This method returns the brain's random number generator, which is used for
     * stochastic processes, random connectivity initialization, and other
     * probabilistic computations.
     * 
     * @param brain Pointer to Brain instance
     * @return Random generator pointer (nullptr if not available)
     * 
     * @pre brain must not be null
     * @post Random generator returned
     */
    static RandomGenerator* getRandomGenerator(Brain* brain);
};

} // namespace nlm