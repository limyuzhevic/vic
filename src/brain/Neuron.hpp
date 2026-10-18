#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
/**
 * @brief Complete neuron state representation for Leaky Integrate-and-Fire model
 * 
 * Contains all necessary state variables for LIF neuron dynamics. Structured
 * for memory efficiency with all fields stored contiguously.
 * 
 * The state includes:
 * - Membrane potential and related electrical properties
 * - Firing state and timing information
 * - Adaptation mechanisms for spike-frequency adaptation
 * - Refractory period tracking
 */
struct NeuronState {
    /** @brief Current membrane potential (mV) */
    MembranePotential membranePotential;    // Current membrane potential (mV)
    /** @brief Resting membrane potential (mV) */
    MembranePotential restingPotential;    // Resting potential (mV)
    /** @brief Firing threshold (mV) */
    MembranePotential threshold;           // Firing threshold (mV)
    /** @brief Post-spike reset value (mV) */
    MembranePotential resetPotential;      // Post-spike reset value (mV)
    /** @brief Leak conductance (nS) */
    MembranePotential leakConductance;      // Leak conductance (nS)
    /** @brief Synaptic conductance (nS) */
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    /** @brief Current firing rate (Hz) */
    FiringRate firingRate;                 // Current firing rate (Hz)
    /** @brief Current firing state */
    FiringState firingState;               // Current firing state
    /** @brief Steps remaining in refractory period */
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    /** @brief Total refractory period (steps) */
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    /** @brief Adaptation variable for spike-frequency adaptation */
    float adaptationVariable;              // For spike-frequency adaptation
    /** @brief Timestamp of last spike (-1 if none) */
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor with initialized values
     * 
     * Sets typical biological parameters for an integrate-and-fire neuron.
     * Resting potential at -70mV, threshold at -55mV, etc.
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
/**
 * @brief Leaky Integrate-and-Fire (LIF) neuron implementation
 * 
 * Represents a single biological neuron with simplified LIF dynamics.
 * This is the fundamental computational unit of the NLM brain.
 * 
 * Key features:
 * - Leaky integrate-and-fire dynamics with biological realism
 * - Event-driven spike generation and propagation
 * - Synaptic integration from excitatory/inhibitory/modulatory inputs
 * - Spike-frequency adaptation mechanisms
 * - Refractory period modeling
 * - Memory of recent spike activity for plasticity
 * - Strong typing for safety and clear abstractions
 * 
 * The neuron implements the mathematical model:
 * dV/dt = (V_rest - V)/τ + I/C
 * where V is membrane potential, τ is time constant, I is synaptic current, C is capacitance
 * 
 * @note This is a simplified model, not biologically accurate, but captures
 *       essential dynamics for artificial developmental brain research.
 */
class Neuron {
public:
    /**
     * @brief Construct a neuron with unique identifier
     * 
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     */
    ~Neuron();
    
    /**
     * @brief Disable copying (Pimpl idiom)
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * 
     * @return NeuronId Unique identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type (sensory, motor, internal, etc.)
     * 
     * @return NeuronType Type of neuron
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * 
     * @param type New neuron type
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the complete neuron state
     * 
     * @return const NeuronState& Reference to current state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron state
     * 
     * @return NeuronState& Mutable reference to current state
     */
    NeuronState& getState();
    
    /**
     * @brief Get current membrane potential
     * 
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set membrane potential directly
     * 
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add value to membrane potential
     * 
     * @param delta Change in membrane potential (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get firing threshold
     * 
     * @return MembranePotential Threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set firing threshold
     * 
     * @param threshold New threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * 
     * @return bool true if neuron is in Active state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * 
     * @return bool true if neuron cannot fire this step
     */
    bool isRefractory() const;
    
    /**
     * @brief Set firing state
     * 
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set refractory period duration
     * 
     * @param steps Number of simulation steps in refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory period counter
     */
    void decrementRefractory();
    
    /**
     * @brief Get current firing rate
     * 
     * @return FiringRate Firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set firing rate directly
     * 
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set leak conductance
     * 
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get leak conductance
     * 
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get refractory period duration
     * 
     * @return uint32_t Refractory period in steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set resting potential
     * 
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get resting potential
     * 
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set reset potential
     * 
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron has reached firing threshold
     * 
     * @return bool true if membrane potential >= threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * 
     * @return float Timestamp of last spike, -1 if never spiked
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step
     * 
     * @param currentTime Current simulation time (ms)
     * @param dt Timestep duration (ms)
     * @return bool true if neuron fired this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * 
     * @param amplitude Synaptic current amplitude (mV)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input
     * 
     * @param amplitude Modulatory input amplitude (mV)
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (e.g., from sensory input)
     * 
     * @param current Current amplitude (mV)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total current input
     * 
     * @return MembranePotential Total synaptic current
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear synaptic current input
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp
     * 
     * @param timestamp Time of spike (ms)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get spike history
     * 
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse to this neuron
     * 
     * @param handle Synapse handle
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse from this neuron
     * 
     * @param handle Synapse handle
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Incoming synapses
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * 
     * @return const std::vector<SynapseHandle>& Outgoing synapses
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags
     * 
     * @return const PlasticityFlags& Current plasticity settings
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get mutable reference to plasticity flags
     * 
     * @return PlasticityFlags& Mutable reference to plasticity settings
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * 
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set region identifier
     * 
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get region identifier
     * 
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set population identifier
     * 
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get population identifier
     * 
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * 
     * @param currentTime Current simulation time (ms)
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random biologically plausible parameters
     * 
     * @param rng Random number generator for stochastic initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Private implementation details (Pimpl idiom)
     */
    struct Impl;
    /**
     * @brief Pointer to implementation for forward declaration
     */
    Impl* pImpl;
};

} // namespace nlm
