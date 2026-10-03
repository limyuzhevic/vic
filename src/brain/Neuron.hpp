#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
/// 
/// Contains all internal state variables for a single LIF neuron.
/// Initialized with biologically plausible default values.
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
    
    NeuronState()
        : membranePotential(-70.0f)           ///< Initial membrane potential
        , restingPotential(-70.0f)           ///< Initial resting potential
        , threshold(-55.0f)                  ///< Initial firing threshold  
        , resetPotential(-70.0f)             ///< Initial reset potential
        , leakConductance(10.0f)              ///< Initial leak conductance
        , synapseConductance(0.0f)            ///< Initial synaptic conductance
        , firingRate(0.0f)                    ///< Initial firing rate
        , firingState(FiringState::Resting)   ///< Initial firing state
        , refractoryRemaining(0)              ///< Not in refractory initially
        , refractoryPeriod(5)                 ///< Standard 5ms refractory period
        , adaptationVariable(0.0f)            ///< No adaptation initially
        , lastSpikeTime(-1.0f)               ///< No previous spikes
    {}
};

// Neuron class representing a single neuron
///
/// Implements Leaky Integrate-and-Fire (LIF) neural dynamics with:
/// - Membrane potential integration with exponential decay
/// - Spike detection and refractory periods
/// - Synaptic input integration (excitatory, inhibitory, modulatory)
/// - Spike-frequency adaptation
/// - Random initialization with biological constraints
///
/// The neuron operates according to: dV/dt = (V_rest - V)/tau + I/C
/// where V is membrane potential, V_rest is resting potential,
/// tau is time constant, and I/C is synaptic current contribution.
class Neuron {
public:
    /// Create neuron with unique ID
    ///
    /// @param id Unique identifier for this neuron
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    /// Disable copying (neurons are expensive to copy)
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /// Enable moving (for efficient transfer between containers)
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity and metadata
    /// Get neuron identifier
    ///
    /// @return Unique neuron ID
    NeuronId getId() const;
    
    /// Get neuron type classification
    ///
    /// @return Neuron type (Internal, Excitatory, Inhibitory, Sensory, Motor, Modulatory)
    NeuronType getType() const;
    
    /// Set neuron type classification
    ///
    /// @param type New neuron type
    void setType(NeuronType type);
    
    // State access
    /// Get neuron state (read-only access)
    ///
    /// @return Constant reference to neuron state
    const NeuronState& getState() const;
    
    /// Get neuron state (read-write access for internal modifications)
    ///
    /// @return Mutable reference to neuron state
    NeuronState& getState();
    
    // Membrane potential management
    /// Get current membrane potential
    ///
    /// @return Membrane potential in mV
    MembranePotential getMembranePotential() const;
    
    /// Set membrane potential directly
    ///
    /// @note Use setMembranePotential for guaranteed bounds checking
    /// @param potential New membrane potential (will be clamped to -100..+50mV)
    void setMembranePotential(MembranePotential potential);
    
    /// Add delta to membrane potential (for synaptic integration)
    ///
    /// @param delta Change in membrane potential (mV)
    void addToMembranePotential(MembranePotential delta);
    
    // Firing threshold management
    /// Get firing threshold potential
    ///
    /// @return Threshold in mV (typically -55mV)
    MembranePotential getThreshold() const;
    
    /// Set firing threshold
    ///
    /// @param threshold New threshold (will be clamped to -100..-40mV)
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    /// Check if neuron is currently firing
    ///
    /// @return True if neuron has spiked this cycle
    bool isFiring() const;
    
    /// Check if neuron is in refractory period
    ///
    /// @return True if neuron cannot fire until refractory period ends
    bool isRefractory() const;
    
    /// Set firing state directly
    ///
    /// @param state New firing state
    void setFiringState(FiringState state);
    
    /// Set refractory period duration
    ///
    /// @param steps Number of simulation steps neuron cannot fire
    void setRefractoryPeriod(uint32_t steps);
    
    /// Decrement refractory period counter (called each simulation step)
    void decrementRefractory();
    
    // Firing rate management
    /// Get current firing rate (Hz)
    ///
    /// @return Firing rate in spikes per second
    FiringRate getFiringRate() const;
    
    /// Set firing rate directly
    ///
    /// @param rate New firing rate (Hz)
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    /// Set leak conductance (affects membrane time constant)
    ///
    /// @param conductance Leak conductance in nS
    void setLeakConductance(MembranePotential conductance);
    
    /// Get leak conductance
    ///
    /// @return Leak conductance in nS
    MembranePotential getLeakConductance() const;
    
    /// Get refractory period duration
    ///
    /// @return Refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    
    /// Set resting potential
    ///
    /// @param potential Resting potential in mV (clamped to -90..-50mV)
    void setRestingPotential(MembranePotential potential);
    
    /// Get resting potential
    ///
    /// @return Resting potential in mV
    MembranePotential getRestingPotential() const;
    
    /// Set reset potential (post-spike reset value)
    ///
    /// @param potential Reset potential in mV (clamped to -90..-40mV)
    void setResetPotential(MembranePotential potential);
    
// Spike detection
    /// Check if membrane potential exceeds firing threshold
    ///
    /// @return True if neuron would fire this step
    bool checkThreshold() const;
    
    /// Get timestamp of last spike
    ///
    /// @return Last spike time, or -1.0 if never spiked
    float getLastSpikeTime() const;
    
    // LIF dynamics
    /// Perform one step of LIF integration
    ///
    /// Implements the differential equation: dV/dt = (V_rest - V)/tau + I/C
    /// where V is membrane potential, V_rest is resting potential,
    /// tau is time constant, and I/C is synaptic current contribution.
    ///
    /// @param currentTime Current simulation time (ms)
    /// @param dt Time step for integration (s)
    /// @return True if neuron fired (crossed threshold), false otherwise
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Synaptic input management
    /// Receive excitatory synaptic input (depolarizing)
    ///
    /// @param amplitude Synaptic current contribution in mV
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /// Receive inhibitory synaptic input (hyperpolarizing)
    ///
    /// @param amplitude Synaptic current contribution in mV
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /// Receive modulatory synaptic input (affects plasticity, not firing)
    ///
    /// @param amplitude Modulatory current contribution in mV
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /// Get total synaptic current for current step
    ///
    /// @return Total synaptic current in mV
    MembranePotential getTotalCurrent() const;
    
    /// Clear synaptic input (called at end of each integration step)
    void clearTotalCurrent();
    
    // Spike history (for STDP)
    /// Record a spike timestamp (called when neuron fires)
    ///
    /// @param timestamp Time when spike occurred (ms)
    void recordSpike(Timestamp timestamp);
    
    /// Get recent spike history for STDP calculations
    ///
    /// @return Vector of recent spike timestamps (ms)
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /// Clear all spike history
    void clearSpikeHistory();
    
    // Synapse management
    /// Add incoming synapse (post-synaptic)
    ///
    /// @param handle Synapse handle for incoming connection
    void addIncomingSynapse(SynapseHandle handle);
    
    /// Add outgoing synapse (pre-synaptic)
    ///
    /// @param handle Synapse handle for outgoing connection
    void addOutgoingSynapse(SynapseHandle handle);
    
    /// Get list of incoming synapses
    ///
    /// @return Vector of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /// Get list of outgoing synapses
    ///
    /// @return Vector of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    /// Get plasticity flags (read-only)
    ///
    /// @return Plasticity flags for Hebbian, STDP, reward-modulated learning
    const PlasticityFlags& getPlasticityFlags() const;
    
    /// Get plasticity flags (read-write access for modifications)
    ///
    /// @return Mutable reference to plasticity flags
    PlasticityFlags& getPlasticityFlags();
    
    /// Enable/disable plasticity mechanisms
    ///
    /// @param hebbian Enable Hebbian learning
    /// @param stdp Enable Spike-Timing Dependent Plasticity
    /// @param rewardModulated Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Neural connectivity
    /// Set region identifier for this neuron
    ///
    /// @param region Region ID this neuron belongs to
    void setRegionId(RegionId region);
    
    /// Get region identifier
    ///
    /// @return Region ID this neuron belongs to
    RegionId getRegionId() const;
    
    /// Set population identifier for this neuron
    ///
    /// @param population Population ID this neuron belongs to
    void setPopulationId(PopulationId population);
    
    /// Get population identifier
    ///
    /// @return Population ID this neuron belongs to
    PopulationId getPopulationId() const;
    
    // Simulation control
    /// Update neuron for one simulation step (default implementation)
    ///
    /// @note TODO PHASE 2: Implement real integrate-and-fire dynamics
    /// @param currentTime Current simulation time (ms)
    void step(Timestamp currentTime);
    
    /// Reset neuron to initial state
    void reset();
    
    /// Initialize neuron with random biological parameters
    ///
    /// @param rng Random number generator for parameter variability
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
