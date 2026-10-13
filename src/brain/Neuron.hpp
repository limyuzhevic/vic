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
     * @brief Create neuron with unique identifier
     * @param id Unique identifier for the neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor deleted (neurons are not copyable)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator deleted (neurons are not copyable)
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Neuron to move from
     * @return Reference to this neuron
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get neuron identifier
     * @return Neuron ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get neuron type
     * @return Neuron type (Excitatory, Inhibitory, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set neuron type
     * @param type Neuron type to set
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get complete neuron state
     * @return Reference to current neuron state
     * @note State is read-only access to internal state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * @return Reference to neuron state for modification
     * @warning Modifying state directly may cause inconsistent neuron behavior
     */
    NeuronState& getState();
    
    /**
     * @brief Get membrane potential
     * @return Membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to membrane potential
     * @param delta Change in membrane potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * @return Membrane potential threshold (mV) at which neuron fires
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * @param threshold New firing threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * @return true if neuron is in Active state, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return true if neuron cannot fire, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set neuron firing state
     * @param state New firing state (Resting, Active, Refractory, etc.)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     * @details Called by simulation framework to progress through refractory period
     */
    void decrementRefractory();
    
    /**
     * @brief Get firing rate
     * @return Current firing rate in Hertz (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance
     * @param conductance Leak conductance in nanoSiemens (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period
     * @return Remaining refractory period in steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential
     * @param potential Post-spike reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential exceeds threshold
     * @return true if neuron should fire, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * @return Time of last spike in simulation steps, or -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform Leaky Integrate-and-Fire dynamics
     * @param currentTime Current simulation time
     * @param dt Timestep duration in milliseconds
     * @return true if neuron fired during this timestep, false otherwise
     * @details Implements LIF equation: dV/dt = (V_rest - V)/τ + I/C
     *          Uses exponential Euler integration for numerical stability
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic input amplitude in mV
     * @details Adds excitatory current to membrane potential integration
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic input amplitude in mV
     * @details Subtracts inhibitory current from membrane potential integration
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * @param amplitude Modulatory input amplitude
     * @details Affects plasticity but not membrane potential directly
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * @param current Current amplitude to inject
     * @details Simulates external input from sensory or motor systems
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total synaptic input
     * @return Sum of all synaptic inputs this timestep
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear synaptic input
     * @details Resets synaptic input accumulator for next timestep
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record spike event
     * @param timestamp Time of spike occurrence
     * @details Stores spike time for synaptic plasticity and STDP
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history
     * @return Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @details Removes all recorded spike events
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse
     * @param handle Synapse handle to add
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse
     * @param handle Synapse handle to add
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * @return Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * @return Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * @return Reference to plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity flags
     * @return Reference to plasticity configuration for modification
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity rules
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region identifier
     * @param region Region ID where neuron is located
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region identifier
     * @return Region ID where neuron is located
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population identifier
     * @param population Population ID where neuron is located
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population identifier
     * @return Population ID where neuron is located
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step (deprecated)
     * @param currentTime Current simulation time
     * @deprecated Use stepLIF() instead. This method is kept for backward compatibility.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     * @details Restores all state variables to initialized values:
     *          - Membrane potential to resting potential
     *          - Firing state to Resting
     *          - Refractory period to 0
     *          - Spike history cleared
     *          - Synaptic input reset
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biological parameters
     * @param rng Random number generator for biological variability
     * @details Sets realistic neuron parameters based on type:
     *          - Membrane potential near resting potential
     *          - Threshold around -55mV with small variation
     *          - Resting potential around -70mV
     *          - Refractory period 2-10 steps
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Private implementation structure for Neuron
     * @details Internal implementation details to reduce header size and improve compilation times
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
