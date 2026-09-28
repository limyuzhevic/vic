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
     * @brief Construct a new Neuron object with specified ID
     * @param id Unique identifier for the neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destroy the Neuron object
     */
    ~Neuron();
    
    /**
     * @brief Disable copying - neurons cannot be copied
     */
    Neuron(const Neuron&) = delete;
    
    /**
     * @brief Disable copy assignment - neurons cannot be copied
     */
    Neuron& operator=(const Neuron&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * @param other Neuron to move from
     */
    Neuron(Neuron&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * @param other Neuron to move from
     * @return Neuron& Reference to this object
     */
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this neuron
     * @return NeuronId The neuron's ID
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type (Internal, Sensory, Motor, etc.)
     * @return NeuronType The type of neuron
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * @param type The neuron type to set
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the current state of the neuron
     * @return const NeuronState& Reference to the neuron's state (read-only)
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the current state of the neuron
     * @return NeuronState& Reference to the neuron's state
     */
    NeuronState& getState();
    
    /**
     * @brief Get the current membrane potential
     * @return MembranePotential The membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * @param potential The membrane potential value in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add a delta to the membrane potential
     * @param delta The change in membrane potential in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold voltage
     * @return MembranePotential The threshold voltage in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold voltage
     * @param threshold The threshold voltage in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if the neuron is currently firing (spiking)
     * @return bool true if neuron is firing, false otherwise
     */
    bool isFiring() const;
    
    /**
     * @brief Check if the neuron is in refractory period
     * @return bool true if in refractory period, false otherwise
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state of the neuron
     * @param state The firing state (Resting, Active, Refractory)
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * @param steps Number of simulation steps for refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement the refractory period counter
     * Decreases refractoryRemaining by 1 and updates state if period ends
     */
    void decrementRefractory();
    
    /**
     * @brief Get the current firing rate
     * @return FiringRate The firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * @param rate The firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance (membrane resistance)
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance (membrane resistance)
     * @return MembranePotential The leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * @return uint32_t Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * @param potential The resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * @return MembranePotential The resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential (voltage after spike)
     * @param potential The reset potential in mV
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Check if membrane potential exceeds firing threshold
     * @return bool true if neuron should fire, false otherwise
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of the last spike
     * @return float Time of last spike in simulation steps (-1 if none)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute one step of the Leaky Integrate-and-Fire dynamics
     * @param currentTime Current simulation time in ms
     * @param dt Time step in ms
     * @return bool true if neuron fired during this step, false otherwise
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic conductance contribution in mV
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic conductance contribution in mV (positive value represents inhibition)
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory (neuromodulatory) input
     * @param amplitude Neuromodulatory signal contribution in mV
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current (e.g., from sensory input)
     * @param current Current injection in nS
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get the total current input accumulated this step
     * @return MembranePotential Total current in nS
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear the total current input for the next step
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike occurrence
     * @param timestamp Time when spike occurred
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get the history of spike times
     * @return const std::vector<Timestamp>& Vector of spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear the spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add an incoming synapse handle
     * @param handle Synapse handle for incoming connection
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add an outgoing synapse handle
     * @param handle Synapse handle for outgoing connection
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get all incoming synapse handles
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get all outgoing synapse handles
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * @return const PlasticityFlags& Reference to plasticity flags (read-only)
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get the plasticity flags for this neuron
     * @return PlasticityFlags& Reference to plasticity flags
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable specific plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the region ID this neuron belongs to
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the region ID this neuron belongs to
     * @return RegionId The neuron's region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the population ID this neuron belongs to
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the population ID this neuron belongs to
     * @return PopulationId The neuron's population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step using default LIF timestep
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial/default state
     */
    void reset();
    
    /**
     * @brief Initialize neuron parameters with random values
     * Uses biological constraints and RandomGenerator for realistic initialization
     * @param rng Random number generator for reproducible initialization
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
