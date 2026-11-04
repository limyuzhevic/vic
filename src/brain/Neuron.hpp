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
        // Phase 6: 9 steps, not 5. One action presentation is 10
        // brain steps, so a 5-step refractory let a neuron fire
        // twice inside a single burst; 9 steps caps it at about
        // one spike per presentation, which is the population's
        // natural burst rhythm anyway. This lowers the reported
        // rate without changing any drive constant (drive
        // scaling was tried and reverted: -30% drive silenced
        // the network entirely, 0 Hz, because this is a
        // threshold system with no gentle middle ground).
        , refractoryPeriod(9)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @class Neuron
 * @brief Represents a single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics
 * 
 * The Neuron class is the fundamental computational unit of the NLM brain simulation.
 * Each neuron maintains its own membrane potential state, responds to synaptic inputs,
 * and generates action potentials (spikes) when its membrane potential exceeds threshold.
 * 
 * The LIF model provides a biophysically plausible approximation of neuronal behavior
 * while remaining computationally efficient for large-scale simulations.
 * 
 * @note Neurons are organized into populations within regions, and maintain
 *       connections (synapses) with other neurons for information propagation.
 * 
 * @see Brain for integration with higher-level brain systems
 * @see Synapse for incoming and outgoing synaptic connections
 */
class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronId id);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique identifier
     * @return NeuronId The neuron's identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron's type classification
     * @return NeuronType The neuron's type (Sensory, Internal, Motor, Inhibitory)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type classification
     * @param type The neuron type to assign
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron's complete state
     * @return const NeuronState& Reference to the neuron's current state
     * @note State includes membrane potential, firing rate, spike history, etc.
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get mutable reference to neuron's state (for modification)
     * @return NeuronState& Mutable reference to state
     * @warning Modifying state directly is generally not recommended. Use proper API methods.
     */
    NeuronState& getState();
    
    /**
     * @brief Get the neuron's current membrane potential
     * @return MembranePotential Current membrane potential in millivolts (mV)
     * @see getRestingPotential() for the neuron's resting membrane potential
     * @see checkThreshold() to determine if neuron should fire
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the neuron's membrane potential directly
     * @param potential New membrane potential in millivolts (mV)
     * @warning This bypasses the normal integration process. Use with caution.
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add current to membrane potential (simulate incoming synaptic input)
     * @param delta Change in membrane potential in millivolts (mV)
     * @note Positive delta = excitatory input, negative delta = inhibitory input
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the neuron's firing threshold
     * @return MembranePotential Threshold voltage in millivolts (mV)
     * @see checkThreshold() to determine if neuron should fire based on current potential
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the neuron's firing threshold
     * @param threshold New threshold voltage in millivolts (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently in refractory period
     * @return bool True if neuron is refractory (cannot fire)
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the refractory period duration
     * @param steps Number of simulation steps neuron must remain refractory after firing
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter (called each simulation step)
     */
    void decrementRefractory();
    
    /**
     * @brief Get the neuron's current firing rate
     * @return FiringRate Firing rate in Hertz (spikes per second)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the neuron's firing rate directly
     * @param rate New firing rate in Hertz
     * @warning This bypasses the normal spike-based rate computation
     */
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if neuron should fire based on current membrane potential
     * @return bool True if membrane potential exceeds threshold
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get timestamp of last spike
     * @return float Timestamp of last spike in simulation time units (-1.0 if no spike)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Perform one LIF integration step - returns true if neuron fired
     * @param currentTime Current simulation time
     * @param dt Time step duration in simulation time units
     * @return bool True if neuron crossed threshold and fired during this step
     * @note This is the core neuron computation method called during each simulation step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Apply excitatory synaptic input to neuron
     * @param amplitude Synaptic input amplitude in millivolts (mV)
     * @note This represents post-synaptic potentials from excitatory synapses
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Apply inhibitory synaptic input to neuron
     * @param amplitude Synaptic input amplitude in millivolts (mV)
     * @note This represents post-synaptic potentials from inhibitory synapses
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Apply modulatory input (neuromodulators, etc.)
     * @param amplitude Modulatory input amplitude in millivolts (mV)
     * @note This represents inputs from neuromodulatory systems
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Apply external current input to neuron
     * @param current Current input in millivolts (mV)
     * @note This represents external driving inputs (sensory input, motor commands, etc.)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total accumulated current
     * @return MembranePotential Total current in millivolts (mV)
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear accumulated current
     * @note Should be called at the beginning of each simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp for plasticity computations
     * @param timestamp Simulation time when spike occurred
     * @note Used by STDP and other spike-timing dependent plasticity rules
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike timestamps for plasticity
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     * @note Used by STDP and other spike-timing dependent plasticity rules
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     * @note Used to reset plasticity state between experiments
     */
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
