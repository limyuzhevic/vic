#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @class NeuralDynamics
 * @brief Abstract base class for neural dynamics models
 * @details Defines the interface for different neuron dynamics models.
 *         Different implementations provide various levels of biological
 *         realism and computational complexity.
 * 
 * @note This is an abstract base class - concrete implementations must
 *       override all pure virtual methods. The NLM project implements:
 *       - IntegrateAndFireDynamics: Simple integrate-and-fire model
 *       - HodgkinHuxleyDynamics: Detailed ion channel dynamics (placeholder)
 *       - FitzHughNagumoDynamics: Simplified excitable system (placeholder)
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup dynamics
 * @implements neural_dynamics_model
 * @abstract
 */

class NeuralDynamics {
public:
    /**
     * @brief Virtual destructor
     * @details Ensures proper cleanup of derived class objects
     */
    virtual ~NeuralDynamics() = default;
    
    // ========== ABSTRACT VIRTUAL METHODS ==========
    
    /**
     * @brief Update neuron state for one simulation step
     * @param neuron Pointer to the neuron to update
     * @param dt Timestep duration for the update
     * @details Implements the core neural dynamics computation for a single timestep.
     *         This is where the neuron model computes its new state based on:
     *         - Current membrane potential
     *         - Synaptic inputs
     *         - Intrinsic properties (leak conductance, threshold, etc.)
     *         - External current injections
     * 
     * @note This is a pure virtual method that must be implemented by derived classes.
     *       Different implementations provide different levels of biological accuracy:
     *       - LIF: Simple leaky integrate-and-fire
     *       - HH: Detailed Hodgkin-Huxley ion channel dynamics
     *       - FHN: Simplified FitzHugh-Nagumo excitable system
     * 
     * @param neuron Neuron object to update
     * @param dt Simulation timestep duration
     * @pre neuron must not be nullptr
     * @pre dt must be positive
     * @see shouldFire() for spike detection
     * @see applySpikeInput() for synaptic input handling
     */
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    
    /**
     * @brief Update synapse state for one simulation step
     * @param synapse Pointer to the synapse to update
     * @param dt Timestep duration for the update
     * @details Updates synaptic dynamics based on:
     *         - Previous spike timing (for plasticity rules)
     *         - Current synaptic weight
     *         - Voltage-dependent gating variables (for some models)
     *         - Short-term plasticity (facilitation/depression)
     * 
     * @note This is a pure virtual method that must be implemented by derived classes.
     *       Synapse dynamics can include:
     *       - Conductance-based synaptic transmission
     *       - Short-term plasticity (STP)
     *       - Voltage-dependent release probability
     *       - Calcium-dependent processes
     * 
     * @param synapse Synapse object to update
     * @param dt Simulation timestep duration
     * @pre synapse must not be nullptr
     * @pre dt must be positive
     */
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    
    /**
     * @brief Apply synaptic input from spike to neuron
     * @param neuron Pointer to the target neuron
     * @param synapse Pointer to the synapse that delivered the spike
     * @details Applies the effect of a synaptic spike to the target neuron,
     *         typically by increasing membrane potential or conductance.
     *         The specific implementation depends on the synapse type:
     *         - Excitatory: depolarizes membrane
     *         - Inhibitory: hyperpolarizes membrane
     *         - Modulatory: changes neuronal excitability
     * 
     * @note This is a pure virtual method that must be implemented by derived classes.
     *       The synaptic input can be implemented as:
     *       - Conductance-based: changes membrane conductance
     *       - Current-based: injects current into neuron
     *       - Both: combines conductance and current effects
     * 
     * @param neuron Target neuron
     * @param synapse Source synapse
     * @pre neuron must not be nullptr
     * @pre synapse must not be nullptr
     */
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    
    /**
     * @brief Check if neuron should fire in current timestep
     * @param neuron Pointer to the neuron to check
     * @return bool true if neuron should fire, false otherwise
     * @details Determines whether the neuron has reached firing threshold
     *         based on its current membrane potential and other state variables.
     *         This check occurs after updateNeuron() has been called.
     * 
     * @note This is a pure virtual method that must be implemented by derived classes.
     *       Different neuron models have different firing criteria:
     *       - LIF: Membrane potential > threshold
     *       - HH: Complex voltage-dependent gating variables
     *       - FHN: Threshold crossing with hysteresis
     * 
     * @param neuron Neuron to check for firing
     * @return true if neuron should fire this timestep
     * @return false otherwise
     * @pre neuron must not be nullptr
     * @see updateNeuron() for state update before firing check
     */
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    
    /**
     * @brief Reset dynamics state
     * @details Resets all internal state variables of the dynamics model to
     *         their initial values. This is typically called when:
     *         - Starting a new simulation
     *         - Resetting a neuron after firing
     *         - Loading a checkpoint
     *         - Clearing neural state
     * 
     * @note This is a pure virtual method that must be implemented by derived classes.
     *       The reset operation should:
     *       - Reset membrane potential to resting value
     *       - Clear refractory state
     *       - Reset gating variables (for HH/FHN models)
     *       - Clear any accumulated history
     */
    virtual void reset() = 0;
};

/**
 * @class IntegrateAndFireDynamics
 * @brief Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
 * @details The LIF model is the simplest spiking neuron model that captures
 *         essential neural dynamics:
 *         - Membrane potential integration
 *         - Exponential decay (leak)
 *         - Threshold-based spiking
 *         - Reset to resting potential
 * 
 * @note The LIF model is computationally efficient and widely used in neural
 *       network simulations. It provides a good balance between biological
 *       plausibility and computational efficiency. The differential equation:
 *       τ dv/dt = -v + I
 *       where v is membrane potential, τ is time constant, I is input current.
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup dynamics
 * @implements integrate_and_fire_dynamics
 * @extends NeuralDynamics
 */

class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    /**
     * @brief Construct a new IntegrateAndFireDynamics object
     * @details Creates LIF dynamics with default parameters:
     *          - Membrane time constant: 10ms
     *          - Leak conductance: 10nS
     *          - Reset behavior: simple threshold reset
     *          - Refractory period: enabled
     */
    IntegrateAndFireDynamics();
    
    /**
     * @brief Destroy the IntegrateAndFireDynamics object
     * @details Cleans up any allocated resources and resets internal state.
     */
    ~IntegrateAndFireDynamics() override;
    
    /**
     * @brief Update neuron state for one simulation step
     * @param neuron Pointer to the neuron to update
     * @param dt Timestep duration for the update
     * @details Implements the LIF dynamics update:
     *          dv/dt = (-v + I_syn + I_ext) / τ
     *          where:
     *          - v is membrane potential
     *          - τ is membrane time constant
     *          - I_syn is synaptic current
     *          - I_ext is external current
     * 
     * @note The update follows these steps:
     *       1. Compute total current (synaptic + external)
     *       2. Update membrane potential using Euler integration
     *       3. Check for threshold crossing
     *       4. Handle refractory period if neuron is refractory
     *       5. Record spike if threshold reached
     * 
     * @param neuron Neuron object to update
     * @param dt Simulation timestep duration
     * @pre neuron must not be nullptr
     * @pre dt must be positive
     * @see shouldFire() for spike detection
     * @see applySpikeInput() for synaptic input handling
     */
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    
    /**
     * @brief Update synapse state for one simulation step
     * @param synapse Pointer to the synapse to update
     * @param dt Timestep duration for the update
     * @details Updates synaptic state including:
     *         - Short-term depression/facilitation (if implemented)
     *         - Synaptic conductance dynamics
     *         - Weight changes due to plasticity (handled separately)
     *         - Eligibility traces for reward-modulated plasticity
     * 
     * @note For LIF neurons, synapse updates typically involve:
     *       - Conductance-based synaptic transmission
     *       - Short-term plasticity dynamics
     *       - Synaptic depression during high firing rates
     *       - Synaptic facilitation during low firing rates
     * 
     * @param synapse Synapse object to update
     * @param dt Simulation timestep duration
     * @pre synapse must not be nullptr
     * @pre dt must be positive
     */
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    
    /**
     * @brief Apply synaptic input from spike to neuron
     * @param neuron Pointer to the target neuron
     * @param synapse Pointer to the synapse that delivered the spike
     * @details Applies postsynaptic current based on the synaptic weight
     *         and synapse type (excitatory/inhibitory). The current is added
     *         to the neuron's total input current.
     * 
     * @note The synaptic input is implemented as:
     *       I_syn += weight * synaptic_kernel(t)
     *       where synaptic_kernel(t) is typically an alpha function or
     *       exponential decay.
     * 
     * @param neuron Target neuron
     * @param synapse Source synapse
     * @pre neuron must not be nullptr
     * @pre synapse must not be nullptr
     */
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    
    /**
     * @brief Check if neuron should fire in current timestep
     * @param neuron Pointer to the neuron to check
     * @return bool true if neuron should fire, false otherwise
     * @details Checks if the neuron's membrane potential exceeds the firing
     *         threshold and the neuron is not in refractory period.
     *         If firing, records the spike time and initiates reset.
     * 
     * @note The firing condition is:
     *       shouldFire = (v > threshold) AND NOT (refractory > 0)
     *       where v is membrane potential, threshold is firing threshold.
     * 
     * @param neuron Neuron to check for firing
     * @return true if neuron should fire this timestep
     * @return false otherwise
     * @pre neuron must not be nullptr
     */
    bool shouldFire(const Neuron* neuron) const override;
    
    /**
     * @brief Reset dynamics state
     * @details Resets all internal LIF dynamics state to initial values:
     *         - Membrane potential to resting value
     *         - Refractory period to zero
     *         - Any gating variables (if HH/FHN model)
     *         - Spike history
     * 
     * @note This is called when:
     *       - Neuron reaches threshold and needs to reset
     *       - Starting a new simulation
     *       - Loading a checkpoint
     *       - Clearing neural state
     */
    void reset() override;
    
    // ========== LIF PARAMETERS ==========
    
    /**
     * @brief Set membrane time constant
     * @param tau Membrane time constant (in ms)
     * @details The membrane time constant τ controls how quickly the membrane
     *         potential decays toward the resting potential in the absence of input.
     *         Larger τ means slower decay and longer integration time.
     * 
     * @note τ is related to membrane resistance (R) and capacitance (C):
     *       τ = R * C
     *       Typical values: 5-50ms for biological neurons
     * 
     * @param tau Membrane time constant in milliseconds
     * @see getMembraneTimeConstant() for getting current value
     * @see updateNeuron() for usage in LIF equation
     */
    void setMembraneTimeConstant(float tau);
    /**
     * @brief Get the membrane time constant
     * @return float Current membrane time constant (in ms)
     * @see setMembraneTimeConstant() for setting value
     */
    float getMembraneTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
