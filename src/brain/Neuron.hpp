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
 * Contains all state variables needed for LIF neuron dynamics.
 * Organized for cache efficiency and SIMD optimization.
 * This structure holds the complete internal state of a neuron.
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
     * @brief Default constructor initializes neuron to resting state
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
 * @brief Represents a single neuron implementing Leaky Integrate-and-Fire dynamics
 * 
 * The Neuron class models a biophysical neuron with the following characteristics:
 * - Membrane potential integration with exponential decay
 * - Threshold-based spike generation with refractory period
 * - Synaptic input integration (excitatory, inhibitory, modulatory)
 * - Spike-frequency adaptation
 * - Connectivity management (incoming/outgoing synapses)
 * - Plasticity flag management for STDP, Hebbian, and reward-modulated learning
 * - Random parameter initialization for biologically plausible variability
 * 
 * The neuron follows the LIF equation: dV/dt = (V_rest - V)/τ + I/C
 * where τ is the membrane time constant and C is capacitance.
 * 
 * @note Uses PIMPL idiom for efficient memory usage and forward declarations
 * @see Neuron.cpp for implementation details
 * @see Synapse.hpp for synaptic connections
 */
class Neuron {
public:
    /**
     * @brief Creates a new neuron with specified ID
     * 
     * @param id Unique identifier for the neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (neurons are not copyable)
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Enable moving (neurons can be moved efficiently)
     */
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (Excitatory, Inhibitory, etc.)
     * 
     * @return NeuronType The neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's current state
     * 
     * @return const NeuronState& Reference to internal state (non-modifying)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron's current state (mutable)
     * 
     * @return NeuronState& Reference to internal state (may modify)
     */
    NeuronState& getState();
    
    /**
     * @brief Get the membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * 
     * @param potential New membrane potential (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the membrane potential
     * 
     * @param delta Potential change (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * 
     * @return MembranePotential Threshold potential (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * 
     * @param threshold New threshold potential (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool True if neuron is in Active firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool True if neuron cannot fire this step
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the neuron's firing state
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called automatically by step function)
     */
    void decrementRefractory();
    
    /**
     * @brief Get the neuron's firing rate
     * 
     * @return FiringRate Current firing rate (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the neuron's firing rate
     * 
     * @param rate New firing rate (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance (membrane time constant)
     * 
     * @param conductance Leak conductance (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * 
     * @return MembranePotential Leak conductance (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * 
     * @param potential Resting potential (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * 
     * @return MembranePotential Resting potential (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (post-spike voltage)
     * 
     * @param potential Reset potential (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential has reached threshold
     * 
     * @return bool True if neuron would spike this step
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of last spike
     * 
     * @return float Timestamp of last spike (-1.0 if never spiked)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform LIF step function with real-time integration
     * 
     * Implements the actual integrate-and-fire computation for one simulation timestep.
     * Updates membrane potential based on:
     * - Synaptic inputs
     * - Leak dynamics
     * - Spike-frequency adaptation
     * - Refractory period
     * 
     * @param currentTime Current simulation time (s)
     * @param dt Timestep duration (s)
     * @return bool True if neuron fired this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic conductance contribution (nS * mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic conductance contribution (nS * mV, positive value)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * Modulatory inputs affect plasticity but not directly integrated into membrane potential.
     * Commonly used for neuromodulators like dopamine, acetylcholine, etc.
     * 
     * @param amplitude Modulatory input strength
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (e.g., from sensory input)
     * 
     * @param current External current injection (nS * mV)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current input for current timestep
     * 
     * @return MembranePotential Total synaptic + external current
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated synaptic current (called at step end)
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for plasticity calculations
     * 
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get list of incoming synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Handles of incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get list of outgoing synapse connections
     * 
     * @return const std::vector<SynapseHandle>& Handles of outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity configuration flags
     * 
     * @return const PlasticityFlags& Reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable plasticity configuration flags
     * 
     * @return PlasticityFlags& Reference to plasticity flags (may modify)
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable/disable plasticity rules
     * 
     * @param hebbian Enable Hebbian (rate-based) plasticity
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region membership for spatial organization
     * 
     * @param region Region ID where this neuron resides
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region membership
     * 
     * @return RegionId Region ID containing this neuron
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population membership for functional organization
     * 
     * @param population Population ID for functional grouping
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population membership
     * 
     * @return PopulationId Population ID for this neuron
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron state for one simulation step
     * 
     * Wrapper around stepLIF() with standard timestep (1ms).
     * This is the main update function called by the brain simulation loop.
     * 
     * @param currentTime Current simulation time (s)
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial resting state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biologically plausible parameters
     * 
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief PIMPL idiom implementation details
     * 
     * Pointer-to-implementation pattern for efficient compilation and forward declarations
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
