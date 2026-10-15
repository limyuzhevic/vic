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
     * @brief Construct a new Neuron object with a unique identifier
     * 
     * @param id Unique identifier for this neuron (typically auto-incremented)
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron object and clean up internal resources
     */
    ~Neuron();
    
    // Disable copying, enable moving (RAII pattern)
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * 
     * @return NeuronId The neuron's identifier
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the type/classification of this neuron
     * 
     * @return NeuronType The neuron's type (Excitatory, Inhibitory, Sensory, etc.)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the type/classification of this neuron
     * 
     * @param type The neuron type to assign
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the current state of the neuron (const access)
     * 
     * @return const NeuronState& Reference to the neuron's state structure
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the current state of the neuron (non-const access)
     * 
     * @return NeuronState& Non-const reference for state modification
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * 
     * @return MembranePotential Current membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential directly
     * 
     * @param potential New membrane potential in millivolts (mV)
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Adjust the membrane potential by adding a delta value
     * 
     * @param delta Change in membrane potential in millivolts (mV)
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold potential
     * 
     * @return MembranePotential The threshold potential in millivolts (mV)
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold potential
     * 
     * @param threshold New threshold potential in millivolts (mV)
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (above threshold)
     * 
     * @return true if neuron membrane potential >= threshold
     * @return false if neuron is below threshold
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * 
     * @return true if neuron cannot fire (refractory period active)
     * @return false if neuron can fire
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * 
     * @param state The firing state to set (Resting, Active, Refractory, etc.)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the duration of the refractory period
     * 
     * @param steps Number of simulation steps the neuron should remain refractory
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the remaining refractory time by one step
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate of the neuron
     * 
     * @return FiringRate Current firing rate in Hertz (Hz)
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate of the neuron
     * 
     * @param rate New firing rate in Hertz (Hz)
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance parameter for LIF dynamics
     * 
     * @param conductance Leak conductance in nanoSiemens (nS)
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance parameter
     * 
     * @return MembranePotential Leak conductance in nanoSiemens (nS)
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * 
     * @return uint32_t Total refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting membrane potential
     * 
     * @param potential Resting potential in millivolts (mV)
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting membrane potential
     * 
     * @return MembranePotential Resting potential in millivolts (mV)
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential after spike (usually near resting)
     * 
     * @param potential Reset potential in millivolts (mV)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if the neuron membrane potential exceeds the firing threshold
     * 
     * @return true if neuron will fire (potential >= threshold)
     * @return false if neuron will not fire
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * 
     * @return float Timestamp of last spike, or -1.0 if no spikes recorded
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute one step of Leaky Integrate-and-Fire dynamics
     * 
     * @param currentTime Current simulation timestamp in milliseconds
     * @param dt Timestep duration in seconds
     * @return bool true if neuron fired during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input (depolarizing)
     * 
     * @param amplitude Synaptic input strength (positive current)
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input (hyperpolarizing)
     * 
     * @param amplitude Synaptic input strength (negative current)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory synaptic input (affects plasticity, not voltage)
     * 
     * @param amplitude Modulatory input strength
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current directly into the neuron
     * 
     * @param current External current in nanoAmperes (nA)
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current injected into the neuron (from sensory/input)
     * 
     * @return MembranePotential Total current accumulated this simulation step
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the accumulated current for the next simulation step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike event at the given timestamp
     * 
     * @param timestamp Time when spike occurred (in milliseconds)
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of recent spike timestamps
     * 
     * @return const std::vector<Timestamp>& Vector of spike times for STDP calculations
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history (reset for new experiment)
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse connection to this neuron
     * 
     * @param handle Handle/identifier of the incoming synapse
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse connection from this neuron
     * 
     * @param handle Handle/identifier of the outgoing synapse
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse connections
     * 
     * @return const std::vector<SynapseHandle>& List of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse connections
     * 
     * @return const std::vector<SynapseHandle>& List of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags configuration
     * 
     * @return const PlasticityFlags& Current plasticity configuration
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get non-const access to plasticity flags
     * 
     * @return PlasticityFlags& Reference to plasticity flags for modification
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Configure which plasticity rules are enabled
     * 
     * @param hebbian Enable Hebbian learning rule
     * @param stdp Enable Spike-Timing-Dependent Plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID this neuron belongs to
     * 
     * @param region Region identifier this neuron is part of
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID this neuron belongs to
     * 
     * @return RegionId Identifier of the neuron's region
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID this neuron belongs to
     * 
     * @param population Population identifier this neuron is part of
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID this neuron belongs to
     * 
     * @return PopulationId Identifier of the neuron's population
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step (default implementation)
     * 
     * @param currentTime Current simulation timestamp in milliseconds
     * 
     * @note TODO PHASE 2: Implement real integrate-and-fire dynamics
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset the neuron to its initial state
     * 
     * @note Restores membrane potential, clears spike history, resets counters
     */
    void reset();
    
    /**
     * @brief Initialize neuron with biologically plausible random parameters
     * 
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
