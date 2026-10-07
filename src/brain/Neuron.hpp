#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
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
    /**
     * @class Neuron
     * @brief Implements a Leaky Integrate-and-Fire (LIF) neuron model
     * 
     * This class represents a single biological neuron with realistic dynamics including:
     * - Leaky integration of membrane potential
     * - Threshold-based spike generation
     * - Refractory period after spiking
     * - Synaptic input integration (excitatory, inhibitory, modulatory)
     * - Current injection from external sources
     * - Spike-frequency adaptation
     * - STDP-compatible spike recording
     * - Synapse management (incoming and outgoing connections)
     * - Plasticity control (Hebbian, STDP, reward-modulated)
     * - Region/population membership for distributed processing
     * 
     * The neuron follows the LIF differential equation:
     * dV/dt = (V_rest - V)/τ + I/C
     * 
     * Where:
     * V = membrane potential
     * V_rest = resting potential
     * τ = membrane time constant
     * I = total synaptic current
     * C = membrane capacitance
     * 
     * @note This implementation uses the PIMPL pattern for encapsulation
     * @note All operations are thread-safe for concurrent access
     * @note Memory-efficient design with fixed-size internal structures
     */
public:
    // Create neuron with ID
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    **/
     * @brief Construct a neuron with a specific ID
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the neuron and clean up resources
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (neurons cannot be copied)
     * 
     * Neurons are unique entities with their own state and cannot be copied
     * to prevent incorrect state sharing. Use move semantics instead.
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable move construction (neurons can be moved)
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Enable move assignment (neurons can be moved)
     * @param other Neuron to move from
     * @return Reference to this neuron
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type
     * @return NeuronType The neuron's type
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state
     * @return const NeuronState& Reference to the neuron's state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron's complete state (non-const version)
     * @return NeuronState& Reference to the neuron's state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * @return MembranePotential Current membrane potential (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * @param potential New membrane potential (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the membrane potential
     * @param delta Change in membrane potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * @return MembranePotential Threshold potential (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * @param threshold New threshold potential (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * @return bool True if neuron is in active firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return bool True if neuron cannot spike
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * @post If refractory period reaches zero, neuron becomes available to fire
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * @param rate New firing rate (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance parameter
     * @param conductance Leak conductance (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance parameter
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * @param potential New resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (post-spike value)
     * @param potential New reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential exceeds threshold
     * @return bool True if neuron should spike
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * @return float Last spike timestamp, or -1.0 if no spikes yet
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Update neuron state for one simulation step with configurable timestep
     * 
     * This is the core LIF neuron update function. It implements:
     * 1. Refractory period handling
     * 2. Leaky integration of membrane potential
     * 3. Synaptic input integration
     * 4. Spike-frequency adaptation
     * 5. Spike threshold detection and reset
     * 6. Refractory period initiation
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Simulation timestep (ms)
     * @return bool True if neuron fired (spiked) during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Update neuron for one simulation step with default timestep
     * 
     * Convenience method that uses the standard 1ms timestep for backward
     * compatibility. For new code, use step(Timestamp, TimestepDuration).
     * 
     * @param currentTime Current simulation time (ms)
     */
    void step(Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Update neuron for one simulation step (backward compatibility)
     * @param currentTime Current simulation time (ms)
     */
    void step(Timestamp currentTime);

    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic conductance contribution (nS * mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic conductance contribution (nS * mV)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Modulatory conductance contribution (nS * mV)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject current from external source
     * @param current External current injection (nA)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current input for this step
     * @return MembranePotential Total current input (nA)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic input
     * @post Synaptic input is reset to zero for next step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for STDP calculations
     * @param timestamp Time when spike occurred (ms)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @post All spike timestamps are removed
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse connections
     * @return const std::vector<SynapseHandle>& List of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse connections
     * @return const std::vector<SynapseHandle>& List of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (non-const version)
     * @return PlasticityFlags& Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the neuron's region ID
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neuron's region ID
     * @return RegionId The neuron's region
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the neuron's population ID
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the neuron's population ID
     * @return PopulationId The neuron's population
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Reset neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * @param rng Random number generator for stochastic initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief PIMPL implementation detail
     * @note Neuron implementation is hidden in the pImpl idiom for encapsulation
     */
    struct Impl;
    Impl* pImpl;
};
};

} // namespace nlm
