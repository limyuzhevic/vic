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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with adaptive excitability
// This is the fundamental computational unit of the NLM brain
class Neuron {
public:
    /// @brief Create a new neuron with a unique identifier
    /// @param id Unique identifier for this neuron
    explicit Neuron(NeuronId id);
    
    /// @brief Destroy the neuron and clean up resources
    ~Neuron();
    
    /// @brief Disable copy construction (neurons cannot be copied)
    Neuron(const Neuron&) = delete;
    
    /// @brief Disable copy assignment (neurons cannot be copied)
    Neuron& operator=(const Neuron&) = delete;
    
    /// @brief Enable move construction (transfer ownership of pImpl)
    /// @param other Neuron to move from (will be left in valid but empty state)
    Neuron(Neuron&&) noexcept;
    
    /// @brief Enable move assignment (transfer ownership of pImpl)
    /// @param other Neuron to move from (will be left in valid but empty state)
    /// @return Reference to this neuron
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity and basic properties
    /// @brief Get the neuron's unique identifier
    /// @return NeuronId The unique identifier
    NeuronId getId() const;
    
    /// @brief Get the neuron's type (excitatory, inhibitory, sensory, etc.)
    /// @return NeuronType The neuron's functional type
    NeuronType getType() const;
    
    /// @brief Set the neuron's type
    /// @param type The new neuron type
    void setType(NeuronType type);
    
    // State access methods
    /// @brief Get the neuron's complete state structure
    /// @return Const reference to NeuronState structure
    const NeuronState& getState() const;
    
    /// @brief Get mutable access to the neuron's state structure
    /// @return Mutable reference to NeuronState structure
    NeuronState& getState();
    
    // Membrane potential management
    /// @brief Get the current membrane potential
    /// @return Current membrane potential in millivolts (mV)
    MembranePotential getMembranePotential() const;
    
    /// @brief Set the membrane potential to an exact value
    /// @param potential Membrane potential in mV
    void setMembranePotential(MembranePotential potential);
    
    /// @brief Add a delta to the current membrane potential
    /// @param delta Potential change in mV (can be positive or negative)
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold management
    /// @brief Get the neuron's firing threshold
    /// @return Firing threshold in mV
    MembranePotential getThreshold() const;
    
    /// @brief Set the neuron's firing threshold
    /// @param threshold Firing threshold in mV
    void setThreshold(MembranePotential threshold);
    
    // Firing state management
    /// @brief Check if neuron is currently firing (active)
    /// @return true if neuron's firing state is Active
    bool isFiring() const;
    
    /// @brief Check if neuron is in refractory period
    /// @return true if neuron is in Refractory state
    bool isRefractory() const;
    
    /// @brief Set the neuron's firing state directly
    /// @param state The new firing state
    void setFiringState(FiringState state);
    
    /// @brief Set the refractory period duration
    /// @param steps Number of simulation steps in refractory period
    void setRefractoryPeriod(uint32_t steps);
    
    /// @brief Decrement refractory counter (called each simulation step)
    void decrementRefractory();
    
    // Firing rate management (for rate-based computations)
    /// @brief Get the neuron's current firing rate
    /// @return Firing rate in Hertz (Hz)
    FiringRate getFiringRate() const;
    
    /// @brief Set the neuron's firing rate
    /// @param rate Firing rate in Hz
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameter management
    /// @brief Set the leak conductance parameter
    /// @param conductance Leak conductance in nanoSiemens (nS)
    void setLeakConductance(MembranePotential conductance);
    
    /// @brief Get the leak conductance parameter
    /// @return Leak conductance in nS
    MembranePotential getLeakConductance() const;
    
    /// @brief Get the refractory period duration
    /// @return Refractory period in simulation steps
    uint32_t getRefractoryPeriod() const;
    
    /// @brief Set the resting potential
    /// @param potential Resting potential in mV
    void setRestingPotential(MembranePotential potential);
    
    /// @brief Get the resting potential
    /// @return Resting potential in mV
    MembranePotential getRestingPotential() const;
    
    /// @brief Set the reset potential (after spike)
    /// @param potential Reset potential in mV
    void setResetPotential(MembranePotential potential);
    
    /// @brief Get the reset potential
    /// @return Reset potential in mV
    MembranePotential getResetPotential() const;
    
    // Spike detection and history
    /// @brief Check if neuron has reached firing threshold
    /// @return true if membrane potential >= threshold
    bool checkThreshold() const;
    
    /// @brief Get the timestamp of the most recent spike
    /// @return Timestamp of last spike in seconds, or -1.0 if never spiked
    float getLastSpikeTime() const;
    
    /// @brief Execute one step of LIF dynamics
    /// @param currentTime Current simulation time in seconds
    /// @param dt Timestep duration in seconds
    /// @return true if neuron fired during this step
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Input signal handling
    /// @brief Receive excitatory synaptic input
    /// @param amplitude Input amplitude in mV (positive for excitation)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /// @brief Receive inhibitory synaptic input
    /// @param amplitude Input amplitude in mV (negative for inhibition)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /// @brief Receive modulatory input (affects plasticity, not firing)
    /// @param amplitude Modulation strength in mV
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /// @brief Inject external current (e.g., from sensory input)
    /// @param current Current injection in mV
    void injectCurrent(MembranePotential current);
    
    /// @brief Get the total synaptic current for this timestep
    /// @return Total synaptic current in mV (cached value)
    MembranePotential getTotalCurrent() const;
    
    /// @brief Clear accumulated synaptic input (called at end of timestep)
    void clearTotalCurrent();
    
    // Spike history management for STDP
    /// @brief Record a new spike event
    /// @param timestamp Time when spike occurred
    void recordSpike(Timestamp timestamp);
    
    /// @brief Get the spike history vector
    /// @return Const reference to vector of spike timestamps
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /// @brief Clear all spike history records
    void clearSpikeHistory();
    
    // Synapse management
    /// @brief Add an incoming synapse to this neuron
    /// @param handle Unique identifier for the synapse
    void addIncomingSynapse(SynapseHandle handle);
    
    /// @brief Add an outgoing synapse from this neuron
    /// @param handle Unique identifier for the synapse
    void addOutgoingSynapse(SynapseHandle handle);
    
    /// @brief Get list of incoming synapses
    /// @return Const reference to vector of incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /// @brief Get list of outgoing synapses
    /// @return Const reference to vector of outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state management
    /// @brief Get the neuron's plasticity flags
    /// @return Const reference to PlasticityFlags structure
    const PlasticityFlags& getPlasticityFlags() const;
    
    /// @brief Get mutable access to plasticity flags
    /// @return Mutable reference to PlasticityFlags structure
    PlasticityFlags& getPlasticityFlags();
    
    /// @brief Enable specific plasticity rules for this neuron
    /// @param hebbian Enable Hebbian plasticity
    /// @param stdp Enable STDP plasticity
    /// @param rewardModulated Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Regional and population membership
    /// @brief Set the neuron's region identifier
    /// @param region RegionId where this neuron resides
    void setRegionId(RegionId region);
    
    /// @brief Get the neuron's region identifier
    /// @return RegionId of the containing region
    RegionId getRegionId() const;
    
    /// @brief Set the neuron's population identifier
    /// @param population PopulationId within the region
    void setPopulationId(PopulationId population);
    
    /// @brief Get the neuron's population identifier
    /// @return PopulationId within the region
    PopulationId getPopulationId() const;
    
    // Simulation lifecycle
    /// @brief Execute one complete simulation step
    /// @param currentTime Current simulation time in seconds
    /// @note Default implementation uses standard timestep (1ms)
    void step(Timestamp currentTime);
    
    /// @brief Reset neuron to its initial state
    void reset();
    
    /// @brief Initialize neuron with random biological parameters
    /// @param rng Random number generator for parameter variability
    void initializeRandom(RandomGenerator& rng);
    
private:
    /// @brief Forward declaration of implementation struct (Pimpl pattern)
    struct Impl;
    
    /// @brief Pointer to implementation (encapsulates private data)
    Impl* pImpl;
};

} // namespace nlm
