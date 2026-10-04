#pragma once

/**
 * @file Neuron.hpp
 * @brief Core neuron interface implementing Leaky Integrate-and-Fire (LIF) dynamics
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full spiking dynamics implementation planned for Phase 2.
 */

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

// Forward declarations
class Synapse;
class RandomGenerator;
class Brain;

/**
 * @class NeuronStepProcessor
 * @brief Processes individual neurons and regions for one simulation step
 * @details Contains the low-level functions that update neuron states, handle spikes,
 *          and manage synaptic transmission. These are the core computational
 *          primitives used by the simulation step coordinator.
 * @note Static class with no instance state. Used internally by SimulationStepCoordinator.
 */

class NeuronStepProcessor {
public:
    /**
     * @brief Processes a single neuron for one LIF integration step
     * @param neuron Pointer to the neuron to process
     * @param currentTime The current simulation time (seconds)
     * @param dt The time step duration (seconds)
     * @return bool true if the neuron fired (crossed threshold), false otherwise
     * @details Updates the neuron's membrane potential using the Leaky
     *          Integrate-and-Fire (LIF) dynamics equation: dV/dt = (V_rest - V)/tau + I/C
     * @note Handles refractory periods, spike detection, and reset.
     */
    static bool processNeuronStep(Neuron* neuron, Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Processes all neurons in a specific region
     * @param brain Pointer to the parent Brain instance
     * @param regionId ID of the region to process
     * @param currentTime The current simulation time (seconds)
     * @param dt The time step duration (seconds)
     * @details Iterates through all populations in the region and updates each neuron.
     *          Used by SimulationStepCoordinator::executeStep2().
     */
    static void processRegionNeurons(Brain* brain, RegionId regionId, 
                                      Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Processes spike events and their effects
     * @param brain Pointer to the parent Brain instance
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Processes all neurons that fired during this step, scheduling
     *          immediate and delayed spike events and updating spike history.
     * @note Called by SimulationStepCoordinator::executeStep3().
     */
    static void processSpikes(Brain* brain, SimulationStep currentStep, 
                              Timestamp currentTime);

    /**
     * @brief Delivers synaptic input from delayed spike events
     * @param brain Pointer to the parent Brain instance
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Processes all delayed spike events that are now ready to deliver
     *          their synaptic input to postsynaptic neurons.
     * @note Called by SimulationStepCoordinator::executeStep1().
     */
    static void deliverDelayedSpikes(Brain* brain, SimulationStep currentStep, 
                                      Timestamp currentTime);
};
