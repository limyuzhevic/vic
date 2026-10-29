#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

/**
 * @struct NeuronState
 * @brief Complete internal state representation for a neuron
 * 
 * This structure contains all the state variables needed to simulate a neuron's
 * integrate-and-fire behavior. All values are stored as float for efficient
 * memory access and computation.
 * 
 * @note Stored as contiguous memory for cache efficiency
 * @note Updated atomically during simulation steps
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
     * @brief Default constructor initializes neuron to typical biological parameters
     * 
     * Sets up a realistic neuron state with:
     * - Resting potential at -70mV (typical for cortical neurons)
     * - Threshold at -55mV (standard for action potential initiation)
     * - Refractory period of 5ms (standard recovery time)
     * - Zero synaptic input and adaptation
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
 * @class Neuron
 * @brief Core computational unit in the NLM artificial brain
 * 
 * This class implements a biologically-inspired Leaky Integrate-and-Fire (LIF) neuron model
 * that forms the fundamental building block of the Neural Learning Machine. Each neuron
 * maintains its own state, receives inputs from other neurons via synapses, and can
 * fire action potentials when its membrane potential reaches a threshold.
 * 
 * Key features:
 * - Dynamic membrane potential integration using LIF dynamics
 * - Spike-frequency adaptation through adaptation variable
 * - Absolute refractory period after each spike
 * - Multiple input modalities (excitatory, inhibitory, modulatory)
 * - Plasticity mechanisms (STDP, Hebbian learning)
 * - State management for learning and memory
 * 
 * The neuron follows the LIF equation:
 * dV/dt = (V_rest - V)/τ + I/C
 * where V is membrane potential, V_rest is resting potential, τ is time constant,
 * I is total current input, and C is membrane capacitance.
 * 
 * @note Uses the Pimpl idiom for efficient compilation and encapsulation
 * @note Thread-safe for concurrent access during simulation
 * @note Optimized for vectorization with cache-friendly memory layout
 */
class Neuron {
public:
    /**
     * @brief Constructs a new neuron with the specified identifier
     * 
     * @param id Unique identifier for this neuron (NeuronId)
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor - cleans up internal implementation
     */
    ~Neuron();
    
    /**
     * @brief Copy constructor deleted to prevent copying (Pimpl idiom)
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Copy assignment operator deleted to prevent copying
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of internal resources
     * 
     * @param other The neuron to move from (leaves in valid but unspecified state)
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of internal resources
     * 
     * @param other The neuron to move from (leaves in valid but unspecified state)
     * @return Reference to this neuron
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Retrieves the neuron's unique identifier
     * 
     * @return NeuronId The neuron's identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Gets the neuron's functional type
     * 
     * @return NeuronType The neuron's type (Excitatory, Inhibitory, Modulatory, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Sets the neuron's functional type
     * 
     * @param type The new neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Retrieves the complete neuron state
     * 
     * @return const NeuronState& Reference to the neuron's state (read-only)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Retrieves the complete neuron state for modification
     * 
     * @return NeuronState& Reference to the neuron's state
     */
    NeuronState& getState();
    
    /**
     * @brief Gets the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Sets the membrane potential directly
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adds to the membrane potential (e.g., from synaptic input)
     * 
     * @param delta Potential change in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Gets the firing threshold
     * 
     * @return MembranePotential Firing threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Sets the firing threshold
     * 
     * @param threshold New firing threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Checks if the neuron is currently firing
     * 
     * @return bool true if neuron is in Active state
     */
    bool isFiring() const;
    
    /**
     * @brief Checks if the neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire
     */
    bool isRefractory() const;
    
    /**
     * @brief Sets the neuron's firing state
     * 
     * @param state The new firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Sets the refractory period duration
     * 
     * @param steps Number of simulation steps the neuron will be refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrements refractory counter (call each simulation step)
     */
    void decrementRefractory();
    
    /**
     * @brief Gets the current firing rate
     * 
     * @return FiringRate Firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Sets the firing rate
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Sets the leak conductance parameter
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Gets the leak conductance parameter
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Gets the current refractory period
     * 
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Sets the resting potential
     * 
     * @param potential New resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Gets the resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Sets the reset potential (membrane potential after spike)
     * 
     * @param potential New reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Gets the reset potential
     * 
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Checks if membrane potential has reached firing threshold
     * 
     * @return bool true if neuron should fire
     */
    bool checkThreshold() const;
    
    /**
     * @brief Gets the timestamp of the last spike
     * 
     * @return float Timestamp of last spike in seconds (-1 if never spiked)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Executes one step of LIF dynamics
     * 
     * @param currentTime Current simulation time in seconds
     * @param dt Time step duration in seconds
     * @return bool true if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receives excitatory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives inhibitory synaptic input
     * 
     * @param amplitude Synaptic input amplitude in mV
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receives modulatory synaptic input (e.g., neuromodulators)
     * 
     * @param amplitude Modulatory input amplitude in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Injects external current (e.g., from sensory input)
     * 
     * @param current External current input in mV
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Gets the total current input for this step
     * 
     * @return MembranePotential Total synaptic + external current
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clears accumulated current input
     */
    void clearTotalCurrent();
    
    /**
     * @brief Records a spike timestamp for learning rules
     * 
     * @param timestamp Simulation time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Gets the spike history for learning and analysis
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clears the spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Adds a handle to an incoming synapse
     * 
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Adds a handle to an outgoing synapse
     * 
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Gets all incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Gets all outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Gets the plasticity flags for learning mechanisms
     * 
     * @return const PlasticityFlags& Reference to plasticity state
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Gets the plasticity flags for modification
     * 
     * @return PlasticityFlags& Reference to plasticity state
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enables plasticity mechanisms for this neuron
     * 
     * @param hebbian Enable Hebbian learning rule
     * @param stdp Enable spike-timing-dependent plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Sets the region identifier for the neuron
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Gets the neuron's region identifier
     * 
     * @return RegionId The neuron's region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Sets the population identifier for the neuron
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Gets the neuron's population identifier
     * 
     * @return PopulationId The neuron's population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Updates neuron for one simulation step
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initializes neuron with random biological parameters
     * 
     * @param rng Random number generator for parameter initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
};

} // namespace nlm
