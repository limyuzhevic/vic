#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @brief Neuron state structure for efficient storage
 * 
 * Contains all state variables needed for Leaky Integrate-and-Fire neuron dynamics.
 * All potentials are in millivolts (mV), times in milliseconds (ms),
 * conductances in nanosiemens (nS), and rates in hertz (Hz).
 */
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Initialize neuron state with biologically plausible defaults
     * 
     * Sets resting potential to -70mV, threshold to -55mV, and other
     * parameters to typical values for spiking neurons.
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

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics
class Neuron {
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    /**
     * @brief LIF step function with exponential Euler integration
     * 
     * Implements the Leaky Integrate-and-Fire neuron dynamics using
     * exponential Euler method for better stability. Updates membrane
     * potential based on synaptic input, leak current, and adaptation.
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Time step in seconds for integration
     * @return true if neuron spiked/fired during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Update neuron state with configurable timestep
     * 
     * Convenience wrapper around stepLIF that uses the default 1ms timestep
     * when no timestep is specified. Equivalent to step(currentTime, 0.001s).
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Handle incoming excitatory synaptic input
     * 
     * Adds excitatory post-synaptic potential to neuron's synaptic input.
     * The amplitude represents synaptic conductance multiplied by
     * the driving force.
     * 
     * @param amplitude Excitatory input amplitude in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Handle incoming inhibitory synaptic input
     * 
     * Subtracts inhibitory post-synaptic potential from neuron's synaptic input.
     * Inhibitory inputs hyperpolarize the neuron, reducing firing probability.
     * 
     * @param amplitude Inhibitory input amplitude in mV
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Handle incoming modulatory synaptic input
     * 
     * Modulatory inputs affect neuronal plasticity but not direct integration.
     * Used for neuromodulators like dopamine, acetylcholine, etc.
     * 
     * @param amplitude Modulatory input amplitude in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (e.g., from sensory input)
     * 
     * Direct current injection from external sources adds to synaptic input
     * for LIF integration. Used by sensory processing and external drives.
     * 
     * @param current External current in nA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total synaptic input current
     * 
     * @return Total current from all synaptic inputs in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic input
     * 
     * Resets synaptic input for next integration step.
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event timestamp
     * 
     * Adds timestamp to spike history for STDP and other plasticity rules.
     * Implements refractory storage with max history size.
     * 
     * @param timestamp Time when spike occurred in seconds
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history for plasticity computation
     * 
     * @return Vector of timestamps for recent spikes
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * 
     * Removes all spike records from history.
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * 
     * @param handle Handle to synapse providing input to this neuron
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * 
     * @param handle Handle to synapse through which this neuron sends output
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapses
     * 
     * @return Vector of synapse handles receiving from this neuron
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapses
     * 
     * @return Vector of synapse handles originating from this neuron
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity configuration flags
     * 
     * @return Reference to plasticity flags structure
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration flags
     * 
     * @return Reference to plasticity flags structure (for modification)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian synaptic strengthening
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region membership for this neuron
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region identifier
     * 
     * @return Region identifier containing this neuron
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population membership within a region
     * 
     * @param population Population identifier within region
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population identifier
     * 
     * @return Population identifier for this neuron
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Reset neuron to initial state
     * 
     * Clears all accumulated state and resets to defaults.
     */
    void reset();
    
    /**
     * @brief Calculate and update internal scaling factors for simulation
     * 
     * Converts physical time units (seconds) to simulation steps and applies
     * appropriate scaling for numerical integration. Uses the simulation
     * timestep configured in the brain system.
     * 
     * @param currentTime Current time in seconds
     * @param simulationTimestep Time step used for integration (seconds)
     * @return Timestep in milliseconds for use in LIF calculations
     */
    TimestepDuration calculateScaledTimestep(Timestamp currentTime, TimestepDuration simulationTimestep) const;
    
    /**
     * @brief Check for and handle neuron spike events
     * 
     * Processes neuron state for spike detection, recording spike events,
     * and managing refractory periods. Called by step() after state update.
     * 
     * @param currentTime Current simulation time
     * @return true if neuron fired, false otherwise
     */
    bool checkAndHandleSpike(Timestamp currentTime);
    
    /**
     * @brief Update synaptic efficacy based on recent neural activity
     * 
     * Modulates synaptic efficacy based on recent spike history using
     * activity-dependent plasticity rules. Essential for learning and
     * maintaining appropriate synaptic strengths.
     * 
     * @param currentTime Current simulation time
     */
    void updateSynapticEfficacy(Timestamp currentTime);
    
    /**
     * @brief Apply spike-frequency adaptation mechanisms
     * 
     * Implements spike-timing dependent adaptation that reduces neuronal
     * excitability following high-frequency firing. Contributes to stable
     * firing patterns and prevents runaway excitation.
     * 
     * @param currentTime Current simulation time
     */
    void applyAdaptation(Timestamp currentTime);
    
    /**
     * @brief Update short-term plasticity state
     * 
     * Models activity-dependent changes in synaptic strength that occur
     * on fast timescales (milliseconds to seconds). Implements depression
     * and facilitation dynamics using the Tsodyks-Markram model.
     * 
     * @param currentTime Current simulation time
     */
    void updateShortTermPlasticity(Timestamp currentTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
