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
 * This structure holds all the state variables needed for Leaky Integrate-and-Fire
 * neuron dynamics. Using a separate struct allows for efficient memory access patterns
 * and reduces memory overhead compared to storing individual variables.
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
     * @brief Default constructor initializes neuron to resting state
     * 
     * Sets all state variables to biologically plausible default values
     * that represent a healthy, mature neuron at rest.
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
 * @brief Neuron class representing a single neuron
 * 
 * Implements Leaky Integrate-and-Fire (LIF) dynamics for biologically plausible
 * neural computation. This class uses the Pimpl idiom to hide implementation
 * details and provide a clean interface for neural simulation.
 * 
 * Key features:
 * - Event-driven spike propagation
 * - Synaptic plasticity (STDP, Hebbian, reward-modulated)
 * - Developmental capabilities
 * - Integration with neuromodulation systems
 * - Support for multiple neuron types (excitatory, inhibitory, modulatory)
 */
class Neuron {
public:
    /**
     * @brief Constructor creates neuron with specified ID
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor cleans up neuron resources
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor is deleted (neurons are unique objects)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor enables efficient transfer of neuron ownership
     * @param other Neuron to move from (becomes invalid after move)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator enables efficient transfer of neuron ownership
     * @param other Neuron to move from (becomes invalid after assignment)
     * @return Reference to this neuron after move operation
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Gets the unique identifier of this neuron
     * @return NeuronId of this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron type (excitatory, inhibitory, modulatory, etc.)
     * @return Current neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron type
     * @param type New neuron type to assign
     */
    void setType(NeuronType type);
    
    /**
     * @brief Gets the current neuron state (const version)
     * @return Const reference to neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Gets the current neuron state (non-const version)
     * @return Non-const reference to neuron state for modification
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * @return Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * @param potential New membrane potential value in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds to the membrane potential (for synaptic input)
     * @param delta Change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold potential
     * @return Threshold potential in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold
     * @param threshold New threshold potential in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if neuron is currently firing
     * @return true if neuron is in active firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if neuron is in refractory period
     * @return true if neuron cannot fire due to refractory period
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the firing state of the neuron
     * @param state New firing state (Resting, Active, Refractory, etc.)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * @param steps Number of simulation steps for refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements refractory period counter
     * @note Called each simulation step during refractory period
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the current firing rate
     * @return Firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate (for rate-based computation)
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the leak conductance parameter
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance parameter
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the current refractory period duration
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (voltage after spike)
     * @param potential New reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Checks if membrane potential has exceeded firing threshold
     * @return true if neuron should fire
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the time of last spike
     * @return Timestamp of last spike (-1 if no spikes yet)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief LIF step function - integrates dynamics and detects spikes
     * @param currentTime Current simulation time
     * @param dt Time step duration
     * @return true if neuron fired during this time step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input
     * @param amplitude Synaptic conductance change in nS
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * @param amplitude Synaptic conductance change in nS (negative values)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input
     * @param amplitude Modulatory conductance change in nS
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects external current into neuron
     * @param current Current in nA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets the total injected current for this simulation step
     * @return Total injected current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears the injected current accumulator
     * @note Called at beginning of each simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike timestamp for plasticity computations
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the history of recent spikes
     * @return Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds incoming synapse reference to this neuron
     * @param handle Handle to the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds outgoing synapse reference from this neuron
     * @param handle Handle to the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets all incoming synapse references
     * @return Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets all outgoing synapse references
     * @return Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets the plasticity flags for this neuron
     * @return Const reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets the plasticity flags for this neuron (non-const version)
     * @return Non-const reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables specific plasticity rules for this neuron
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the region ID of this neuron
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the region ID of this neuron
     * @return Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the population ID of this neuron
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the population ID of this neuron
     * @return Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Updates neuron for one simulation step
     * 
     * This is the main update function that should be called each simulation step.
     * The implementation details are hidden in the Pimpl structure to maintain
     * clean interface separation.
     * 
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets neuron to initial resting state
     */
    void reset();
    
    /**
     * @brief Initializes neuron with random biologically plausible parameters
     * @param rng Random number generator for stochastic initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Pimpl implementation detail structure
     * 
     * This structure holds all the implementation details of the Neuron class,
     * including the actual LIF computation, synaptic integration, and plasticity
     * state management. This separation allows for better encapsulation and
     * easier maintenance of the complex neural dynamics code.
     */
    struct Impl;
    Impl* pImpl;  // Pointer to implementation
};

} // namespace nlm

/**
 * @brief Create a new Neuron with specified ID
 * @param id Unique identifier for the neuron
 * @return Shared pointer to the newly created neuron
 */
inline std::shared_ptr<Neuron> createNeuron(NeuronId id) {
    return std::make_shared<Neuron>(id);
}
