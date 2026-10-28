/**
 * @file Neuron.hpp
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * @author NLM Team
 * @date 2026
 * @version Phase 2
 *
 * This file implements a single artificial neuron using the Leaky Integrate-and-Fire model,
 * which is a standard model in computational neuroscience for simulating neuronal dynamics.
 */

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * @brief Neuron state structure containing all membrane potential and firing parameters
 *
 * This structure holds the dynamic and static parameters of the neuron, including
 * membrane potential, threshold, refractory state, and adaptation variables.
 */
struct NeuronState {
    MembranePotential membranePotential;    ///< Current membrane potential (mV)
    MembranePotential restingPotential;    ///< Resting potential (mV)
    MembranePotential threshold;           ///< Firing threshold (mV)
    MembranePotential resetPotential;       ///< Post-spike reset value (mV)
    MembranePotential leakConductance;      ///< Leak conductance (nS)
    MembranePotential synapseConductance;   ///< Synaptic conductance (nS)
    FiringRate firingRate;                 ///< Current firing rate (Hz)
    FiringState firingState;               ///< Current firing state
    uint32_t refractoryRemaining;          ///< Steps remaining in refractory period
    uint32_t refractoryPeriod;             ///< Total refractory period (steps)
    float adaptationVariable;              ///< For spike-frequency adaptation
    float lastSpikeTime;                   ///< Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializing neuron to resting state
     *
     * Sets typical biological parameters for an integrate-and-fire neuron:
     * - Resting potential: -70mV
     * - Threshold: -55mV
     * - Reset potential: -70mV
     * - Refractory period: 5 steps
     */
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @brief Neuron class representing a single artificial neuron
 * @details Implements Leaky Integrate-and-Fire (LIF) dynamics with support for:
 * - Event-driven spike generation
 * - Synaptic integration (excitatory, inhibitory, modulatory)
 * - Short-term plasticity state
 * - Refractory period management
 * - Spike-frequency adaptation
 * - Real-time dynamics stepping with variable timesteps
 *
 * The neuron follows the LIF equation:
 * dV/dt = (V_rest - V)/tau + I/C
 * where V is membrane potential, tau is time constant, I is synaptic input, C is capacitance
 */
class Neuron {
public:
    /**
     * @brief Constructs a neuron with a unique identifier
     * @param id Unique identifier for this neuron (required for debugging and tracking)
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - releases implementation pointer
     */
    ~Neuron();
    
    /**
     * @brief Disables copying (neurons cannot be shared via copy)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disables copy assignment (neurons cannot be shared via assignment)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enables move construction (transfers ownership of implementation)
     * @param other Neuron to move from (will be left in valid but unspecified state)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Enables move assignment (transfers ownership of implementation)
     * @param other Neuron to move from
     * @return Reference to this neuron (for chaining)
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Gets the neuron's unique identifier
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's type (internal, sensory, motor, etc.)
     * @return NeuronType The neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's type
     * @param type The neuron type to set
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the neuron's current state
     * @return const NeuronState& Reference to internal state (read-only)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets the neuron's current state (mutable)
     * @return NeuronState& Reference to internal state (modifiable)
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds a delta to the membrane potential (in-place)
     * @param delta Potential change in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold
     * @return MembranePotential Threshold potential in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold
     * @param threshold Threshold potential in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently firing (membrane potential >= threshold)
     * @return bool True if neuron is firing, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is in refractory period
     * @return bool True if neuron is refractory, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state
     * @param state The firing state to set
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Decrements the refractory period counter
     * @details Called during simulation step, updates firing state when refractory period ends
     */
    void decrementRefractory();
    
    /**
     * @brief Sets the refractory period duration
     * @param steps Number of simulation steps the neuron will be refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Gets the refractory period duration
     * @return uint32_t Remaining refractory period in steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (membrane potential after spike)
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Gets the reset potential
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Checks if the neuron has reached firing threshold
     * @return bool True if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the timestamp of the last spike
     * @return float Last spike timestamp (-1.0f if no spikes occurred)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Receives excitatory synaptic input
     * @param amplitude Synaptic conductance × reversal potential contribution
     * @details Excitatory input increases membrane potential through synaptic currents
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * @param amplitude Synaptic conductance × reversal potential contribution
     * @details Inhibitory input decreases membrane potential through hyperpolarization
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory input (neuromodulators like dopamine, acetylcholine)
     * @param amplitude Neuromodulatory signal strength
     * @details Modulatory input affects plasticity and adaptation but not direct integration
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects current directly into the neuron (from sensory or external sources)
     * @param current Input current in pA (picoamperes)
     * @details Direct current injection bypasses synaptic dynamics and adds directly to membrane potential
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets total synaptic input accumulated since last step
     * @return MembranePotential Total synaptic current input
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears accumulated synaptic input
     * @details Resets synaptic input buffer for next simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike event with timestamp
     * @param timestamp Simulation timestamp when spike occurred
     * @details Updates spike history and triggers plasticity updates
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets recent spike history (for STDP calculations)
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @details Maintains rolling window of recent spikes (max MAX_SPIKE_HISTORY)
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears spike history
     * @details Resets spike timestamp buffer
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds an incoming synapse to this neuron
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds an outgoing synapse from this neuron
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets all incoming synapse handles
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets all outgoing synapse handles
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets plasticity flags controlling which plasticity rules are active
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets plasticity flags (mutable)
     * @return PlasticityFlags& Mutable plasticity configuration
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables/disables specific plasticity rules
     * @param hebbian Enable Hebbian plasticity (SSRN)
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the region ID this neuron belongs to
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the region ID this neuron belongs to
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the population ID this neuron belongs to
     * @param population Population identifier within the region
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the population ID this neuron belongs to
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Performs a single LIF simulation step
     * @param currentTime Current simulation timestamp
     * @param dt Simulation timestep duration in ms
     * @return bool True if neuron fired during this step
     * @details Implements the core LIF dynamics:
     * dV/dt = (V_rest - V)/tau + I/C
     * Uses exponential Euler integration for stability
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Updates neuron for one simulation step (with default timestep)
     * @param currentTime Current simulation timestamp
     * @details Calls stepLIF with standard 1ms timestep
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets neuron to initial resting state
     * @details Clears all internal state, resets membrane potential, spike history
     */
    void reset();
    
    /**
     * @brief Initializes neuron with random biological parameters
     * @param rng Random number generator for stochastic initialization
     * @details Sets biologically plausible parameters: resting potential, threshold,
     * reset potential, refractory period, etc. with small random variations
     */
    void initializeRandom(RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
