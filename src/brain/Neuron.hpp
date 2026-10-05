#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <mutex>
#include <memory>

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
// Thread-safe for concurrent access in multi-threaded simulations
class Neuron {
public:
    // Create neuron with ID
    /**
     * @brief Constructs a neuron with the given ID
     * @param id Unique identifier for the neuron
     */
    explicit Neuron(NeuronId id);
    
    /**
     * @brief Destructor
     * @note Uses RAII with smart pointer for automatic memory management
     */
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    /**
     * @brief Get the neuron's unique ID
     * @return NeuronId Unique identifier for this neuron
     */
    NeuronId getId() const;
    
    /**
     * @brief Get the neuron type
     * @return NeuronType Type of neuron (Internal, External, Input, Output)
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron type
     * @param type New neuron type to set
     */
    void setType(NeuronType type);
    
    /**
     * @brief Get the neuron state (read-only)
     * @return const NeuronState& Constant reference to neuron state
     */
    const NeuronState& getState() const;
    
    /**
     * @brief Get the neuron state (mutable)
     * @return NeuronState& Mutable reference to neuron state
     * @warning Thread-safe when using getState() const variant
     */
    NeuronState& getState();
    
    /**
     * @brief Get the membrane potential
     * @return MembranePotential Current membrane potential in mV
     */
    MembranePotential getMembranePotential() const;
    
    /**
     * @brief Set the membrane potential
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add delta to the membrane potential
     * @param delta Value to add in mV
     */
    void addToMembranePotential(MembranePotential delta);
    
    /**
     * @brief Get the firing threshold
     * @return MembranePotential Firing threshold in mV
     */
    MembranePotential getThreshold() const;
    
    /**
     * @brief Set the firing threshold
     * @param threshold New threshold in mV
     */
    void setThreshold(MembranePotential threshold);
    
    /**
     * @brief Check if neuron is currently firing
     * @return bool true if neuron is in firing state
     */
    bool isFiring() const;
    
    /**
     * @brief Check if neuron is in refractory period
     * @return bool true if neuron is refractory
     */
    bool isRefractory() const;
    
    /**
     * @brief Set the firing state
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the refractory period duration
     * @param steps Number of simulation steps for refractory period
     */
    void setRefractoryPeriod(uint32_t steps);
    
    /**
     * @brief Decrement refractory counter and check if period expired
     * @note Updates firing state if refractory period completes
     */
    void decrementRefractory();
    
    /**
     * @brief Get the firing rate
     * @return FiringRate Current firing rate in Hz
     */
    FiringRate getFiringRate() const;
    
    /**
     * @brief Set the firing rate
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);
    
    /**
     * @brief Set the leak conductance
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);
    
    /**
     * @brief Get the leak conductance
     * @return MembranePotential Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;
    
    /**
     * @brief Get the refractory period duration
     * @return uint32_t Total refractory period in steps
     */
    uint32_t getRefractoryPeriod() const;
    
    /**
     * @brief Set the resting potential
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);
    
    /**
     * @brief Get the resting potential
     * @return MembranePotential Resting potential in mV
     */
    MembranePotential getRestingPotential() const;
    
    /**
     * @brief Set the reset potential
     * @param potential Reset potential in mV (post-spike value)
     */
    void setResetPotential(MembranePotential potential);
    
    /**
     * @brief Get the reset potential
     * @return MembranePotential Reset potential in mV
     */
    MembranePotential getResetPotential() const;
    
    /**
     * @brief Check if membrane potential exceeds threshold
     * @return bool true if neuron would fire
     */
    bool checkThreshold() const;
    
    /**
     * @brief Get the timestamp of last spike
     * @return float Last spike time in ms (-1.0 if no spikes)
     */
    float getLastSpikeTime() const;
    
    /**
     * @brief Execute one LIF simulation step
     * @param currentTime Current simulation time in ms
     * @param dt Timestep duration in ms
     * @return bool true if neuron fired during this step
     * @note Thread-safe with internal mutex protection
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Receive excitatory synaptic input
     * @param amplitude Synaptic conductance * reversal potential contribution
     */
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive inhibitory synaptic input
     * @param amplitude Synaptic conductance * reversal potential contribution
     */
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    /**
     * @brief Receive modulatory input
     * @param amplitude Neuromodulatory signal strength
     */
    void receiveModulatoryInput(MembranePotential amplitude);
    
    /**
     * @brief Inject external current
     * @param current External current in nA
     */
    void injectCurrent(MembranePotential current);
    
    /**
     * @brief Get total synaptic current
     * @return MembranePotential Total synaptic current in nA
     */
    MembranePotential getTotalCurrent() const;
    
    /**
     * @brief Clear total synaptic current
     */
    void clearTotalCurrent();
    
    /**
     * @brief Record a spike timestamp
     * @param timestamp Time of spike in ms
     */
    void recordSpike(Timestamp timestamp);
    
    /**
     * @brief Get recent spike history
     * @return const std::vector<Timestamp>& Vector of recent spike timestamps
     */
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    /**
     * @brief Clear spike history
     */
    void clearSpikeHistory();
    
    /**
     * @brief Add incoming synapse connection
     * @param handle Synapse handle identifier
     */
    void addIncomingSynapse(SynapseHandle handle);
    
    /**
     * @brief Add outgoing synapse connection
     * @param handle Synapse handle identifier
     */
    void addOutgoingSynapse(SynapseHandle handle);
    
    /**
     * @brief Get incoming synapse handles
     * @return const std::vector<SynapseHandle>& Vector of incoming synapse handles
     */
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    /**
     * @brief Get outgoing synapse handles
     * @return const std::vector<SynapseHandle>& Vector of outgoing synapse handles
     */
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    /**
     * @brief Get plasticity flags (read-only)
     * @return const PlasticityFlags& Constant reference to plasticity flags
     */
    const PlasticityFlags& getPlasticityFlags() const;
    
    /**
     * @brief Get plasticity flags (mutable)
     * @return PlasticityFlags& Mutable reference to plasticity flags
     * @warning Thread-safe when using getPlasticityFlags() const variant
     */
    PlasticityFlags& getPlasticityFlags();
    
    /**
     * @brief Enable plasticity mechanisms
     * @param hebbian Enable Hebbian plasticity
     * @param stdp Enable STDP plasticity
     * @param rewardModulated Enable reward-modulated plasticity
     */
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    /**
     * @brief Set the neuron's region ID
     * @param region Region identifier
     */
    void setRegionId(RegionId region);
    
    /**
     * @brief Get the neuron's region ID
     * @return RegionId Region identifier
     */
    RegionId getRegionId() const;
    
    /**
     * @brief Set the neuron's population ID
     * @param population Population identifier
     */
    void setPopulationId(PopulationId population);
    
    /**
     * @brief Get the neuron's population ID
     * @return PopulationId Population identifier
     */
    PopulationId getPopulationId() const;
    
    /**
     * @brief Update neuron for one simulation step
     * @param currentTime Current simulation time in ms
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset neuron to initial state
     */
    void reset();
    
    /**
     * @brief Initialize neuron with random parameters
     * @param rng Random number generator reference
     */
    void initializeRandom(RandomGenerator& rng);
    
private:
    /**
     * @brief Neuron implementation details (pImpl idiom)
     * @note Uses std::unique_ptr for automatic memory management
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Mutex for thread safety
     * @note Protects all accesses to shared neuron state
     */
    mutable std::mutex mutex;
};

} // namespace nlm
