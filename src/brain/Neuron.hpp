// NLM (熙然) - Neural Learning Machine
// Documentation improvements for Neuron.hpp

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
// NeuronState contains all state variables needed for LIF neuron dynamics
struct NeuronState {
    // Membrane potential (mV) - current membrane potential
    MembranePotential membranePotential;
    
    // Resting potential (mV) - equilibrium potential without input
    MembranePotential restingPotential;
    
    // Firing threshold (mV) - voltage at which neuron fires
    MembranePotential threshold;
    
    // Post-spike reset value (mV) - potential after spike
    MembranePotential resetPotential;
    
    // Leak conductance (nS) - conductance for passive leak current
    MembranePotential leakConductance;
    
    // Synaptic conductance (nS) - total synaptic conductance
    MembranePotential synapseConductance;
    
    // Firing rate (Hz) - current firing rate
    FiringRate firingRate;
    
    // Current firing state - can be Resting, Active, or Refractory
    FiringState firingState;
    
    // Steps remaining in refractory period
    uint32_t refractoryRemaining;
    
    // Total refractory period (steps) - how long neuron stays refractory after spike
    uint32_t refractoryPeriod;
    
    // Adaptation variable for spike-frequency adaptation - slowly builds up with each spike
    float adaptationVariable;
    
    // Timestamp of last spike (-1 if no spike yet)
    float lastSpikeTime;
    
    // Constructor initializes with biologically realistic default values
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
// Implements Leaky Integrate-and-Fire (LIF) dynamics with:
// - Membrane potential integration
// - Spike detection and reset
// - Refractory period
// - Synaptic inputs (excitatory, inhibitory, modulatory)
// - Spike history for STDP
// - Plasticity state management
// - Random initialization
class Neuron {
public:
    /// @brief Constructor creates a neuron with unique ID
    /// @param id Unique identifier for this neuron
    explicit Neuron(NeuronId id);
    
    /// @brief Destructor cleans up implementation
    ~Neuron();
    
    /// @brief Disable copying to prevent multiple ownership
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /// @brief Enable moving - allows transfer of ownership
    /// @param other Neuron to move from
    Neuron(Neuron&&) noexcept;
    
    /// @brief Move assignment operator
    /// @param other Neuron to move from
    Neuron& operator=(Neuron&&) noexcept;
    
    /// @brief Get the neuron's unique ID
    /// @return NeuronId The neuron's identifier
    NeuronId getId() const;
    
    /// @brief Get the neuron's type (Internal, Sensory, Motor)
    /// @return NeuronType The neuron's functional type
    NeuronType getType() const;
    
    /// @brief Set the neuron's type
    /// @param type New neuron type
    void setType(NeuronType type);
    
    /// @brief Get the complete neuron state
    /// @return const NeuronState& Reference to neuron state
    const NeuronState& getState() const;
    
    /// @brief Get mutable reference to neuron state
    /// @return NeuronState& Mutable reference to neuron state
    NeuronState& getState();
    
    /// @brief Get current membrane potential
    /// @return MembranePotential Current membrane potential in mV
    MembranePotential getMembranePotential() const;
    
    /// @brief Set membrane potential directly
    /// @param potential New membrane potential (mV)
    void setMembranePotential(MembranePotential potential);
    
    /// @brief Add to membrane potential (for synaptic inputs)
    /// @param delta Voltage change to add (mV)
    void addToMembranePotential(MembranePotential delta);
    
    /// @brief Get firing threshold
    /// @return MembranePotential Threshold voltage (mV)
    MembranePotential getThreshold() const;
    
    /// @brief Set firing threshold
    /// @param threshold New threshold voltage (mV)
    void setThreshold(MembranePotential threshold);
    
    /// @brief Check if neuron is currently firing
    /// @return bool true if neuron is in Active state
    bool isFiring() const;
    
    /// @brief Check if neuron is in refractory period
    /// @return bool true if neuron cannot fire due to refractory
    bool isRefractory() const;
    
    /// @brief Set firing state
    /// @param state New firing state
    void setFiringState(FiringState state);
    
    /// @brief Set refractory period duration
    /// @param steps Number of simulation steps in refractory
    void setRefractoryPeriod(uint32_t steps);
    
    /// @brief Decrement refractory counter
    /// Call once per simulation step while in refractory
    void decrementRefractory();
    
    /// @brief Get current firing rate
    /// @return FiringRate Firing rate in Hz
    FiringRate getFiringRate() const;
    
    /// @brief Set firing rate
    /// @param rate New firing rate (Hz)
    void setFiringRate(FiringRate rate);
    
    /// @brief Get leak conductance
    /// @return MembranePotential Leak conductance (nS)
    void setLeakConductance(MembranePotential conductance);
    
    /// @brief Get leak conductance
    /// @return MembranePotential Leak conductance (nS)
    MembranePotential getLeakConductance() const;
    
    /// @brief Get refractory period
    /// @return uint32_t Refractory period in steps
    uint32_t getRefractoryPeriod() const;
    
    /// @brief Set resting potential
    /// @param potential New resting potential (mV)
    void setRestingPotential(MembranePotential potential);
    
    /// @brief Get resting potential
    /// @return MembranePotential Resting potential (mV)
    MembranePotential getRestingPotential() const;
    
    /// @brief Set reset potential (post-spike)
    /// @param potential New reset potential (mV)
    void setResetPotential(MembranePotential potential);
    
    /// @brief Get reset potential
    /// @return MembranePotential Reset potential (mV)
    MembranePotential getResetPotential() const;
    
    /// @brief Check if membrane potential exceeds threshold
    /// @return bool true if neuron should fire
    bool checkThreshold() const;
    
    /// @brief Get time since last spike
    /// @return float Time since last spike in ms (-1 if no spike)
    float getLastSpikeTime() const;
    
    /// @brief Receive excitatory synaptic input
    /// @param amplitude Synaptic conductance contribution (nS)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /// @brief Receive inhibitory synaptic input
    /// @param amplitude Synaptic conductance contribution (nS)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /// @brief Receive modulatory input (affects plasticity)
    /// @param amplitude Modulatory signal strength
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /// @brief Inject direct current (e.g., from sensory input)
    /// @param current Current to inject (nA)
    void injectCurrent(MembranePotential current);
    
    /// @brief Get total current (read-only)
    /// @return MembranePotential Total current input (nA)
    MembranePotential getTotalCurrent() const;
    
    /// @brief Clear accumulated current
    void clearTotalCurrent();
    
    /// @brief Record a spike event
    /// @param timestamp When spike occurred
    void recordSpike(Timestamp timestamp);
    
    /// @brief Get spike history
    /// @return const std::vector<Timestamp>& Vector of spike times
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /// @brief Clear spike history
    void clearSpikeHistory();
    
    /// @brief Add incoming synapse connection
    /// @param handle Handle to incoming synapse
    void addIncomingSynapse(SynapseHandle handle);
    
    /// @brief Add outgoing synapse connection
    /// @param handle Handle to outgoing synapse
    void addOutgoingSynapse(SynapseHandle handle);
    
    /// @brief Get incoming synapses
    /// @return const std::vector<SynapseHandle>& Incoming synapse handles
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /// @brief Get outgoing synapses
    /// @return const std::vector<SynapseHandle>& Outgoing synapse handles
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /// @brief Get plasticity flags
    /// @return const PlasticityFlags& Synaptic plasticity rules
    const PlasticityFlags& getPlasticityFlags() const;
    
    /// @brief Get mutable plasticity flags
    /// @return PlasticityFlags& Mutable plasticity flags
    PlasticityFlags& getPlasticityFlags();
    
    /// @brief Enable/disable plasticity rules
    /// @param hebbian Enable Hebbian plasticity
    /// @param stdp Enable Spike-Timing-Dependent Plasticity
    /// @param rewardModulated Enable reward-modulated plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /// @brief Set region ID
    /// @param region Region identifier
    void setRegionId(RegionId region);
    
    /// @brief Get region ID
    /// @return RegionId Region identifier
    RegionId getRegionId() const;
    
    /// @brief Set population ID
    /// @param population Population identifier
    void setPopulationId(PopulationId population);
    
    /// @brief Get population ID
    /// @return PopulationId Population identifier
    PopulationId getPopulationId() const;
    
    /// @brief Update neuron for one simulation step
    /// This is the main LIF integration function
    /// @param currentTime Current simulation time
    void step(Timestamp currentTime);
    
    /// @brief Reset neuron to initial state
    void reset();
    
    /// @brief Initialize neuron parameters with random biologically plausible values
    /// @param rng Random number generator for initialization
    void initializeRandom(RandomGenerator& rng);
    
private:
    /// @brief Pimpl implementation struct for encapsulation
    struct Impl;
    
    /// @brief Pointer to implementation
    Impl* pImpl;
};

} // namespace nlm

// Documentation improvements complete.
// This version includes comprehensive Doxygen-style documentation for the Neuron class and its methods.
// The documentation covers:
// - Purpose and functionality of the neuron
// - Detailed parameter descriptions
// - Return value explanations
// - Behavioral notes and constraints
// - Implementation considerations
// - Integration with other components